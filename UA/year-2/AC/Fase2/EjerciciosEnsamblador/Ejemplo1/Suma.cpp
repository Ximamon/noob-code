// Ensamblador en línea. Procesador: x86 (32 bits)
#include <iostream>

// Le decimos a C++: "Confía en mí, esta función existe en otro archivo y está compilada al estilo C"
extern "C" int sumar(int a, int b);

int main() {
    int numero1 = 15;
    int numero2 = 25;

    // Llamamos a la función escrita en ensamblador
    int resultado = sumar(numero1, numero2);

    std::cout << "La suma calculada en x86 es: " << resultado << std::endl;

    return 0;
}