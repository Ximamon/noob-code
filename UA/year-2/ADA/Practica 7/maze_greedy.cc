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
    std::cerr << "Usage:\nmaze_greedy [--p2D] -f file" << std::endl;
}

void maze_parser(int argc, char* argv[], args& arguments) {
    for (int i = 1; i < argc; i++) {
        if (std::string(argv[i]) == "--p2D") {
            arguments.p2D = true;
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

long maze_greedy(std::vector<std::vector<int>> &maze) {
    int n = maze.size();
    int m = maze[0].size();

    // Si la mismisima entrada es un muro, no podemos empezar
    if (maze[0][0] == 0) return 0;

    int i = 0, j = 0;   // Origen
    long pasos = 1;     // El origen cuenta como un paso

    // Marcamos la casilla inicial con un número especial (ej: 2) 
    // para diferenciar el "camino pisado" de los "1" normales.
    maze[0][0] = 2;

    // Mientras NO estemos en la meta...
    while (i != n - 1 || j != m - 1) {
        
        // 1ª Opción (La mejor): ¿Puedo ir en DIAGONAL?
        // Compruebo que no me salgo del mapa y que la casilla es un 1
        if (i + 1 < n && j + 1 < m && maze[i + 1][j + 1] == 1) {
            i++;
            j++;
        }
        // 2ª Opción: ¿Puedo ir hacia ABAJO?
        else if (i + 1 < n && maze[i + 1][j] == 1) {
            i++;
        }
        // 3ª Opción: ¿Puedo ir hacia la DERECHA?
        else if (j + 1 < m && maze[i][j + 1] == 1) {
            j++;
        }
        // 4ª Opción: ¡Callejón sin salida!
        else {
            return 0; // Nos hemos atascado, el método voraz fracasa.
        }

        // Si hemos logrado dar un paso, lo sumamos y marcamos la casilla con un 2
        pasos++;
        maze[i][j] = 2;
    }

    // Si el bucle termina de forma natural, es que hemos llegado a la meta
    return pasos;
}

int main(int argc, char* argv[]) {
    args arguments;
    maze_parser(argc, argv, arguments);

    int n, m;
    std::vector<std::vector<int>> maze;
    leer_laberinto(arguments.filename, n, m, maze);

    // 1. Llamamos a nuestra función voraz
    long resultado = maze_greedy(maze);

    // 2. Imprimimos el resultado numérico (con salto de línea final)
    std::cout << resultado << std::endl;

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

