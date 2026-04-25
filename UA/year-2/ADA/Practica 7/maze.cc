// Julian Hinojosa Gil, 48795869N

#include <iostream>
#include <vector>
#include <fstream>
#include <cstring>
#include <algorithm>
#include <limits>

#define INFINITO std::numeric_limits<long>::max()
#define SENTINEL -1

struct args {
    bool p2D = false;
    std::string filename;
};

void mostrar_uso() {
    std::cerr << "Usage:\nmaze [-t] [--p2D] [--ignore-naive] -f file" << std::endl;
}

int main(int argc, char* argv[]) {

    args arguments;
    
    
    return 0;
}

