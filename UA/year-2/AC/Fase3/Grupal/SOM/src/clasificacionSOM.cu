/*----------------------------------------------------------------------------*/
/*  FICHERO:       clasificacionSOM.cu									        */
/*  AUTOR:         Jorge Azorin								       			    */
/*													                            */
/*  RESUMEN												                        */
/*  ~~~~~~~												                        */
/* Ejercicio grupal para la clasificación de patrones de entrada basada         */
/* en SOM utilizando GPUs                                                       */
/*----------------------------------------------------------------------------*/

// includes, system
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>


// includes, project
#include <cuda.h>
#include <cuda_runtime.h>
#include <device_launch_parameters.h>
#include "clasificacionSOM.h"

// --- ADAPTACIÓN MULTIPLATAFORMA ---
#if defined(_WIN32) || defined(_WIN64)
    #include <Windows.h>
    typedef LARGE_INTEGER timeStamp;
#else
    #include <sys/time.h>
    #include <stddef.h>
#endif
// ----------------------------------



#define ERROR_CHECK { cudaError_t err; if ((err = cudaGetLastError()) != cudaSuccess) { printf("CUDA error: %s, line %d\n", cudaGetErrorString(err), __LINE__);}}

double getTime();

/*----------------------------------------------------------------------------*/
/* Helpers M3: indices row-major y distancia euclidea sobre arrays planos     */
/*----------------------------------------------------------------------------*/
__host__ __device__ inline int IndiceNeuronaRowMajor(int y, int x, int ancho)
{
	// Convenio comun del equipo: neurona (y,x) -> y*ancho + x
	return y * ancho + x;
}

__host__ __device__ inline int IndicePesoRowMajor(int indiceNeurona, int dimension, int componente)
{
	// Layout lineal de pesos: [neurona0(dim), neurona1(dim), ...]
	return indiceNeurona * dimension + componente;
}

__device__ float DistanciaEuclideaNeurona(const float* pesosSOM, const float* patron, int indiceNeurona, int dimension)
{
	float acumulado = 0.0f;
	for (int d = 0; d < dimension; ++d)
	{
		const float diferencia = pesosSOM[IndicePesoRowMajor(indiceNeurona, dimension, d)] - patron[d];
		acumulado += diferencia * diferencia;
	}
	return sqrtf(acumulado);
}

/*----------------------------------------------------------------------------*/
/* Kernel M3 (Ximo): distancia(neurona) + distancia(vecindario cruz valido)   */
/* Se encarga de calcular las distancias de cada neurona al patron de entrada */
/*----------------------------------------------------------------------------*/
__global__ void KernelDistanciasVecindario(
	const float* pesosSOM,
	const float* patron,
	int alto,
	int ancho,
	int dimension,
	float* distancias)
{
	// El patron se comparte a nivel bloque para evitar lecturas repetidas a global.
	extern __shared__ float sPatron[];

	const int hiloLineal = threadIdx.y * blockDim.x + threadIdx.x;
	const int hilosBloque = blockDim.x * blockDim.y;

	// Carga cooperativa del patron: cada hilo copia componentes separadas por stride.
	for (int d = hiloLineal; d < dimension; d += hilosBloque)
	{
		sPatron[d] = patron[d];
	}
	__syncthreads();

	const int x = blockIdx.x * blockDim.x + threadIdx.x;
	const int y = blockIdx.y * blockDim.y + threadIdx.y;

	if (x >= ancho || y >= alto) return;

	const int indiceNeurona = IndiceNeuronaRowMajor(y, x, ancho);
	float sumaDistancias = DistanciaEuclideaNeurona(pesosSOM, sPatron, indiceNeurona, dimension);

	// Vecindario cruz acordado: arriba, abajo, izquierda, derecha si esta en rango.
	if (y > 0)
	{
		sumaDistancias += DistanciaEuclideaNeurona(pesosSOM, sPatron, IndiceNeuronaRowMajor(y - 1, x, ancho), dimension);
	}
	if (y < alto - 1)
	{
		sumaDistancias += DistanciaEuclideaNeurona(pesosSOM, sPatron, IndiceNeuronaRowMajor(y + 1, x, ancho), dimension);
	}
	if (x > 0)
	{
		sumaDistancias += DistanciaEuclideaNeurona(pesosSOM, sPatron, IndiceNeuronaRowMajor(y, x - 1, ancho), dimension);
	}
	if (x < ancho - 1)
	{
		sumaDistancias += DistanciaEuclideaNeurona(pesosSOM, sPatron, IndiceNeuronaRowMajor(y, x + 1, ancho), dimension);
	}

	// Contrato de salida M3: para el patron actual, una puntuacion por neurona.
	distancias[indiceNeurona] = sumaDistancias;
}

