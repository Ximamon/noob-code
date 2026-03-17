/*
 * Fibonacci - Benchmark Reducido F-II - Arquitectura de Computadores
 * Universidad de Alicante
 *
 * Algoritmos: Fibonacci
 *   Se trata de una secuencia de numeros donde cada termino es la suma de los dos anteriores
 * 
 * Punto central: main() ejecuta las tres versiones y compara tiempos.
 *   VERSION 1: C puro
 *   VERSION 2: Ensamblador x86 inline (MSVC)
 *   VERSION 3: SSE2 SIMD
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
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <emmintrin.h>          // Cabecera para intrinsecos de SSE2

#define NUM_SERIES     4        // Numero de series de Fibonacci paralelas en version SSE (4 por el tamaño de los registros XMM)
#define ITERACIONES  10000000   // Numero de iteraciones para el benchmark (ajustar segun el tiempo de ejecucion)

 // Flags para mostrar resultados, tiempos y informacion adicional
#define SHOW_OUTPUT     0       // Mostrar los primeros terminos de cada serie (1 = SI, 0 = NO)
#define SHOW_TIMES      0       // Mostrar tiempos de ejecucion de cada version (1 = SI, 0 = NO)

 /* ============================================================
  * VERSION 1: Fibonacci en C puro
  * ============================================================ */
void fibonacci_c(uint32_t *fib, int n_actual, uint32_t inicio) {
    fib[0] = 0;
    fib[1] = inicio;
    for (int i = 2; i < n_actual; i++) {
        fib[i] = fib[i - 1] + fib[i - 2];
    }
}

/* ============================================================
 * VERSION 2: Fibonacci en Ensamblador x86 inline (MSVC)
 * Opera con registros de 32 bits (int). Desbordamiento esperado
 * a partir del termino 46 (fib[46] > INT_MAX).
 * ============================================================ */
#ifdef __cplusplus
extern "C"
#endif
void fibonacci_asm(uint32_t* fib, int n_actual, uint32_t inicio);

/* ============================================================
 * VERSION 3: Fibonacci SSE2 (SIMD)
 * Calcula 4 series de Fibonacci en paralelo usando registros XMM
 * de 128 bits. Instrucciones SSE2: PADDD, MOVDQA, MOVDQU
 * ============================================================ */
void fibonacci_sse(uint32_t inicio[NUM_SERIES], uint32_t (*resultado)[NUM_SERIES], int n_actual) {
    __m128i xmm0, xmm1, xmm2;

    /* fib[0] = 0 para las 4 series */
    xmm0 = _mm_set_epi32(0, 0, 0, 0);

    /* fib[1] = valor inicial de cada serie */
    xmm1 = _mm_set_epi32(inicio[3], inicio[2], inicio[1], inicio[0]);

    /* Guardar fib[0] y fib[1] directamente con MOVDQU (128 bits de golpe) */
	_mm_storeu_si128((__m128i*)resultado[0], xmm0); // fib[0] = 0 para las 4 series
	_mm_storeu_si128((__m128i*)resultado[1], xmm1); // fib[1] = valor inicial de cada serie

    for (int i = 2; i < n_actual; i++) {
        /* PADDD: suma 4 enteros de 32 bits en paralelo */
        xmm2 = _mm_add_epi32(xmm0, xmm1);

        /* MOVDQA: guardar 128 bits en memoria alineada */
        _mm_storeu_si128((__m128i*)resultado[i], xmm2);

        // Este bucle anula el paralelismo
        // for (int s = 0; s < NUM_SERIES; s++)
        //     resultado[s][i] = temp[s];

        /* Deslizar ventana */
        xmm0 = xmm1;
        xmm1 = xmm2;
    }
}

/* ============================================================
 * MAIN: Ejecuta y compara las tres versiones
 * ============================================================ */
