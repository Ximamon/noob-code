// Julian Hinojosa Gil 48795869N

#include <iostream>
#include <iomanip>
#include <cmath>

unsigned long pasos2_1;
unsigned long pasos2_2;
unsigned long pasos2_3;

unsigned long pow2_1(unsigned n);
unsigned long pow2_2(unsigned n);
unsigned long pow2_3(unsigned n);


int main() {
    std::cout << "# n\t\t Theta(n)\t\tTheta(2^n)\tTheta(log n)" << std::endl;

    for (unsigned n = 0; n <= 24; n++) {
        pasos2_1 = pasos2_2 = pasos2_3 = 0;

        unsigned long res1 = pow2_1(n);
        unsigned long res2 = pow2_2(n);
        unsigned long res3 = pow2_3(n);
        unsigned long esperado = (unsigned long) pow(2, n);

        if (res1 != esperado || res2 != esperado || res3 != esperado) {
            std::cerr << "Error: resultados incorrectos para n = " << n << std::endl;
            return 1;
        }
        
        std::cout << std::setw(3) << n << " "
          << std::setw(12) << pasos2_1 << " "
          << std::setw(12) << pasos2_2 << " "
          << std::setw(12) << pasos2_3 << std::endl;
    }

    return 0;
}

// coste: Theta(n)
unsigned long pow2_1(unsigned n) {
    unsigned long res = 0;

    pasos2_1++;
    if (n == 0)
        res = 1;
    else 
        res = 2*pow2_1(n-1);
    
    return res;
}

// coste: Theta(2^n)
unsigned long pow2_2(unsigned n) {
    unsigned long res = 0;;

    pasos2_2++;
    if (n==0)
        res = 1;
    else
        res = pow2_2(n-1) + pow2_2(n-1);

    return res;
}

// coste: Theta(log n)
unsigned long pow2_3(unsigned n) {
    unsigned long res = 0;

    pasos2_3++;
    if (n==0)
        res = 1;
    else {
        res = pow2_3(n/2);
        res = res * res;
        if (n%2==1)
            res *= 2;
    }

    return res;
        
}