/*
 * Fibonacci - Versión SSE2 (SIMD)
 * Benchmark Reducido F-II - Arquitectura de Computadores
 * Universidad de Alicante
 *
 * Calcula 4 series de Fibonacci en paralelo usando registros XMM de 128 bits.
 * Instrucciones SSE2: PADDD, MOVDQA, MOVDQU
 */

#include <emmintrin.h>

#define N_SSE    400
#define NUM_SERIES 4

void fibonacci_sse(int inicio[NUM_SERIES], int resultado[NUM_SERIES][N_SSE]) {
    __m128i xmm0, xmm1, xmm2;
    /* 16-byte aligned buffer: required for _mm_store_si128 and avoids
       stack corruption detected by MSVC /RTC1 in Debug x86 builds */
    __declspec(align(16)) int temp[4];

    /* fib[0] = 0 para las 4 series */
    xmm0 = _mm_set_epi32(0, 0, 0, 0);

    /* fib[1] = valor inicial de cada serie */
    xmm1 = _mm_set_epi32(inicio[3], inicio[2], inicio[1], inicio[0]);

    for (int s = 0; s < NUM_SERIES; s++) {
        resultado[s][0] = 0;
        resultado[s][1] = inicio[s];
    }

    for (int i = 2; i < N_SSE; i++) {
        /* PADDD: suma 4 enteros de 32 bits en paralelo */
        xmm2 = _mm_add_epi32(xmm0, xmm1);

        /* MOVDQA: guardar 128 bits en memoria alineada */
        _mm_store_si128((__m128i*)temp, xmm2);

        for (int s = 0; s < NUM_SERIES; s++)
            resultado[s][i] = temp[s];

        /* Deslizar ventana */
        xmm0 = xmm1;
        xmm1 = xmm2;
    }
}