static void CopiarSOMLineal(float* pesosLineales)
{
	// Convierte TSOM (doble puntero) a array row-major plano para GPU.
	for (int y = 0; y < SOM.Alto; ++y)
	{
		for (int x = 0; x < SOM.Ancho; ++x)
		{
			const int indiceNeurona = IndiceNeuronaRowMajor(y, x, SOM.Ancho);
			for (int d = 0; d < SOM.Dimension; ++d)
			{
				pesosLineales[IndicePesoRowMajor(indiceNeurona, SOM.Dimension, d)] = SOM.Neurona[y][x].pesos[d];
			}
		}
	}
}

static void CopiarPatronLineal(int np, float* patronLineal)
{
	for (int d = 0; d < Patrones.Dimension; ++d)
	{
		patronLineal[d] = Patrones.Pesos[np][d];
	}
}

static void CopiarEtiquetasLineal(int* labelsLineales)
{
	for (int y = 0; y < SOM.Alto; ++y)
		for (int x = 0; x < SOM.Ancho; ++x)
			labelsLineales[IndiceNeuronaRowMajor(y, x, SOM.Ancho)] = SOM.Neurona[y][x].label;
}

/*--------------------------------------------------------------------------------*/
/*    Kernel M4 (Julián): Reducción Naive (Búsqueda secuencial en 1 hilo)         */
/* Se encarga de encontrar el índice de la neurona ganadora (con menor distancia) */
/*--------------------------------------------------------------------------------*/
__global__ void KernelReduccion(const float* distancias, int totalNeuronas, int* indiceGanador)
{
	// Forzamos a que solo el primer hilo de la GPU haga el trabajo
	if (threadIdx.x == 0 && blockIdx.x == 0)
	{
		float min_dist = distancias[0];
		int min_idx = 0;

		for (int i = 1; i < totalNeuronas; ++i)
		{
			if (distancias[i] < min_dist)
			{
				min_dist = distancias[i];
				min_idx = i;
			}
		}
		// Devolvemos la posición (índice) donde estaba la distancia más corta
		*indiceGanador = min_idx;
	}
}

/*----------------------------------------------------------------------------*/
/*  FUNCION A PARALELIZAR  (versión secuencial-CPU)  				          */
/*	Implementa la clasificación basada en SOM de un conjunto de patrones      */
/*  de entrada definidos en un fichero                                        */
/*							!!!!! NO TOCAR !!!!!						      */
/*----------------------------------------------------------------------------*/
int ClasificacionSOMCPU()
{
	float distancia;
	float distanciaMenor=MAXDIST;

	for (int np=0;np<Patrones.Cantidad;np++) // Recorrido de todos los patrones
	{
		distanciaMenor=MAXDIST;
		for (int y = 0; y<SOM.Alto; y++)			// Recorrido de todas las neuronas
		{
			for (int x = 0; x<SOM.Ancho; x++)
			{
				distancia=CalculaDistancia(y,x,np);     // CalculaDistancia entre neurona (y,x) y patrón np
				for (int vy=-1;vy<2;vy++)               // Calculo en la vecindad
					for (int vx=-1;vx<2;vx++)
						if ((vx == 0) ^ (vy == 0))         // No comprobar con la misma neurona, se usa XOR en vez de AND para evirtar sumar solo las diagonales 
						   distancia+=CalculaDistancia(y+vy,x+vx,np);
				if (distancia < distanciaMenor)
				{
					distanciaMenor=distancia;  // Neurona con menor distancia
					EtiquetaCPU[np]=SOM.Neurona[y][x].label;
				}

			}
		}

	}
	return OKCLAS;									// Simulación CORRECTA
}

// ---------------------------------------------------------------
// ---------------------------------------------------------------
// FUNCION A IMPLEMENTAR POR EL GRUPO (paralelización de ClasificacionSOMCPU)
// ---------------------------------------------------------------
// ---------------------------------------------------------------

