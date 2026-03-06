/*
 * Fibonacci - Versión C puro
 * Benchmark Reducido F-II - Arquitectura de Computadores
 * Universidad de Alicante
 */

#include <stdio.h>

#define N 500

void fibonacci_c(long long fib[N]) {
    fib[0] = 0;
    fib[1] = 1;
    for (int i = 2; i < N; i++) {
        fib[i] = fib[i-1] + fib[i-2];
    }
}