int main() {
    clock_t inicio, fin;
    double tiempo;

    printf("=== Benchmark Fibonacci - Arquitectura de Computadores ===\n");

    printf("\nInstrucciones x86 usadas:\n");
    printf("  MOV  : Transferencia de datos (registros <-> memoria)\n");
    printf("  ADD  : Suma aritmetica (EAX + EBX -> EDX)\n");
    printf("  SUB  : Resta (inicializar contador ECX)\n");
    printf("  LOOP : Decrementa ECX y salta si ECX != 0\n");

    printf("Instrucciones SSE2 usadas:\n");
    printf("  _mm_set_epi32    -> MOVDQA  / cargar 4 enteros en XMM\n");
    printf("  _mm_add_epi32    -> PADDD   / suma paralela 4 x 32 bits\n");
    printf("  _mm_storeu_si128 -> MOVDQU  / guardar 128 bits en memoria\n");


	// Tamaños de N para cada test (ajustar array según el numero de test deseados)
    // 10 -> Benchmark completo
	// 5  -> Benchmark reducido (para pruebas rápidas)
	// 1  -> Solo un test (N=100) para verificar resultados sin esperar tiempos largos
    int tam_N[] = { 100, 200, 400, 800, 1600, 6400, 25600, 102400, 409600, 1638400};
    const int num_test = 10;

    uint32_t inicioSeries[4] = { 1, 2, 3, 5 };

    double resultados[num_test][3]; // [Fila: Num Test][Columna: Versión C, ASM, SSE]

    for (int i = 0; i < num_test; i++) {

		int actual_N = tam_N[i];

		int iter_actuales = 1000000000 / actual_N; // Ajustar iteraciones para tiempos razonables
		if (iter_actuales < 1) iter_actuales = 1; // Al menos 1 iteracion
        printf("\n=====================================================");
        printf("\n>>> TEST %d: N = %d terminos & Iteraciones = %d", i + 1, actual_N, iter_actuales);
        printf("\n=====================================================\n");


        /* ----------------------------------------------------------
        * VERSION 1: C puro
        * ---------------------------------------------------------- */
        {
            uint32_t* fib = (uint32_t*)malloc(actual_N * sizeof(uint32_t));

            if (SHOW_TIMES) printf("\n--- VERSION 1: C puro ---\n");

            inicio = clock();
            for (int iter = 0; iter < iter_actuales; iter++)
                for (int s = 0; s < 4; s++)
                    fibonacci_c(fib, actual_N, inicioSeries[s]);
            fin = clock();
            tiempo = (double)(fin - inicio) / CLOCKS_PER_SEC;

            resultados[i][0] = tiempo;

            if (SHOW_OUTPUT) {
                printf("Primeros 15 terminos:\n");
                // Antes de usar 'fib', verifica que la asignación fue exitosa
                if (fib != NULL) {
                    for (int i = 0; i < 15; i++)
                        printf("  fib[%2d] = %u\n", i, fib[i]);
                }
                else {
                    printf("Error: No se pudo asignar memoria para 'fib'.\n");
                }
            }

            if (SHOW_TIMES) printf("Tiempo: %.4f segundos\n", tiempo);

            free(fib);
        }

        /* ----------------------------------------------------------
         * VERSION 2: Ensamblador x86 inline
         * ---------------------------------------------------------- */
        {
            uint32_t* fib = (uint32_t*)malloc(actual_N * sizeof(uint32_t));

            if (SHOW_TIMES) printf("\n--- VERSION 2: Ensamblador x86 inline (MSVC) ---\n");
            if (SHOW_TIMES) printf("(registros 32 bits: desbordamiento esperado a partir de fib[46])\n");

            inicio = clock();
            for (int iter = 0; iter < iter_actuales; iter++)
                for (int s = 0; s < 4; s++)
                    fibonacci_asm(fib, actual_N, inicioSeries[s]);
            fin = clock();
            tiempo = (double)(fin - inicio) / CLOCKS_PER_SEC;

            resultados[i][1] = tiempo;

            if (SHOW_OUTPUT) {
                printf("Primeros 15 terminos:\n");
                // Antes de usar 'fib', verifica que la asignación fue exitosa
                if (fib != NULL) {
                    for (int i = 0; i < 15; i++)
                        printf("  fib[%2d] = %u\n", i, fib[i]);
                }
                else {
                    printf("Error: No se pudo asignar memoria para 'fib'.\n");
                }
            }
            
            if (SHOW_TIMES) printf("Tiempo: %.4f segundos\n", tiempo);

            free(fib);
        }

        /* ----------------------------------------------------------
         * VERSION 3: SSE2 (SIMD) - 4 series en paralelo
         * ---------------------------------------------------------- */
        {
            uint32_t (*resultado)[4] = (uint32_t (*)[4])malloc(actual_N * sizeof(*resultado));

            if (SHOW_TIMES) printf("\n--- VERSION 3: SSE2 (SIMD) - %d series en paralelo ---\n", NUM_SERIES);

            inicio = clock();
            for (int iter = 0; iter < iter_actuales; iter++)
                fibonacci_sse(inicioSeries, resultado, actual_N);
            fin = clock();
            tiempo = (double)(fin - inicio) / CLOCKS_PER_SEC;

            resultados[i][2] = tiempo;

            if (SHOW_OUTPUT) {
                printf("Primeros 10 terminos de cada serie:\n");
                printf("%-4s  %-20s %-20s %-20s %-20s\n",
                    "i", "Serie0 (fib1=1)", "Serie1 (fib1=2)", "Serie2 (fib1=3)", "Serie3 (fib1=5)");
                printf("------------------------------------------------------------------------------------\n");
                for (int i = 0; i < 10; i++) {
                    printf("%-4d  %-20u %-20u %-20u %-20u\n",
                        i, resultado[i][0], resultado[i][1], resultado[i][2], resultado[i][3]);
                }
            }
            
            if (SHOW_TIMES) printf("Tiempo: %.4f segundos\n", tiempo);

            free(resultado);
        }
    }

    printf("\n\n==============================================================\n");
    printf("   RESULTADOS FINALES - TIEMPO DE EJECUCION (Segundos)\n");
    printf("==============================================================\n");
    printf(" %-10s | %-12s | %-12s | %-12s\n", "N Terminos", "C (32-bit)", "ASM (32-bit)", "SSE (32-bit SIMD)");
    printf("------------|--------------|--------------|-------------------\n");

    for (int i = 0; i < num_test; i++) {
        printf(" %-10d | %-12.4f | %-12.4f | %-12.4f\n",
            tam_N[i], resultados[i][0], resultados[i][1], resultados[i][2]);
    }
    printf("=============================================================\n");

    return 0;
}

/*
 * Compilar en Visual Studio:
 *   - Plataforma: x86 (Win32), NO x64
 *   - Configuracion: Debug o Release con /O0
 */