int ClasificacionSOMGPU()
{
	int estado_final = OKCLAS; // Control de flujo sin goto

	// 1. Declaración de punteros
	float* hPesosLineales = NULL;
	float* hPatronLineal = NULL;
	int* hLabelsLineales = NULL; // NUEVO M4: Array para etiquetas

	float* dPesosLineales = NULL;
	float* dPatronLineal = NULL;
	float* dDistancias = NULL;
	int* dIndiceGanador = NULL;  // NUEVO M4: Variable para que tu kernel devuelva el ganador

	fprintf(stderr, "Iniciando ClasificacionSOMGPU...\n");
	if (Patrones.Dimension != SOM.Dimension) return ERRORCLASS;

	fprintf(stderr, "SOM: %dx%d, Dim: %d | Patrones: %d, Dim: %d\n", SOM.Alto, SOM.Ancho, SOM.Dimension, Patrones.Cantidad, Patrones.Dimension);
	const int totalNeuronas = SOM.Alto * SOM.Ancho;
	const size_t bytesPesos = (size_t)totalNeuronas * (size_t)SOM.Dimension * sizeof(float);
	const size_t bytesPatron = (size_t)SOM.Dimension * sizeof(float);
	const size_t bytesDistancias = (size_t)totalNeuronas * sizeof(float);
	const size_t bytesLabels = (size_t)totalNeuronas * sizeof(int);

	// 2. Reserva en Host
	hPesosLineales = (float*)malloc(bytesPesos);
	hPatronLineal = (float*)malloc(bytesPatron);
	hLabelsLineales = (int*)malloc(bytesLabels);

	if (hPesosLineales == NULL || hPatronLineal == NULL || hLabelsLineales == NULL)
		estado_final = ERRORCLASS;

	if (estado_final == OKCLAS)
	{
		// Copiamos datos al formato lineal
		CopiarSOMLineal(hPesosLineales);
		CopiarEtiquetasLineal(hLabelsLineales); // NUEVO M4

		// 3. Reserva en Device (GPU)
		if (cudaMalloc((void**)&dPesosLineales, bytesPesos) != cudaSuccess) estado_final = ERRORCLASS;
		if (estado_final == OKCLAS && cudaMalloc((void**)&dPatronLineal, bytesPatron) != cudaSuccess) estado_final = ERRORCLASS;
		if (estado_final == OKCLAS && cudaMalloc((void**)&dDistancias, bytesDistancias) != cudaSuccess) estado_final = ERRORCLASS;
		if (estado_final == OKCLAS && cudaMalloc((void**)&dIndiceGanador, sizeof(int)) != cudaSuccess) estado_final = ERRORCLASS; // NUEVO M4
	}

	if (estado_final == OKCLAS)
	{
		// 4. Copia inicial Host -> Device
		if (cudaMemcpy(dPesosLineales, hPesosLineales, bytesPesos, cudaMemcpyHostToDevice) != cudaSuccess) estado_final = ERRORCLASS;

		const dim3 blockDimM3(16, 16);
		const dim3 gridDimM3((SOM.Ancho + blockDimM3.x - 1) / blockDimM3.x, (SOM.Alto + blockDimM3.y - 1) / blockDimM3.y);

		for (int np = 0; np < Patrones.Cantidad && estado_final == OKCLAS; ++np)
		{
			CopiarPatronLineal(np, hPatronLineal);
			if (cudaMemcpy(dPatronLineal, hPatronLineal, bytesPatron, cudaMemcpyHostToDevice) != cudaSuccess) {
				estado_final = ERRORCLASS; break;
			}

			// --- FASE M3: Kernel de Distancias (Ximo) ---
			KernelDistanciasVecindario<<<gridDimM3, blockDimM3, bytesPatron >>>(
				dPesosLineales, dPatronLineal, SOM.Alto, SOM.Ancho, SOM.Dimension, dDistancias);

			if (cudaGetLastError() != cudaSuccess) { estado_final = ERRORCLASS; break; }

			// --- FASE M4: Tu Kernel Naive de Reducción ---
			// Solo 1 bloque y 1 hilo
			KernelReduccion<<<1, 1 >>>(dDistancias, totalNeuronas, dIndiceGanador);

			if (cudaGetLastError() != cudaSuccess) { estado_final = ERRORCLASS; break; }

			// --- RESULTADO: Extraer el ganador ---
			int hIndiceGanador = -1;
			if (cudaMemcpy(&hIndiceGanador, dIndiceGanador, sizeof(int), cudaMemcpyDeviceToHost) != cudaSuccess) {
				estado_final = ERRORCLASS; break;
			}

			// Buscamos la etiqueta real usando el índice que encontró tu kernel
			EtiquetaGPU[np] = hLabelsLineales[hIndiceGanador];
		}
	}

	// 5. Limpieza de memoria centralizada (se ejecuta siempre al final)
	if (dDistancias != NULL) cudaFree(dDistancias);
	if (dPatronLineal != NULL) cudaFree(dPatronLineal);
	if (dPesosLineales != NULL) cudaFree(dPesosLineales);
	if (dIndiceGanador != NULL) cudaFree(dIndiceGanador);

	if (hPatronLineal != NULL) free(hPatronLineal);
	if (hPesosLineales != NULL) free(hPesosLineales);
	if (hLabelsLineales != NULL) free(hLabelsLineales);

	return estado_final;
}
 // ---------------------------------------------------------------
 // ---------------------------------------------------------------
 // ---------------------------------------------------------------
 // ---------------------------------------------------------------
 // ---------------------------------------------------------------

 // Declaraciones adelantadas de funciones
 int LeerSOM(const char *fichero);
 int LeerPatrones(const char *fichero);
 int EscribirSOM(int ancho, int alto,int dimension, const char *fichero);
 int EscribirPatrones(int cantidad,int dimension, const char *fichero);



