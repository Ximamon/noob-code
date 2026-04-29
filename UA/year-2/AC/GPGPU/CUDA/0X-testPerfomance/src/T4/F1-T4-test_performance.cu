/* =========================================================================================
 * REGISTRO DE RENDIMIENTO (Tiempos en milisegundos)
 * Hardware: Google Colab T4 | Tamaño del Grid/Bloque: 256 hilos por bloque, bloques calculados para cubrir N elementos
 * Descripción: Kernel que va aplicando cada concepto aprendido en los capitulos del libro Programming Massively Parallel Processors: A Hands-on Approach 4ed
 * Nota: Para ejecuciones en GPGPU-Sim, se transforma el IPC Global a IPC por SM que es lo que da NCU
 *       con la formula IPC_SM = (IPC_Global / 32) / 80 (32 Warps por SM y 80 SMs en V100)
 * =========================================================================================
 * Versión  |    T1     |     T2    |     T3    |     T4    |     T5    |   T. Min  |   Ciclos    |  IPC   | Descripción
 * ---------|-----------|-----------|-----------|-----------|-----------|-----------|-------------|--------|---------------------------------
 * Fase 1   | 24.527 ms | 24.497 ms | 24.503 ms | 24.528 ms | 24.577 ms | 24.497 ms |  623681610  | 2.0713 | Implementación base (Naive)
 * Fase 2   | 26.220 ms | 26.208 ms | 26.229 ms | 26.173 ms | 26.169 ms | 26.169 ms |  694286228  | 1.9815 | Version con mapeo 2D (Grid y Bloques en 2D)
 * Fase 3   | 20.429 ms | 20.371 ms | 20.355 ms | 20.362 ms | 20.368 ms | 20.355 ms |  564083103  | 1.5169 | Ordenamiento de datos para reducir divergencia (sort en el Host)
 * *Fase 3.1| 20.429 ms | 20.371 ms | 20.355 ms | 20.362 ms | 20.368 ms | 20.355 ms |  564083103  | 1.5169 | Optimización: Bloques de 32x16 hilos para aumentar ocupación (en lugar de 16x16)
 * ========================================================================================= */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// Kernel de la GPU (Cap 2: Mapeo 1D)
__global__ void filtro_condicional(float *in, float *out, int N) {
    // 1. Identidad del hilo (Mapeo 1D)
    int i = blockIdx.x * blockDim.x + threadIdx.x; // Formula universal para 1D

    // 2. Verificacion de limites
    if (i < N) {
        // 3. Lógica con divergencia intencional (Cap 4)
        if (in[i] > 0.5) 
            // Carda de trabajo PESADA
            out[i] = sinf(in[i]) * cosf(in[i]) + sqrtf(in[i]);
        else
            // Carga de trabajo LIGERA
            out[i] = in[i] * 2.0f;
    }
}


// Codigo del Host (CPU)
int main() {
    // Tamaño
    // N = 536870912 (512 Millones)      -> ~??s (Baseline para Colab T4, no se ejecuta en tiempo razonable en GPGPU-Sim)
    // N = 4194304 (4 Millones)          -> ~??s (baseline para GPUGPU-Sim, no se ejecuta en tiempo razonable en Colab)
    long N = 536870912;
    size_t size = N * sizeof(float);

    // Punteros para el Host(CPU) y el Device(GPU)
    float *h_in, *h_out;
    float *d_in, *d_out;

    // 1. Asignacion de memoria en el Host
    h_in = (float *)malloc(size);
    h_out = (float *)malloc(size);

    // Inicializar datos con valores aleatorios entre 0 y 1
    for (int i = 0; i < N; i++)
        h_in[i] = (float)rand() / (float)RAND_MAX;
        
    // 2. Asignacion de memoria en el Device
    cudaMalloc((void**)&d_in, size);
    cudaMalloc((void**)&d_out, size); 

    // 3. Transferir datos: Host -> Device
    cudaMemcpy(d_in, h_in, size, cudaMemcpyHostToDevice);

    // 4. Configurar la ejecucion (GRID y Bloques)
    int hilosPorBloque = 256;
    // Formula maestra para asegurar suficientes bloques: ceil(N / hilosPorBloque)
    int bloquesPorGrid = (N + hilosPorBloque - 1) / hilosPorBloque;

    // 5. Configurar los eventos de CUDA para medir el tiempo
    cudaEvent_t start, stop;
    cudaEventCreate(&start);
    cudaEventCreate(&stop);

    // =====================================================================
    // 6. LANZAR KERNEL Y MEDIR (MODIFICADO PARA PROFILING PROFESIONAL)
    // =====================================================================
    printf("Configuracion: %d bloques de %d hilos. N = %ld\n\n", bloquesPorGrid, hilosPorBloque, N);

    // --- FASE A: WARM-UP (Calentamiento) ---
    printf("Realizando 'Warm-up' de la GPU...\n");
    filtro_condicional<<<bloquesPorGrid, hilosPorBloque>>>(d_in, d_out, N);
    cudaDeviceSynchronize(); // Esperamos a que termine para limpiar el contexto
    
    // --- FASE B: BUCLE DE MEDICIÓN ---
    const int NUM_EJECUCIONES = 5;
    float tiempos[NUM_EJECUCIONES];
    float suma_tiempos = 0.0f;
    float tiempo_minimo = 999999.0f; // Inicializamos alto para buscar el mínimo

    printf("Iniciando %d ejecuciones cronometradas...\n", NUM_EJECUCIONES);
    
    for (int i = 0; i < NUM_EJECUCIONES; i++) {
        cudaEventRecord(start);
        
        filtro_condicional<<<bloquesPorGrid, hilosPorBloque>>>(d_in, d_out, N);
        
        cudaEventRecord(stop);
        cudaEventSynchronize(stop); // Fundamental sincronizar en cada iteración

        // Extraer tiempo de esta iteración
        float ms = 0;
        cudaEventElapsedTime(&ms, start, stop);
        
        tiempos[i] = ms;
        suma_tiempos += ms;
        if (ms < tiempo_minimo) {
            tiempo_minimo = ms;
        }
    }

    // --- FASE C: CÁLCULO DE ESTADÍSTICAS ---
    float media = suma_tiempos / NUM_EJECUCIONES;

    printf("\n¡Ejecucion completada!\n");
    printf("--------------------------------------------------\n");
    printf("T1: %.3f ms | T2: %.3f ms | T3: %.3f ms | T4: %.3f ms | T5: %.3f ms\n", 
           tiempos[0], tiempos[1], tiempos[2], tiempos[3], tiempos[4]);
    printf("--------------------------------------------------\n");
    printf("TIEMPO MEDIO  : %.3f ms\n", media);
    printf("TIEMPO MÍNIMO : %.3f ms\n", tiempo_minimo);
    printf("--------------------------------------------------\n");
    // =====================================================================

    // 7. Transferir resultados: Device -> Host
    cudaMemcpy(h_out, d_out, size, cudaMemcpyDeviceToHost);

    // 8. Limpiar memoria
    cudaFree(d_in);
    cudaFree(d_out);
    free(h_in);
    free(h_out);
    cudaEventDestroy(start);
    cudaEventDestroy(stop);

    return 0;
}