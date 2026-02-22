// Julian Hinojosa Gil, 48795869N

/*
    ADA. 2025-26
    Practice 2: "Empirical analysis by means of program-steps account of two sorting algorithms: Middle-Quicksort and Heapsort."
*/
#include <unistd.h>
#include <iostream>
#include <iomanip>
#include <sys/utsname.h>
#include <cstdint> 
#include <fstream> // <- NUEVO: Para leer archivos
#include <string>  // <- NUEVO: Para manejar textos

#ifdef __APPLE__
    #include <sys/sysctl.h>
#elif defined(__linux__)
    #include <unistd.h>
    #include <sys/sysinfo.h>
#endif
#include <math.h>

using namespace std;

// Función para imprimir información del sistema (opcional, pero útil para contextualizar los resultados)
void printSystemInfo() {
    struct utsname u;
    if (uname(&u) == 0) {
        cout << "#System: " << u.sysname << " " << u.release << " (" << u.machine << ")" << endl;
    }

#ifdef __APPLE__
    // --- CÓDIGO EXCLUSIVO PARA MAC ---
    char cpu[256];
    size_t size = sizeof(cpu);
    if (sysctlbyname("machdep.cpu.brand_string", &cpu, &size, nullptr, 0) == 0) {
        cout << "#CPU: " << cpu << endl;
    }

    int physical = 0, logical = 0;
    size = sizeof(physical);
    if (sysctlbyname("hw.physicalcpu", &physical, &size, nullptr, 0) == 0) {
        cout << "#Physical cores: " << physical << endl;
    }

    size = sizeof(logical);
    if (sysctlbyname("hw.logicalcpu", &logical, &size, nullptr, 0) == 0) {
        cout << "#Logical cores: " << logical << endl;
    }

    uint64_t mem = 0;
    size = sizeof(mem);
    if (sysctlbyname("hw.memsize", &mem, &size, nullptr, 0) == 0) {
        cout << "#RAM (GiB): " << fixed << setprecision(2)
             << (double)mem / (1024.0 * 1024.0 * 1024.0) << endl;
    }

#elif defined(__linux__)
    // --- CÓDIGO EXCLUSIVO PARA LINUX (Ubuntu) ---
    
    // 1. Leer el modelo de CPU desde /proc/cpuinfo
    ifstream cpuinfo("/proc/cpuinfo");
    string line;
    if (cpuinfo.is_open()) {
        while (getline(cpuinfo, line)) {
            // Buscamos la línea que empieza por "model name"
            if (line.substr(0, 10) == "model name") {
                size_t pos = line.find(':');
                if (pos != string::npos) {
                    // Extraemos lo que hay después de los dos puntos (saltando el espacio)
                    cout << "#CPU: " << line.substr(pos + 2) << endl;
                    break; // Salimos del bucle al encontrar el primero
                }
            }
        }
        cpuinfo.close();
    }

    // 2. Núcleos lógicos
    long logical = sysconf(_SC_NPROCESSORS_ONLN);
    if (logical > 0) {
        cout << "#Logical cores: " << logical << endl;
    }

    // 3. Memoria RAM
    long paginas = sysconf(_SC_PHYS_PAGES);
    long tamano_pagina = sysconf(_SC_PAGE_SIZE);
    if (paginas > 0 && tamano_pagina > 0) {
        uint64_t mem = (uint64_t)paginas * (uint64_t)tamano_pagina;
        cout << "#RAM (GiB): " << fixed << setprecision(2)
             << (double)mem / (1024.0 * 1024.0 * 1024.0) << endl;
    }
#endif
}


//-----------Middle-Quicksort------------------------------------
// The algorithm selects the middle element of the array as the "pivot".
// In a process called "partitioning", it rearranges the elements so that
// all elements smaller than the pivot are placed to its left, and
// all elements greater than the pivot are placed to its right.
// The process is then repeated recursively on the two resulting
// subarrays (left and right of the pivot).
//--------------------------------------------------------------

void middle_QuickSort(int *v, long left, long right, size_t &pasos) {
    long i, j;
    int pivot;

    pasos++;
    if (left < right) {
        i = left; j = right;
        pivot = v[(i + j) / 2];
        pasos++;
        // pivot based partitioning:
        do {
            pasos++;
            while (v[i] < pivot) i++, pasos++;
            
            while (v[j] > pivot) j--, pasos++;
            
            if (i <= j) {
                swap(v[i], v[j]);
                i++; j--;
            }
            
        } while (i <= j);
        // Repeat for each non-empty subarray:
        if (left < j) middle_QuickSort(v, left, j, pasos);
        if (i < right) middle_QuickSort(v, i, right, pasos);
    }
}

//-------Heapsort---------------------------------------------
// The algorithm works by repeatedly selecting the largest remaining element
// and placing it at the end of the vector in its correct position.
//
// To efficiently select the largest element, it builds a max-heap.
//
// The sink procedure is used for heap construction (or reconstruction).
//--------------------------------------------------------------

