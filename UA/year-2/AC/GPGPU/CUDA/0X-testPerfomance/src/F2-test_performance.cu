/* =========================================================================================
 * REGISTRO DE RENDIMIENTO (Tiempos en milisegundos)
 * Hardware: GPGPUSim SM7_QV100 | Tamaño del Grid/Bloque: 256 hilos por bloque, bloques calculados para cubrir N elementos
 * Descripción: Kernel que va aplicando cada concepto aprendido en los capitulos del libro Programming Massively Parallel Processors: A Hands-on Approach 4ed
 * Nota: Para ejecuciones en GPGPU-Sim, se transforma el IPC Global a IPC por SM que es lo que da NCU
 *       con la formula IPC_SM = (IPC_Global / 32) / 80 (32 Warps por SM y 80 SMs en V100)
 * =========================================================================================
 * Versión  |      T1       |       T2      |       T3      |       T4      |       T5      |     T. Min    | Ciclos Sim. |    IPC    | Descripción
 * ---------|---------------|---------------|---------------|---------------|---------------|---------------|-------------|-----------|---------------------------------
 * Fase 1   | 852000.000 ms | 846000.000 ms | 840000.000 ms | 834000.000 ms | 786000.000 ms | 786000.000 ms |    56736    |   1.3780  | Implementación base (Naive)
 * *Fase 2  | 868000.000 ms | 906000.000 ms | 868000.000 ms | 860000.000 ms | 861000.000 ms | 860000.000 ms |    61248    |   1.4905  | Version con mapeo 2D (Grid y Bloques en 2D)
 * Fase 3   | 854000.000 ms | 852000.000 ms | 783000.000 ms | 770000.000 ms | 783000.000 ms | 770000.000 ms |    54698    |   1.6690  | Ordenamiento de datos para reducir divergencia (sort en el Host)
 * Fase 3.1 | 867000.000 ms | 840000.000 ms | 764000.000 ms | 842000.000 ms | 871000.000 ms | 764000.000 ms |    58285    |   1.5662  | Optimización: Bloques de 32x16 hilos para aumentar ocupación (en lugar de 16x16)
 * ========================================================================================= */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// Kernel de la GPU (Cap 3: Mapeo 2D)
__global__ void filtro_condicional(float *in, float *out, int width, int height) {
    // 1. Identidad del hilo en File y Columna (Mapeo 2D)
    int col = blockIdx.x * blockDim.x + threadIdx.x;
    int row = blockIdx.y * blockDim.y + threadIdx.y;
    

    // 2. Verificacion de limites en 2D
    if (col < width && row < height) {
        // 3. Aplanamiento de 2D a 1D para acceder a memoria
        int i = row * width + col;

        // 4. Lógica con divergencia intencional (Cap 4)
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
    int width = 2048;
    int height = 2048;
    int N = width * height;
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

    // 4. Configurar la ejecucion en 2D (GRID y Bloques)
    // Bloques de 16x16 = 256 hilos por bloques
    dim3 hilosPorBloque(16, 16);

    // Cuadricula (Grid) en 2D
    dim3 bloquesPorGrid((width + hilosPorBloque.x - 1) / hilosPorBloque.x, (height + hilosPorBloque.y - 1) / hilosPorBloque.y);

    // 5. Configurar los eventos de CUDA para medir el tiempo
    cudaEvent_t start, stop;
    cudaEventCreate(&start);
    cudaEventCreate(&stop);

    // =====================================================================
    // 6. LANZAR KERNEL Y MEDIR (MODIFICADO PARA PROFILING PROFESIONAL)
    // =====================================================================
    printf("Configuracion: Grid: %d%d bloques. Bloque: %d%d hilos. N = %ld\n\n", bloquesPorGrid.x, bloquesPorGrid.y, hilosPorBloque.x, hilosPorBloque.y, N);

    // --- FASE A: WARM-UP (Calentamiento) ---
    printf("Realizando 'Warm-up' de la GPU...\n");
    filtro_condicional<<<bloquesPorGrid, hilosPorBloque>>>(d_in, d_out, width, height);
    cudaDeviceSynchronize(); // Esperamos a que termine para limpiar el contexto
    
    // --- FASE B: BUCLE DE MEDICIÓN ---
    const int NUM_EJECUCIONES = 5;
    float tiempos[NUM_EJECUCIONES];
    float suma_tiempos = 0.0f;
    float tiempo_minimo = 999999.0f; // Inicializamos alto para buscar el mínimo

    printf("Iniciando %d ejecuciones cronometradas...\n", NUM_EJECUCIONES);
    
    for (int i = 0; i < NUM_EJECUCIONES; i++) {
        cudaEventRecord(start);
        
        filtro_condicional<<<bloquesPorGrid, hilosPorBloque>>>(d_in, d_out, width, height);
        
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

    printf("\n¡Ejecucion 2D completada!\n");
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