/*
ADA 2025-26
Práctica 1: "Empirical analysis of Quicksort average-time complexity."
*/

#include <unistd.h>
#include <iostream>
#include <math.h>

//--------------------------------------------------------------
// Middle QuickSort
//--------------------------------------------------------------
void middle_QuickSort(int * v, long left, long right){

    long i,j;
    int pivot,aux; 
    if (left<right)
    {
        i=left; j=right;
        pivot=v[(i+j)/2];
        do
        {
            while (v[i]<pivot) i++;
            while (v[j]>pivot) j--;
            if (i<=j)
            {
                aux=v[i]; v[i]=v[j]; v[j]=aux;
                i++; j--;
            }
       } while (i<=j);
       if (left<j)  middle_QuickSort(v,left,j);
       if (i<right) middle_QuickSort(v,i,right);
    }
}

int main(void) {
    int repeticiones = 30;
    srand(0);
    
    // Cabecera de texto
    std::cout << "QuickSort CPU times in miliseconds: "
              << std::endl
              << "# Size \t Average CPU time (ms.)"
              << std::endl
              << "# -------------------------------"
              << std::endl;


    
    // Bucle para diferentes tamaños de vector de 2^15 a 2^20
    for (int exp = 15; exp <= 20; exp++) {
        // Variable para acumular tiempos de las 30 repeticiones
        auto cumulative_time = 0.0;
        size_t size = size_t(pow(2, exp)); // Tamaño del vector actual
        std::cout << size << "\t\t" << std::flush; // Imprimimos el tamaño del vector

        // Generamos varios vectores de tamaño en base a potencia de 2
        for (int i = 0; i < repeticiones; i++) {
            // Reserva de memoria para el vector, si falla, salimos
            int *v = new int [size];
            if (!v){
                std::cerr << "Error, not enough memory!" << std::endl;
                exit (EXIT_FAILURE);  
            }
            
            // Llenamos los vectores con valores aleatorios
            for (size_t j = 0; j < size; j++) 
                v[j] = rand();
            
            // Medimos el tiempo de ordenación
            auto start = clock();
            middle_QuickSort(v, (size_t)0, size-1);
            auto end = clock();
            
            // Acumulamos el tiempo de cada repetición en milisegundos
            cumulative_time += 1000.0 * (end-start) / CLOCKS_PER_SEC;
            
            // Comprobamos que el vector está ordenado
            for (size_t i = 1; i < size; i++)
                if (v[i] < v[i-1]){ 
                    std::cerr << "Panic, array not sorted! " << i << std::endl; 
                    exit(EXIT_FAILURE);            
                }

            delete[] v; 
        }
        std::cout << cumulative_time / repeticiones << std::endl; // Imprimimos el tiempo medio
    }

    return 0;
}