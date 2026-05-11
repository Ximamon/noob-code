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
#include <Windows.h>



#define ERROR_CHECK { cudaError_t err; if ((err = cudaGetLastError()) != cudaSuccess) { printf("CUDA error: %s, line %d\n", cudaGetErrorString(err), __LINE__);}}

typedef LARGE_INTEGER timeStamp;
double getTime();

/*----------------------------------------------------------------------------*/
/* Helpers M3: layout lineal y calculo de distancia para replicar la CPU base */
/*----------------------------------------------------------------------------*/
__host__ __device__ inline int IndiceNeuronaRowMajor(int y, int x, int ancho)
{
	return y * ancho + x;
}

__host__ __device__ inline int IndicePesoRowMajor(int indiceNeurona, int dimension, int componente)
{
	return indiceNeurona * dimension + componente;
}

__device__ float DistanciaMediaAbsolutaNeurona(const float* pesosSOM, const float* patron, int indiceNeurona, int dimension)
{
	float distancia = 0.0f;
	for (int d = 0; d < dimension; ++d)
	{
		float diferencia = pesosSOM[IndicePesoRowMajor(indiceNeurona, dimension, d)] - patron[d];
		distancia += fabsf(diferencia);
	}
	return distancia / (float)dimension;
}

/*----------------------------------------------------------------------------*/
/* Kernel M3: calcula la distancia de cada neurona (propia + vecindad diagonal) */
/*----------------------------------------------------------------------------*/
__global__ void KernelDistanciasVecindarioCPUBase(
	const float* pesosSOM,
	const float* patron,
	int alto,
	int ancho,
	int dimension,
	float* distancias)
{
	extern __shared__ float sPatron[];

	const int hiloLineal = threadIdx.y * blockDim.x + threadIdx.x;
	const int hilosBloque = blockDim.x * blockDim.y;
	for (int d = hiloLineal; d < dimension; d += hilosBloque)
	{
		sPatron[d] = patron[d];
	}
	__syncthreads();

	const int x = blockIdx.x * blockDim.x + threadIdx.x;
	const int y = blockIdx.y * blockDim.y + threadIdx.y;

	if (x >= ancho || y >= alto) return;

	const int indiceNeurona = IndiceNeuronaRowMajor(y, x, ancho);
	float sumaDistancias = DistanciaMediaAbsolutaNeurona(pesosSOM, sPatron, indiceNeurona, dimension);

	/* Replica exacta del patron de vecindad de ClasificacionSOMCPU original:
	   se suman solo diagonales (vx != 0 && vy != 0). */
	if (y > 0 && x > 0)
	{
		sumaDistancias += DistanciaMediaAbsolutaNeurona(pesosSOM, sPatron, IndiceNeuronaRowMajor(y - 1, x - 1, ancho), dimension);
	}
	if (y > 0 && x < ancho - 1)
	{
		sumaDistancias += DistanciaMediaAbsolutaNeurona(pesosSOM, sPatron, IndiceNeuronaRowMajor(y - 1, x + 1, ancho), dimension);
	}
	if (y < alto - 1 && x > 0)
	{
		sumaDistancias += DistanciaMediaAbsolutaNeurona(pesosSOM, sPatron, IndiceNeuronaRowMajor(y + 1, x - 1, ancho), dimension);
	}
	if (y < alto - 1 && x < ancho - 1)
	{
		sumaDistancias += DistanciaMediaAbsolutaNeurona(pesosSOM, sPatron, IndiceNeuronaRowMajor(y + 1, x + 1, ancho), dimension);
	}

	distancias[indiceNeurona] = sumaDistancias;
}

