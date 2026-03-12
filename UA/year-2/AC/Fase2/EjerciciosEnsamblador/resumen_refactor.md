# Benchmark Fibonacci — Resumen de problemas y soluciones
> Arquitectura de Computadores · Universidad de Alicante  
> Plataforma: Visual Studio 2022 · x86 (Win32) · C++14

---

## Problema 1 — ASM mostraba `-858993460` (`0xCCCCCCCC`)

### Causa
`0xCCCCCCCC` es el patrón que MSVC usa en Debug para rellenar
memoria no inicializada. El error estaba en esta línea:

```asm
lea  esi, fib   ; ❌ Carga la dirección DEL PUNTERO en la pila,
                ;    no la dirección del array real
```

`fib` como parámetro es un **puntero** (4 bytes en la pila que
contienen la dirección del array). `LEA` cargaba la dirección de
ese puntero, no su valor. ESI apuntaba a la pila, no al array.

```
PILA durante fibonacci_asm:
[ebp+8] → fib_ptr = 0x0045FC20   ← LEA apuntaba aquí (MAL)
                        │
                        ▼
               [ array real en main() ]  ← MOV carga este valor (BIEN)
```

### Solución
```asm
; ANTES ❌
lea  esi, fib

; DESPUÉS ✅
mov  esi, fib   ; Desreferencia el puntero → dirección real del array
```

---

## Problema 2 — Crash: `Run-Time Check Failure #2 - Stack corrupted`

### Causa
`_mm_store_si128` genera `MOVDQA`, que **exige alineación de 16 bytes**
en la dirección de destino. En modo Debug, MSVC activa `/RTC1`,
que inserta variables de guarda entre las variables de la pila,
rompiendo la alineación de `temp` aunque tenga `__declspec(align(16))`:

```
PILA en Debug con /RTC1:
┌──────────────────────────────┐
│  [guarda RTC]  ← insertada   │
│  temp[4]       ← desalineado │  → MOVDQA → CRASH
│  [guarda RTC]                │
└──────────────────────────────┘
```

### Solución
```cpp
// ANTES ❌
_mm_store_si128((__m128i*)temp, xmm2);   // MOVDQA — requiere alineación 16B

// DESPUÉS ✅
_mm_storeu_si128((__m128i*)temp, xmm2);  // MOVDQU — sin requisito de alineación
```

---

## Problema 3 — SSE tardaba 3× más que C puro

### Causa A — Layout del array causaba thrashing de caché
```cpp
// Layout ANTIGUO: [NUM_SERIES][N_SSE]
int resultado[4][400];

// Escrituras por cada iteración i:
resultado[0][i]  // base + 0×1600 + i×4
resultado[1][i]  // base + 1×1600 + i×4  ← salto de 1600 bytes
resultado[2][i]  // base + 2×1600 + i×4  ← salto de 1600 bytes
resultado[3][i]  // base + 3×1600 + i×4  ← salto de 1600 bytes
// = 4 líneas de caché distintas cargadas por iteración
```

### Causa B — Bucle interior anulaba el paralelismo SIMD
```cpp
// 4 escrituras secuenciales dispersas que anulan el beneficio de PADDD
for (int s = 0; s < NUM_SERIES; s++)
    resultado[s][i] = temp[s];
// Con 10.000.000 × 400 = 4.000 millones de escrituras dispersas
```

### Solución — Tres cambios combinados

**1. Cambiar el layout a `[N_SSE][NUM_SERIES]`**

Las 4 series quedan contiguas en memoria (16 bytes = 1 línea de caché):

```
ANTES [NUM_SERIES][N_SSE]:
[Serie0: 400 ints · 1600B][Serie1: 400 ints · 1600B]...
 ← 1600 bytes de salto entre series →

DESPUÉS [N_SSE][NUM_SERIES]:
[i=0: s0,s1,s2,s3][i=1: s0,s1,s2,s3][i=2: s0,s1,s2,s3]...
 ← 16 bytes contiguos = 1 sola línea de caché →
```

**2. Eliminar el bucle interior y `temp`, escribir directo a `resultado[i]`**

```cpp
// ANTES ❌
_mm_storeu_si128((__m128i*)temp, xmm2);
for (int s = 0; s < NUM_SERIES; s++)
    resultado[s][i] = temp[s];

// DESPUÉS ✅
_mm_storeu_si128((__m128i*)resultado[i], xmm2);  // 1 instrucción, 16 bytes contiguos
```

**3. Actualizar el `printf` en `main` para el nuevo layout**

```cpp
// ANTES ❌
resultado[0][i], resultado[1][i], resultado[2][i], resultado[3][i]

// DESPUÉS ✅
resultado[i][0], resultado[i][1], resultado[i][2], resultado[i][3]
```

---

## Problema 4 — SSE seguía siendo lenta tras los fixes de código

### Causa
Se estaba compilando en **Debug** (`/Od` + `/RTC1` + `/ZI`).
El overhead de Debug enmascaraba completamente las mejoras de SSE.

| Flag    | Efecto en Debug                            |
|---------|--------------------------------------------|
| `/Od`   | Sin optimización — código genérico         |
| `/RTC1` | Comprobaciones de stack en cada función    |
| `/ZI`   | Información completa de depuración         |

### Solución
Cambiar la configuración en la barra superior de Visual Studio:

```
[Debug ▼]  →  [Release ▼]     (mantener Win32, NO cambiar a x64)
```

> ⚠️ Cambiar a x64 rompe el `__asm` inline de la versión ASM.

---

## Problema 5 — Release con `/O2` transformaba el código

### Causa
`/O2` permite al compilador reorganizar, vectorizar y hacer inlining,
lo que no es representativo del código escrito manualmente.

