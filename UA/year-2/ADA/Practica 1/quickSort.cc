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
    srand(0);
    
    // Cabecera de texto
    std::cout << "QuickSort CPU times in miliseconds: "
              << std::endl
              << "# Size \t CPU time (ms.)"
              << std::endl
              << "# ----------------------------"
              << std::endl;

    // Generamos varios vectores de tamaño en base a potencia de 2
    for (int exp = 15; exp <= 20; exp++) {
        size_t size = size_t(pow(2, exp));
        int *v = new int [size];
        if (!v){
            std::cerr << "Error, not enough memory!" << std::endl;
            exit (EXIT_FAILURE);  
        }
        std::cout << size << "\t\t" << std::flush;

        // Llenamos los vectores con valores aleatorios
        for (size_t j = 0; j < size; j++) 
            v[j] = rand();
        
        int middle = size / 2;
        //std::cout << middle << std::endl;
        
        auto start = clock();
        middle_QuickSort(v, (size_t)0, size-1);
        auto end = clock();


        std::cout << 1000.0 * (end-start) / CLOCKS_PER_SEC  << std::endl;

        for (size_t i = 1; i < size; i++)
            if (v[i] < v[i-1]){ 
                std::cerr << "Panic, array not sorted! " << i << std::endl; 
                exit(EXIT_FAILURE);            
            }

        delete[] v; 
    }

    
}