static void CopiarSOMLineal(float* pesosLineales)
{
	for (int y = 0; y < SOM.Alto; ++y)
	{
		for (int x = 0; x < SOM.Ancho; ++x)
		{
			int indiceNeurona = IndiceNeuronaRowMajor(y, x, SOM.Ancho);
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

/*----------------------------------------------------------------------------*/
/*  FUNCION A PARALELIZAR  (versión secuencial-CPU)  				          */
/*	Implementa la clasificación basada en SOM de un conjunto de patrones      */
/*  de entrada definidos en un fichero                                         */
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
						if (vx != 0 && vy != 0)         // No comprobar con la misma neurona
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
	cudaError_t cudaStatus = cudaSuccess;
	float* hPesosLineales = NULL;
	float* hPatronLineal = NULL;
	float* hDistancias = NULL;
	float* dPesosLineales = NULL;
	float* dPatronLineal = NULL;
	float* dDistancias = NULL;

	if (Patrones.Dimension != SOM.Dimension) return ERRORCLASS;

	const int totalNeuronas = SOM.Alto * SOM.Ancho;
	const size_t bytesPesos = (size_t)totalNeuronas * (size_t)SOM.Dimension * sizeof(float);
	const size_t bytesPatron = (size_t)SOM.Dimension * sizeof(float);
	const size_t bytesDistancias = (size_t)totalNeuronas * sizeof(float);

	hPesosLineales = (float*)malloc(bytesPesos);
	hPatronLineal = (float*)malloc(bytesPatron);
	hDistancias = (float*)malloc(bytesDistancias);
	if (hPesosLineales == NULL || hPatronLineal == NULL || hDistancias == NULL) goto Error;

	CopiarSOMLineal(hPesosLineales);

	cudaStatus = cudaMalloc((void**)&dPesosLineales, bytesPesos);
	if (cudaStatus != cudaSuccess) goto Error;
	cudaStatus = cudaMalloc((void**)&dPatronLineal, bytesPatron);
	if (cudaStatus != cudaSuccess) goto Error;
	cudaStatus = cudaMalloc((void**)&dDistancias, bytesDistancias);
	if (cudaStatus != cudaSuccess) goto Error;

	cudaStatus = cudaMemcpy(dPesosLineales, hPesosLineales, bytesPesos, cudaMemcpyHostToDevice);
	if (cudaStatus != cudaSuccess) goto Error;

	const dim3 blockDim(16, 16);
	const dim3 gridDim(
		(SOM.Ancho + blockDim.x - 1) / blockDim.x,
		(SOM.Alto + blockDim.y - 1) / blockDim.y);

	for (int np = 0; np < Patrones.Cantidad; ++np)
	{
		float distanciaMenor = MAXDIST;
		int indiceGanador = 0;

		CopiarPatronLineal(np, hPatronLineal);
		cudaStatus = cudaMemcpy(dPatronLineal, hPatronLineal, bytesPatron, cudaMemcpyHostToDevice);
		if (cudaStatus != cudaSuccess) goto Error;

		KernelDistanciasVecindarioCPUBase<<<gridDim, blockDim, bytesPatron>>>(
			dPesosLineales,
			dPatronLineal,
			SOM.Alto,
			SOM.Ancho,
			SOM.Dimension,
			dDistancias);
		ERROR_CHECK;

		cudaStatus = cudaMemcpy(hDistancias, dDistancias, bytesDistancias, cudaMemcpyDeviceToHost);
		if (cudaStatus != cudaSuccess) goto Error;

		for (int idx = 0; idx < totalNeuronas; ++idx)
		{
			if (hDistancias[idx] < distanciaMenor)
			{
				distanciaMenor = hDistancias[idx];
				indiceGanador = idx;
			}
		}

		EtiquetaGPU[np] = SOM.Neurona[indiceGanador / SOM.Ancho][indiceGanador % SOM.Ancho].label;
	}

	if (dDistancias != NULL) cudaFree(dDistancias);
	if (dPatronLineal != NULL) cudaFree(dPatronLineal);
	if (dPesosLineales != NULL) cudaFree(dPesosLineales);
	if (hDistancias != NULL) free(hDistancias);
	if (hPatronLineal != NULL) free(hPatronLineal);
	if (hPesosLineales != NULL) free(hPesosLineales);
	return OKCLAS;

Error:
	if (dDistancias != NULL) cudaFree(dDistancias);
	if (dPatronLineal != NULL) cudaFree(dPatronLineal);
	if (dPesosLineales != NULL) cudaFree(dPesosLineales);
	if (hDistancias != NULL) free(hDistancias);
	if (hPatronLineal != NULL) free(hPatronLineal);
	if (hPesosLineales != NULL) free(hPesosLineales);
	return ERRORCLASS;
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
		fprintf(stderr, "Clasificación CPU incorrecta\n");
		BorrarMapa();
		if (EtiquetaCPU != NULL) free(EtiquetaCPU);
		if (EtiquetaGPU != NULL) free(EtiquetaCPU);
		exit(1);
	}
	cpu_end_time = getTime();
	cudaSetDevice(0);
	/* Algoritmo a implementar */
	gpu_start_time = getTime();
	if (ClasificacionSOMGPU() == ERRORCLASS)
	{
		fprintf(stderr, "Clasificación GPU incorrecta\n");
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
	printf("Tiempo ejecución GPU : %fs\n", \
		gpu_end_time - gpu_start_time);
	printf("Tiempo de ejecución en la CPU : %fs\n", \
		cpu_end_time - cpu_start_time);
	printf("Se ha conseguido un factor de aceleración %fx utilizando CUDA\n", (cpu_end_time - cpu_start_time) / (gpu_end_time - gpu_start_time));
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
	timeStamp start;
	timeStamp dwFreq;
	QueryPerformanceFrequency(&dwFreq);
	QueryPerformanceCounter(&start);
	return double(start.QuadPart) / double(dwFreq.QuadPart);
}



/*----------------------------------------------------------------------------*/
/*	Función:  LeerSOM(char *fichero)						              */
/*													                          */
/*	          Lee la estructura del SOM con formato .SOM   */
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
/*	          Lee los patrones de un fichero de entrada .pat   */
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
/*	Función:  LeerPatrones(char *fichero)						              */
/*													                          */
/*	          Lee los patrones de un fichero de entrada .pat   */
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