////////////////////////////////////////////////////////////////////////////////
//PROGRAMA PRINCIPAL
/*							!!!!! NO TOCAR !!!!!						      */
////////////////////////////////////////////////////////////////////////////////
void
runTest(int argc, char** argv)
{

  	double gpu_start_time, gpu_end_time;
	double cpu_start_time, cpu_end_time;

	//EscribirSOM(32,32,64,"peq.som");
	//EscribirPatrones(1024,64,"peq.pat");
	/* Numero de argumentos */
	if (argc != 3)
	{
		fprintf(stderr, "Numero de parametros incorecto\n");
		fprintf(stderr, "Uso: %s superficie pasossim\n", argv[0]);
		return;
	}

	/* Apertura de Fichero */
	printf("Clasificacion basada en SOM...\n");
	/* Mapa SOM */
	if (LeerSOM((char *)argv[1]) == ERRORCLASS)
	{
		fprintf(stderr, "Lectura de SOM incorrecta\n");
		return;
	}
	/* Patrones */
	if (LeerPatrones((char *)argv[2]) == ERRORCLASS)
	{
		fprintf(stderr, "Lectura de patrones incorrecta\n");
		return;
	}
	
	// Creación etiquetas resultados para versiones CPU y GPU

	EtiquetaCPU = (int*)malloc(Patrones.Cantidad*sizeof(int));
	EtiquetaGPU = (int*)malloc(Patrones.Cantidad*sizeof(int));
	
	/* Algoritmo a paralelizar */
	cpu_start_time = getTime();
	if (ClasificacionSOMCPU() == ERRORCLASS)
	{
		fprintf(stderr, "Clasificacion CPU incorrecta\n");
		BorrarMapa();
		if (EtiquetaCPU != NULL) free(EtiquetaCPU);
		if (EtiquetaGPU != NULL) free(EtiquetaGPU);
		exit(1);
	}
	cpu_end_time = getTime();
	cudaSetDevice(0);
	/* Algoritmo a implementar */
	gpu_start_time = getTime();
	if (ClasificacionSOMGPU() == ERRORCLASS)
	{
		fprintf(stderr, "Clasificacion GPU incorrecta\n");
		BorrarMapa();
		if (EtiquetaCPU != NULL) free(EtiquetaCPU);
		if (EtiquetaGPU != NULL) free(EtiquetaGPU);
		return;
	}
	cudaDeviceSynchronize();
	gpu_end_time = getTime();
	// Comparación de corrección
	int comprobar = OKCLAS;
	for (int i = 0; i<Patrones.Cantidad; i++)
	{
		if ((EtiquetaCPU[i] != EtiquetaGPU[i]))
		{
			comprobar = ERRORCLASS;
			fprintf(stderr, "Fallo en la clasificacion del patron %d, valor correcto %d\n", i, EtiquetaCPU[i]);
		}
	}
	// Impresion de resultados
	if (comprobar == OKCLAS)
	{
		printf("Clasificacion correcta!\n");

	}
	// Impresión de resultados
	printf("Tiempo ejecucion GPU : %fs\n", \
		gpu_end_time - gpu_start_time);
	printf("Tiempo de ejecucion en la CPU : %fs\n", \
		cpu_end_time - cpu_start_time);
	printf("Se ha conseguido un factor de aceleracion %fx utilizando CUDA\n", (cpu_end_time - cpu_start_time) / (gpu_end_time - gpu_start_time));
	// Limpieza de Neuronas
	BorrarMapa();
	BorrarPatrones();
	if (EtiquetaCPU != NULL) free(EtiquetaCPU);
	if (EtiquetaGPU != NULL) free(EtiquetaGPU);
	return;
}

