#include <iostream>

// Macro protectora de errores
#define CUDA_CHECK(call) { \
    cudaError_t estado = (call); \
    if (estado != cudaSuccess) { \
        std::cerr << "Error de CUDA en " << #call << ": " << cudaGetErrorString(estado) << std::endl; \
        exit(EXIT_FAILURE); \
    } \
}

int main() {
    cudaDeviceProp propiedades;

    // Consultar las propiedades del dispositivo 0 de forma segura
    CUDA_CHECK(cudaGetDeviceProperties(&propiedades, 0));

    // Imprimir algunas propiedades del dispositivo
    std::cout << "Nombre del dispositivo: " << propiedades.name << std::endl;
    std::cout << "Memoria global total: " << propiedades.totalGlobalMem / (1024 * 1024) << " MB" << std::endl;
    std::cout << "Max Hilos por bloque: " << propiedades.maxThreadsPerBlock << std::endl;
    std::cout << "Número de multiprocesadores (SMs): " << propiedades.multiProcessorCount << std::endl;
}