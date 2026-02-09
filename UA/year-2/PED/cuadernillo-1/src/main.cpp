#include <iostream>
#include "../include/TComplejo.h"

using namespace std;

int main() {

    TComplejo complejo;
    complejo.Re(5.5);
    complejo.Im(3.4);

    cout << "Hello " << complejo << endl;

    return 0;
}