### Solución
En **Propiedades del proyecto (Release · Win32)**:

- `C/C++ > Optimización > Optimización` → **Deshabilitado (`/Od`)**
- `Vinculador > Optimización > Generación de código en tiempo de vínculo` → **Predeterminada**

| Configuración      | `/RTC1` | `0xCCCC` | Sin optimización |
|--------------------|---------|----------|-----------------|
| Debug              | ✅      | ✅       | ✅              |
| **Release + `/Od`**| ❌      | ❌       | ✅  ← objetivo  |
| Release + `/O2`    | ❌      | ❌       | ❌              |

---

## Mejora adicional — Subir N a 800

### Viabilidad (todo en la pila, límite MSVC = 1 MB)

```
long long fib[800]     =  800 × 8  =   6.400 bytes  ✅
int fib[800]           =  800 × 4  =   3.200 bytes  ✅
int resultado[800][4]  =  800 × 16 =  12.800 bytes  ✅
─────────────────────────────────────────────────────
TOTAL                              =  22.400 bytes   << 1 MB
```

### Cambio
```cpp
#define N      800   // antes 500
#define N_SSE  800   // antes 400 — se igualan con el nuevo layout
```

---

## Código final corregido

```cpp
#include <stdio.h>
#include <time.h>
#include <emmintrin.h>

#define N            800
#define N_SSE        800
#define NUM_SERIES     4
#define ITERACIONES  10000000

void fibonacci_c(long long fib[N]) {
    fib[0] = 0;
    fib[1] = 1;
    for (int i = 2; i < N; i++)
        fib[i] = fib[i - 1] + fib[i - 2];
}

void fibonacci_asm(int fib[N]) {
    __asm {
        mov  esi, fib          /* FIX 1: MOV en lugar de LEA */
        mov  eax, 0
        mov  ebx, 1
        mov  [esi], eax
        mov  [esi + 4], ebx
        mov  ecx, N
        sub  ecx, 2
        add  esi, 8
        fib_loop:
            mov  edx, ebx
            add  edx, eax
            mov  [esi], edx
            add  esi, 4
            mov  eax, ebx
            mov  ebx, edx
            loop fib_loop
    }
}

void fibonacci_sse(int inicio[NUM_SERIES], int resultado[N_SSE][NUM_SERIES]) {
    __m128i xmm0, xmm1, xmm2;

    xmm0 = _mm_setzero_si128();
    xmm1 = _mm_set_epi32(inicio[3], inicio[2], inicio[1], inicio[0]);

    _mm_storeu_si128((__m128i*)resultado[0], xmm0);
    _mm_storeu_si128((__m128i*)resultado[1], xmm1);

    for (int i = 2; i < N_SSE; i++) {
        xmm2 = _mm_add_epi32(xmm0, xmm1);
        _mm_storeu_si128((__m128i*)resultado[i], xmm2);  /* FIX 2+3: storeu + directo */
        xmm0 = xmm1;
        xmm1 = xmm2;
    }
}

int main() {
    clock_t inicio, fin;
    double tiempo;

    printf("=== Benchmark Fibonacci ===\n");

    {
        long long fib[N];
        printf("\n--- VERSION 1: C puro ---\n");
        inicio = clock();
        for (int iter = 0; iter < ITERACIONES; iter++) fibonacci_c(fib);
        fin = clock();
        for (int i = 0; i < 15; i++) printf("  fib[%2d] = %lld\n", i, fib[i]);
        printf("Tiempo: %.4f s\n", (double)(fin - inicio) / CLOCKS_PER_SEC);
    }
    {
        int fib[N];
        printf("\n--- VERSION 2: ASM x86 ---\n");
        inicio = clock();
        for (int iter = 0; iter < ITERACIONES; iter++) fibonacci_asm(fib);
        fin = clock();
        for (int i = 0; i < 15; i++) printf("  fib[%2d] = %d\n", i, fib[i]);
        printf("Tiempo: %.4f s\n", (double)(fin - inicio) / CLOCKS_PER_SEC);
    }
    {
        int resultado[N_SSE][NUM_SERIES];
        int inicioSeries[NUM_SERIES] = { 1, 2, 3, 5 };
        printf("\n--- VERSION 3: SSE2 ---\n");
        inicio = clock();
        for (int iter = 0; iter < ITERACIONES; iter++) fibonacci_sse(inicioSeries, resultado);
        fin = clock();
        for (int i = 0; i < 10; i++)
            printf("  [%2d] %d  %d  %d  %d\n",
                i, resultado[i][0], resultado[i][1], resultado[i][2], resultado[i][3]);
        printf("Tiempo: %.4f s\n", (double)(fin - inicio) / CLOCKS_PER_SEC);
    }
    return 0;
}
```

---

## Tabla resumen de todos los cambios

| # | Versión | Problema | Causa raíz | Solución |
|---|---------|----------|------------|----------|
| 1 | ASM | Valores `0xCCCCCCCC` | `lea esi, fib` cargaba dirección del puntero | `mov esi, fib` |
| 2 | SSE | Stack corruption crash | `MOVDQA` exige alineación 16B, `/RTC1` la rompe | `_mm_storeu_si128` (`MOVDQU`) |
| 3 | SSE | 3× más lenta que C | Layout `[series][N]` → fallos de caché + bucle interior | Layout `[N][series]` + store directo |
| 4 | Todas | Mejoras sin efecto | Compilando en Debug (`/RTC1` + overhead) | Cambiar a Release |
| 5 | Todas | Release demasiado rápida | `/O2` transforma el código | Release con `/Od` |
| 6 | Todas | N limitado a 500/400 | Decisión de diseño | `N = N_SSE = 800` (22 KB en pila, seguro) |