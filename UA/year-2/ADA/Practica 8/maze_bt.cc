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
    bool p = false;
    std::string filename;
};

void mostrar_uso() {
    std::cerr << "Usage:\nmaze_bt [-p] [--p2D] -f file" << std::endl;
}

void maze_parser(int argc, char* argv[], args& arguments) {
    for (int i = 1; i < argc; i++) {
        if (std::string(argv[i]) == "--p2D") {
            arguments.p2D = true;
        } else if (std::string(argv[i]) == "-p") {
            arguments.p = true;
        } else if (std::string(argv[i]) == "-f") {
            if (i + 1 < argc) {
                arguments.filename = argv[++i];
            } else {
                std::cerr << "ERROR: missing filename." << std::endl;
                mostrar_uso();
                exit(1);
            }
        } else {
            std::cerr << "ERROR: unknown option " << argv[i] << "." << std::endl;
            mostrar_uso();
            exit(1);
        }
    }
    if (arguments.filename.empty()) {
        std::cerr << "ERROR: missing filename." << std::endl;
        mostrar_uso();
        exit(1);
    }
}

void leer_laberinto(const std::string& filename, int& n, int& m, std::vector<std::vector<int>>& maze) {
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "ERROR: can't open file: " << filename << "." << std::endl;
        mostrar_uso();
        exit(1);
    }

    file >> n >> m;
    maze.resize(n, std::vector<int>(m));
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) {
            file >> maze[i][j];
        }
    }
}


int main(int argc, char* argv[]) {
    args arguments;
    maze_parser(argc, argv, arguments);

    int n, m;
    std::vector<std::vector<int>> maze;
    leer_laberinto(arguments.filename, n, m, maze);

    // 3. Imprimimos el mapa si nos han pasado --p2D
    if (arguments.p2D) {
        // Dibujamos el laberinto
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < m; j++) {
                if (maze[i][j] == 2) {
                    std::cout << "*"; // Si es por donde hemos pisado, un asterisco
                } else {
                    std::cout << maze[i][j]; // Si no, imprimimos el 0 o el 1 original
                }
            }
            std::cout << std::endl; // Sin espacios entre números, solo salto de línea al final
        }
    }
    
    return 0;
}

