#include <stdio.h>

__global__ void hello_gpu() {
    printf("¡Plugin funcionando! Hilo %d\n", threadIdx.x);
}

int main() {
    // Lanzamos 1 bloque con 5 hilos
    hello_gpu<<<1, 5>>>();
    cudaDeviceSynchronize();
    return 0;
}