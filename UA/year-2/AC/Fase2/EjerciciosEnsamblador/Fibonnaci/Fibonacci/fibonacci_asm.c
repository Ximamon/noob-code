/*
 * Fibonacci - Benchmark Unificado F-II - Arquitectura de Computadores
 * Universidad de Alicante
 *
 * Punto central: main() ejecuta las tres versiones y compara tiempos.
 *   VERSION 1: C puro             (fibonacci_c.c)
 *   VERSION 2: Ensamblador x86    (este fichero, inline MSVC)
 *   VERSION 3: SSE2 SIMD          (fibonacci_sse.c)
 *
 * Instrucciones x86 usadas en version ASM:
 *   MOV  - transferencia de datos
 *   ADD  - suma aritmetica
 *   SUB  - resta (inicializar contador)
 *   LOOP - decrementa ECX y salta si ECX != 0
 *
 * NOTA: MSVC no soporta __asm en x64. Compilar como x86 (Win32).
 *       Se usan registros de 32 bits: EAX, EBX, ECX, EDX, ESI.
 *       La version ASM opera con int (32 bits): desbordamiento esperado
 *       a partir del termino 46, comportamiento normal en este benchmark.
 */

#include <stdio.h>
#include <time.h>
#include <emmintrin.h>

#define N          500
#define N_SSE      400
#define NUM_SERIES   4
#define ITERACIONES  5000000

/* ============================================================
 * Declaraciones de funciones definidas en otros ficheros
 * ============================================================ */
void fibonacci_c(long long fib[N]);
void fibonacci_sse(int inicio[NUM_SERIES], int resultado[NUM_SERIES][N_SSE]);

/* ============================================================
 * VERSION 2: Fibonacci en Ensamblador x86 inline (MSVC)
 * Opera con registros de 32 bits (int). Desbordamiento esperado
 * a partir del termino 46 (fib[46] > INT_MAX).
 * ============================================================ */
void fibonacci_asm(int fib[N]) {
    __asm {
        /* Cargar direccion base del array en ESI */
        lea  esi, fib

        /* --- Inicializar fib[0]=0, fib[1]=1 --- */
        mov  eax, 0               /* EAX = 0 (fib[0]) */
        mov  ebx, 1               /* EBX = 1 (fib[1]) */

        /* Guardar fib[0] y fib[1] en memoria */
        mov [esi], eax            /* fib[0] = EAX */
        mov [esi + 4], ebx        /* fib[1] = EBX */

        /* ECX = N - 2 (contador de terminos restantes) */
        mov  ecx, N
        sub  ecx, 2               /* ECX = 498 */

        /* Avanzar puntero a fib[2] (2 * 4 bytes = 8) */
        add  esi, 8

        fib_loop:
        /* EDX = EAX + EBX  =>  fib[i] = fib[i-2] + fib[i-1] */
        mov  edx, ebx             /* EDX = EBX */
        add  edx, eax             /* EDX = EAX + EBX */

        /* Guardar resultado en memoria */
        mov [esi], edx            /* fib[i] = EDX */
        add  esi, 4               /* avanzar al siguiente elemento */

        /* Deslizar ventana */
        mov  eax, ebx             /* EAX = fib[i-1] */
        mov  ebx, edx             /* EBX = fib[i]   */

        /* LOOP: ECX--, si ECX != 0 salta a fib_loop */
        loop fib_loop
    }
}

/* ============================================================
 * MAIN: Ejecuta y compara las tres versiones
 * ============================================================ */
int main() {
    clock_t inicio, fin;
    double tiempo;

    printf("=== Benchmark Fibonacci - Arquitectura de Computadores ===\n");
    printf("Terminos: %d (C/ASM) | %d (SSE) | Iteraciones: %d\n", N, N_SSE, ITERACIONES);

    /* ----------------------------------------------------------
     * VERSION 1: C puro
     * ---------------------------------------------------------- */
    {
        long long fib[N];
        printf("\n--- VERSION 1: C puro ---\n");
        inicio = clock();
        for (int iter = 0; iter < ITERACIONES; iter++)
            fibonacci_c(fib);
        fin = clock();
        tiempo = (double)(fin - inicio) / CLOCKS_PER_SEC;

        printf("Primeros 15 terminos:\n");
        for (int i = 0; i < 15; i++)
            printf("  fib[%2d] = %lld\n", i, fib[i]);
        printf("Tiempo: %.4f segundos\n", tiempo);
    }

    /* ----------------------------------------------------------
     * VERSION 2: Ensamblador x86 inline
     * ---------------------------------------------------------- */
    {
        int fib[N];
        printf("\n--- VERSION 2: Ensamblador x86 inline (MSVC) ---\n");
        printf("(registros 32 bits: desbordamiento esperado a partir de fib[46])\n");
        inicio = clock();
        for (int iter = 0; iter < ITERACIONES; iter++)
            fibonacci_asm(fib);
        fin = clock();
        tiempo = (double)(fin - inicio) / CLOCKS_PER_SEC;

        printf("Primeros 15 terminos:\n");
        for (int i = 0; i < 15; i++)
            printf("  fib[%2d] = %d\n", i, fib[i]);
        printf("Tiempo: %.4f segundos\n", tiempo);

        printf("Instrucciones x86 usadas:\n");
        printf("  MOV  : Transferencia de datos (registros <-> memoria)\n");
        printf("  ADD  : Suma aritmetica (EAX + EBX -> EDX)\n");
        printf("  SUB  : Resta (inicializar contador ECX)\n");
        printf("  LOOP : Decrementa ECX y salta si ECX != 0\n");
    }

    /* ----------------------------------------------------------
     * VERSION 3: SSE2 (SIMD) - 4 series en paralelo
     * ---------------------------------------------------------- */
    {
        int resultado[NUM_SERIES][N_SSE];
        int inicioSeries[NUM_SERIES] = {1, 2, 3, 5};
        printf("\n--- VERSION 3: SSE2 (SIMD) - %d series en paralelo ---\n", NUM_SERIES);
        inicio = clock();
        for (int iter = 0; iter < ITERACIONES; iter++)
            fibonacci_sse(inicioSeries, resultado);
        fin = clock();
        tiempo = (double)(fin - inicio) / CLOCKS_PER_SEC;

        printf("Primeros 10 terminos de cada serie:\n");
        printf("%-4s  %-20s %-20s %-20s %-20s\n",
               "i", "Serie0(fib1=1)", "Serie1(fib1=2)", "Serie2(fib1=3)", "Serie3(fib1=5)");
        printf("------------------------------------------------------------------------------------\n");
        for (int i = 0; i < 10; i++) {
            printf("%-4d  %-20d %-20d %-20d %-20d\n",
                   i, resultado[0][i], resultado[1][i], resultado[2][i], resultado[3][i]);
        }
        printf("Tiempo: %.4f segundos\n", tiempo);

        printf("Instrucciones SSE2 usadas:\n");
        printf("  _mm_set_epi32    -> MOVDQA  / cargar 4 enteros en XMM\n");
        printf("  _mm_add_epi32    -> PADDD   / suma paralela 4 x 32 bits\n");
        printf("  _mm_storeu_si128 -> MOVDQU  / guardar 128 bits en memoria\n");
    }

    return 0;
}

/*
 * Compilar en Visual Studio:
 *   - Plataforma: x86 (Win32), NO x64
 *   - Configuracion: Debug o Release con /O0
 */