int
main(int argc, char** argv)
{
	runTest(argc, argv);
	getchar();
}

/* Funciones auxiliares */
double getTime()
{
#if defined(_WIN32) || defined(_WIN64)
    // Implementación original para Windows
    timeStamp start;
    timeStamp dwFreq;
    QueryPerformanceFrequency(&dwFreq);
    QueryPerformanceCounter(&start);
    return double(start.QuadPart) / double(dwFreq.QuadPart);
#else
    // Implementación para sistemas POSIX (Linux/macOS)
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (double)tv.tv_sec + (double)tv.tv_usec / 1000000.0;
#endif
}



/*----------------------------------------------------------------------------*/
/*	Función:  LeerSOM(char *fichero)						             	  */
/*													                          */
/*	          Lee la estructura del SOM con formato .SOM   					  */
/*							!!!!! NO TOCAR !!!!!						      */
/*----------------------------------------------------------------------------*/
int LeerSOM(const char *fichero)
{
	int i, j, ndim, count;		/* Variables de bucle */
	int alto,ancho;		/* Variables de tamaño del mapa */
	FILE *fpin; 			/* Fichero */
	int nx,ny,lx,ly,label,dimension;
	float pesos;

	/* Apertura de Fichero */
	if ((fpin = fopen(fichero, "r")) == NULL) return ERRORCLASS;
	/* Lectura de cabecera */
	if (fscanf(fpin, "Alto: %d\n", &alto)<0) return ERRORCLASS;
	if (fscanf(fpin, "Ancho: %d\n", &ancho)<0) return ERRORCLASS;
	if (fscanf(fpin, "Dimension: %d\n", &dimension)<0) return ERRORCLASS;
	if (feof(fpin)) return ERRORCLASS;

	if (CrearMapa(alto, ancho, dimension) == ERRORCLASS) return ERRORCLASS;
	/* Lectura del SOM */
	count = 0;
	for (i = 0; i<ancho; i++)
	{
		for (j = 0; j<alto; j++)
		{
			if (!feof(fpin))
			{
				fscanf(fpin, "N%d,%d:", &nx, &ny);
				for (ndim = 0; ndim<dimension-1;ndim++)
				{
			        fscanf(fpin, " %f", &pesos);
				    SOM.Neurona[j][i].pesos[ndim] = pesos;
				}
				fscanf(fpin, " %f\n", &pesos);
				SOM.Neurona[j][i].pesos[ndim] = pesos;
				
				fscanf(fpin, "L%d,%d: %d\n", &lx, &ly, &label);
				SOM.Neurona[j][i].label=label;
			
				count++;
			}
			else break;
		}
	}
	fclose(fpin);
	if (count != ancho*alto) return ERRORCLASS;
	return OKCLAS;
}

