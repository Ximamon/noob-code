#include <iostream>
#include <vector>
#include <cmath>

__global__ void matrixMulNaive(float *A, float *B, float *C, int M, int K, int N) {
    // 1. Calcular fila y columna globales
    int col = blockIdx.x * blockDim.x + threadIdx.x;
    int row = blockIdx.y * blockDim.y + threadIdx.y;

    // 2. Control de limites (Boundry Check)
    if (row < M && col < N) {
        float sum = 0.0f;

        // 3. Producto punto: Iterar sobre la dimension compartida K
        for (int i = 0; i < K; ++i) {
            // Leer A[row][i] y B[i][col] usando la forma de aplanamiento
            float a_val = A[row * K + i];
            float b_val = B[i * N + col];
            sum += a_val * b_val;
        }

        // 4. Escribir el resultado en C[row][col]
        C[row * N + col] = sum;
    }
}

int main() {
    // Dimensiones de matrices (1024x1024)
    int M = 64;
    int K = 64;
    int N = 64;

    size_t sizeA = M * K * sizeof(float);
    size_t sizeB = K * N * sizeof(float);
    size_t sizeC = M * N * sizeof(float);

    // Asignar memoria en el Host
    std::vector<float> h_A(M * K, 1.0f); // Matriz A llena de 1.0
    std::vector<float> h_B(K * N, 2.0f); // Matriz B llena de 2.0
    std::vector<float> h_C(M * N, 0.0f); // Matriz C para el resultado

    // Punteros para el device
    float *d_A, *d_B, *d_C;

    // Reservar memoria en VRAM
    cudaMalloc((void**)&d_A, sizeA);
    cudaMalloc((void**)&d_B, sizeB);
    cudaMalloc((void**)&d_C, sizeC);

    // Transferir datos de Host a Device
    cudaMemcpy(d_A, h_A.data(), sizeA, cudaMemcpyHostToDevice);
    cudaMemcpy(d_B, h_B.data(), sizeB, cudaMemcpyHostToDevice);


    // Configurar la topologia de ejecuccion (Grid y Bloques 2D)
    dim3 threadsPerBlock(16, 16);
    dim3 blocksPerGrid(
        (N + threadsPerBlock.x - 1) / threadsPerBlock.x,
        (N + threadsPerBlock.y - 1) / threadsPerBlock.y
    );

    // Configurar Eventos de CUDA para medir el rendimiento
    cudaEvent_t start, stop;
    cudaEventCreate(&start);
    cudaEventCreate(&stop);

    // Iniciar temporizador
    cudaEventRecord(start);

    // Lanzar el kernel
    matrixMulNaive<<<blocksPerGrid, threadsPerBlock>>>(d_A, d_B, d_C, M, K, N);

    // Detener temporizador
    cudaEventRecord(stop);
    cudaEventSynchronize(stop);

    float ms = 0.0f;
    cudaEventElapsedTime(&ms, start, stop);

    // Transferir el resultado de vuelta al Host
    cudaMemcpy(h_C.data(), d_C, sizeC, cudaMemcpyDeviceToHost);

    // Verificación rápida del resultado (1.0 * 2.0 * 1024 = 2048.0)
    bool isCorrect = true;
    for (int i = 0; i < M * N; ++i) {
        if (std::abs(h_C[i] - (2.0f * K)) > 1e-5) {
            isCorrect = false;
            break;
        }
    }

    std::cout << "Validación: " << (isCorrect ? "ÉXITO" : "ERROR") << std::endl;
    std::cout << "Tiempo de ejecución del Kernel: " << ms << " ms" << std::endl;

    // Liberar memoria
    cudaFree(d_A);
    cudaFree(d_B);
    cudaFree(d_C);
    cudaEventDestroy(start);
    cudaEventDestroy(stop);

    return 0;
    
}