void sink(int *v, size_t n, size_t i, size_t &pasos)
// Sink an element (indexed by i) in a tree to maintain the heap property.
// n is the size of the heap.
{
    size_t largest;
    size_t l, r; // indices of left and right childs

    do {
        pasos++;
        largest = i;  // Initialize largest as root
        l = 2 * i + 1;  // left = 2*i + 1
        r = 2 * i + 2;  // right = 2*i + 2

        // If the left child exists and is larger than the root
        if (l < n && v[l] > v[largest])
            largest = l;

        // If the right child exists and is larger than the largest so far
        if (r < n && v[r] > v[largest])
            largest = r;

        // If the largest is still the root, the process is done
        if (largest == i) break;

        // Otherwise, swap the new largest with the current node i and repeat the process with the children
        swap(v[i], v[largest]);
        i = largest;
    } while (true);
}

//--------Heapsort algorithm (ascending sorting)----------------
void heapSort(int *v, size_t n, size_t &pasos)
{
    // Build a max-heap with the input array ("heapify"):
    // Starting from the last non-leaf node (right to left), sink each element to construct the heap.
    for (size_t i = n / 2 - 1; true; i--) {
        sink(v, n, i, pasos);
        if (i == 0) break; // As size_t is an unsigned type
    }

    // At this point, we have a max-heap. Now, sort the array:
    // Repeatedly swap the root (largest element) with the last element and rebuild the heap.
    for (size_t i = n - 1; i > 0; i--) {
        // Move the root (largest element) to the end by swapping it with the last element.
        swap(v[0], v[i]);
        // Rebuild the heap by sinking the new root element.
        // Note that the heap size is reduced by one in each iteration (so the element moved to the end stays there)
        sink(v, i, 0, pasos);
        // The process ends when the heap has only one element, which is the smallest and remains at the beginning of the array.
    }
}

int main(void) {
    srand(0); // Semilla fija según la plantilla
    printSystemInfo(); // Imprimir información del sistema

	cout << "#QUICKSORT VERSUS HEAPSORT." << endl;
	cout << "#Average processing Msteps (millions of program steps)" << endl;
	cout << "#Number of samples (arrays of integer): 30" << endl;
	cout << "#                 RANDOM ARRAYS       SORTED ARRAYS     REVERSE SORTED ARRAYS" << endl;
	cout << "#    Size     QuickSort  HeapSort  QuickSort  HeapSort  QuickSort   HeapSort" << endl;
	cout << "============================================================================" << endl;

    // Bucle para los tamaños: 2^15 a 2^20
    for (int exp = 15; exp <= 20; exp++) {
        size_t n = pow(2, exp);
        
        // Vector para Quicksort
        int* v1 = new int[n];
        // Vector para Heapsort
        int* v2 = new int[n];

        

        size_t pasosQuick = 0;
        size_t pasosHeap = 0;

        // --- ANÁLISIS VECTOR ALEATORIO ---
        // 30 muestras para cada tamaño
        for (int i = 0; i < 30; i++) {
            for (size_t j = 0; j < n; j++) {
                v1[j] = rand(); 
                v2[j] = v1[j]; // Copiar el mismo vector para ambos algoritmos
            }
            
            size_t pasosQS = 0;
            middle_QuickSort(v1, 0, n - 1, pasosQS);
            size_t pasosHS = 0;
            heapSort(v2, n, pasosHS);
            
            pasosQuick += pasosQS;
            pasosHeap += pasosHS;
        }

        // Calcular media
        double mediaPasosQuickRandom = (double)pasosQuick / (30.0 * 1000000.0); // Media de pasos en millones
        double mediaPasosHeapRandom = (double)pasosHeap / (30.0 * 1000000.0); // Media de pasos en millones


        // --- ANÁLISIS ORDENADO (CRECIENTE) ---
        // Solo 1 muestra necesaria según enunciado y usando los mismos vectores que ya estan ordenados
        
        pasosQuick = 0;
        pasosHeap = 0;
        middle_QuickSort(v1, 0, n - 1, pasosQuick);
        heapSort(v2, n, pasosHeap);
        double pasosQuickOrdenado = (double)pasosQuick / 1000000.0;
        double pasosHeapOrdenado = (double)pasosHeap / 1000000.0;


        // --- ANÁLISIS INVERSO (DECRECIENTE) ---
        // Igual que el caso ordenado, solo 1 muestra necesaria y usando los mismos vectores que ya estan ordenados, pero ahora se crean en orden inverso
        // Crear vector inverso (n, n-1, ...)
        for (size_t k = 0; k < n; k++) {
            v1[k] = n - k; // Vector para Quicksort
            v2[k] = v1[k]; // Copiar el mismo vector para ambos algoritmos
        }
        
        pasosQuick = 0;
        pasosHeap = 0;
        middle_QuickSort(v1, 0, n - 1, pasosQuick);
        heapSort(v2, n, pasosHeap);
        double pasosQuickReverse = (double)pasosQuick / 1000000.0;
        double pasosHeapReverse = (double)pasosHeap / 1000000.0;

        // Liberamos memoria
        delete [] v1;
        delete [] v2;

        // Imprimir resultados
        cout.width(9);
        cout << n << flush;
        cout.width(11);
        cout << fixed << setprecision(4) << mediaPasosQuickRandom;
        cout.width(11);
        cout << fixed << setprecision(4) << mediaPasosHeapRandom;
        cout.width(11);
        cout << fixed << setprecision(4) << pasosQuickOrdenado;
        cout.width(11);
        cout << fixed << setprecision(4) << pasosHeapOrdenado;
        cout.width(11);
        cout << fixed << setprecision(4) << pasosQuickReverse;
        cout.width(11);
        cout << fixed << setprecision(4) << pasosHeapReverse;
        cout << endl;
    }

    return 0;
}