/*----------------------------------------------------------------------------*/
/*	Función:  LeerPatrones(char *fichero)						              */
/*													                          */
/*	          Lee los patrones de un fichero de entrada .pat   			      */
/*							!!!!! NO TOCAR !!!!!						      */
/*----------------------------------------------------------------------------*/
int LeerPatrones(const char *fichero)
{
	int i, ndim, count;		/* Variables de bucle */
	int cantidad,dimension;		/* Variables de tamaño de los patrones */
	FILE *fpin; 			/* Fichero */

	int np;
	float pesos;

	/* Apertura de Fichero */
	if ((fpin = fopen(fichero, "r")) == NULL) return ERRORCLASS;
	/* Lectura de cabecera */
	if (fscanf(fpin, "Numero: %d\n", &cantidad)<0) return ERRORCLASS;
	if (fscanf(fpin, "Dimension: %d\n", &dimension)<0) return ERRORCLASS;
	if (feof(fpin)) return ERRORCLASS;
	
	if (CrearPatrones(cantidad, dimension) == ERRORCLASS) return ERRORCLASS;
	/* Lectura de patrones */
	count = 0;
	for (i = 0; i<cantidad; i++)
	{
	  	if (!feof(fpin))
		{
			fscanf(fpin, "P%d:", &np);
				for (ndim = 0; ndim<dimension-1;ndim++)
				{
			        fscanf(fpin, " %f", &pesos);
					Patrones.Pesos[i][ndim] = pesos;
				}
				fscanf(fpin, " %f\n", &pesos);
				Patrones.Pesos[i][ndim] = pesos;
				
					
				count++;
			}
			else break;
	}
	
	fclose(fpin);
	if (count != cantidad) return ERRORCLASS;
	return OKCLAS;
}

/*----------------------------------------------------------------------------*/
/*	Función:  EscribirSOM(char *fichero)						              */
/*													                          */
/*	          Escribe la estructura del SOM en un fichero de salida .SOM   	  */
/*							!!!!! NO TOCAR !!!!!						      */
/*----------------------------------------------------------------------------*/
int EscribirSOM(int alto, int ancho, int dimension,const char *fichero)
{
	int i, j, ndim, count;		/* Variables de bucle */
	
	FILE *fpin; 			/* Fichero */
	int label;
	float pesos;

	/* Apertura de Fichero */
	if ((fpin = fopen(fichero, "w")) == NULL) return ERRORCLASS;
	/* Lectura de cabecera */
	if (fprintf(fpin, "Alto: %d\n", alto)<0) return ERRORCLASS;
	if (fprintf(fpin, "Ancho: %d\n", ancho)<0) return ERRORCLASS;
	if (fprintf(fpin, "Dimension: %d\n", dimension)<0) return ERRORCLASS;
	if (feof(fpin)) return ERRORCLASS;

	count = 0;
	for (i = 0; i<alto; i++)
	{
		for (j = 0; j<ancho; j++)
		{
			if (!feof(fpin))
			{
				fprintf(fpin, "N%d,%d:", i+1, j+1);
				for (ndim = 0; ndim<dimension-1;ndim++)
				{
					pesos=(rand()%1000)/10.0;
			        fprintf(fpin, " %lf", pesos);
				}				
				pesos=(rand()%1000)/10;
				fprintf(fpin, " %lf\n", &pesos);
				
				label=(rand()%5);
				fprintf(fpin, "L%d,%d: %d\n", i+1, j+1, label);
				
				count++;
			}
			else break;
		}
	}
	fclose(fpin);
	if (count != ancho*alto) return ERRORCLASS;
	return OKCLAS;
}


/*----------------------------------------------------------------------------*/
/*	Función:  EscribirPatrones(char *fichero)						          */
/*													                          */
/*	          Escribe los patrones en un fichero de salida .pat   	          */
/*							!!!!! NO TOCAR !!!!!						      */
/*----------------------------------------------------------------------------*/
int EscribirPatrones(int cantidad, int dimension,const char *fichero)
{
	int i, ndim, count;		/* Variables de bucle */
	
	FILE *fpin; 			/* Fichero */

	int np;
	float pesos;

	/* Apertura de Fichero */
	if ((fpin = fopen(fichero, "w")) == NULL) return ERRORCLASS;
	/* Lectura de cabecera */
	if (fprintf(fpin, "Numero: %d\n", cantidad)<0) return ERRORCLASS;
	if (fprintf(fpin, "Dimension: %d\n", dimension)<0) return ERRORCLASS;
	if (feof(fpin)) return ERRORCLASS;
	
	/* Lectura de patrones */
	count = 0;
	for (i = 0; i<cantidad; i++)
	{
	  	if (!feof(fpin))
		{
			fprintf(fpin, "P%d:", i+1);
				for (ndim = 0; ndim<dimension-1;ndim++)
				{
					pesos=(rand()%1000)/10.0;
			        fprintf(fpin, " %lf", pesos);
				}
				pesos=(rand()%1000)/10;
				fprintf(fpin, " %lf\n", pesos);
					
					
				count++;
			}
			else break;
	}
	
	fclose(fpin);
	if (count != cantidad) return ERRORCLASS;
	return OKCLAS;
}
