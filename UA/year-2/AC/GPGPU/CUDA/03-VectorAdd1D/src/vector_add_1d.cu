#include <iostream>
#include <vector>
#include <cmath>

// Macro de seguridad
#define CUDA_CHECK(call) { \
    cudaError_t estado = (call); \
    if (estado != cudaSuccess) { \
        std::cerr << "Error de CUDA en " << #call << ": " << cudaGetErrorString(estado) << std::endl; \
        exit(EXIT_FAILURE); \
    } \
}

// Kernel (Device) para sumar dos vectores
__global__ void vectorAdd(const float *A, const float *B, float *C, int N) {
    // 1. Calcular el índice global del hilo
    int idx = blockIdx.x * blockDim.x + threadIdx.x;

    // 2. Verificar que el índice esté dentro del rango
    if (idx < N) {
        // 3. Realizar la suma de los elementos correspondientes
        C[idx] = A[idx] + B[idx];
    }
}

// Función principal Host (CPU)
int main() {
    // Definir el tamaño del vector
    int N = 10000;
    size_t size = N * sizeof(float);

    // Vectores en el Host
    std::vector<float> h_A(N, 1.0f); // Inicializar con 1.0f
    std::vector<float> h_B(N, 2.5f); // Inicializar con 2.5f
    std::vector<float> h_C(N); // Vector resultado

    // Punteros para el Device
    float *d_A, *d_B, *d_C;

    // 1. Reservar memoria en el Device (VRAM)
    CUDA_CHECK(cudaMalloc(&d_A, size));
    CUDA_CHECK(cudaMalloc(&d_B, size));
    CUDA_CHECK(cudaMalloc(&d_C, size));

    // 2. Copiar los datos del Host al Device
    CUDA_CHECK(cudaMemcpy(d_A, h_A.data(), size, cudaMemcpyHostToDevice));
    CUDA_CHECK(cudaMemcpy(d_B, h_B.data(), size, cudaMemcpyHostToDevice));

    // 3. Configurar la topología de ejecución - 1D
    int threadsPerBlock = 256;
    int blocksPerGrid = (N + threadsPerBlock - 1) / threadsPerBlock;

    // 4. Lanzar el kernel
    vectorAdd<<<blocksPerGrid, threadsPerBlock>>>(d_A, d_B, d_C, N);

    // 5. Sincronizar y capturar posibles errores de ejecución
    CUDA_CHECK(cudaGetLastError());
    CUDA_CHECK(cudaDeviceSynchronize());

    // 6. Copiar el resultado del Device al Host
    CUDA_CHECK(cudaMemcpy(h_C.data(), d_C, size, cudaMemcpyDeviceToHost));

    // Validar los resultados (1.0 + 2.5 = 3.5)
    bool valid = true;
    for (int i = 0; i < N; i++) {
        if (fabs(h_C[i] - 3.5f) > 1e-5) {
            valid = false;
            std::cerr << "Error en el índice " << i << ": " << h_C[i] << " != 3.5" << std::endl;
            break;
        }
    }

    if (valid) {
        std::cout << "Todos los resultados son correctos." << std::endl;
        // ejemplo de impresión de los primeros 10 resultados
        std::cout << "Primeros 10 resultados:" << std::endl;
        for (int i = 0; i < 10; i++) {
            std::cout << "C[" << i << "] = " << h_C[i]
                        << " (A[" << i << "] = " << h_A[i]
                        << " + B[" << i << "] = " << h_B[i] << ")" << std::endl;
        }

    }

    // 7. Liberar la memoria del Device
    CUDA_CHECK(cudaFree(d_A));
    CUDA_CHECK(cudaFree(d_B));
    CUDA_CHECK(cudaFree(d_C));

    return 0;

}