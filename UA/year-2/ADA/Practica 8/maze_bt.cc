// Julian Hinojosa Gil, 48795869N

#include <iostream>
#include <vector>
#include <fstream>
#include <cstring>
#include <algorithm>
#include <limits>
#include <chrono>

// Macro para representar el infinito, es el valor máximo de long, que es el tipo que usamos para los costes.
#define INFINITO std::numeric_limits<long>::max()


// Estructura que representa los argumentos de la linea de comandos
struct args {
    bool p2D = false;       // Muestra el laberinto con la solución marcada
    bool p = false;         // Muestra la ruta exacta seguida por la solución
    std::string filename;   // Nombre del archivo que contiene el laberinto
};

// Orden de las direcciones a explorar: SE, S, E, SW, NE, W, NW, N
const std::vector<unsigned> orden_dirs = {4, 5, 3, 6, 2, 7, 1, 8};

// Matriz de incrementos para cada dirección (0 es un placeholder para facilitar el acceso)
const int dirs[9][2] = {
    {0, 0},
    {-1, 0}, // 1: N
    {-1, 1}, // 2: NE
    {0, 1},  // 3: E
    {1, 1},  // 4: SE
    {1, 0},  // 5: S
    {1, -1}, // 6: SW
    {0, -1}, // 7: W
    {-1, -1} // 8: NW
};

// Estructura para almacenar las estadísticas del algoritmo
struct Stats {
    long visitados = 0;
    long explorados = 0;
    long hojas = 0;
    long descartados_no_factibles = 0;
    long descartados_no_prometedores = 0;
};

void mostrar_uso() {
    std::cerr << "Usage:\nmaze_bt [-p] [--p2D] -f file" << std::endl;
}

// Función de estimación heurística: Distancia de Chebyshev al destino
long estimacion(int filas, int columnas, int i, int j) {
    return std::max(filas - 1 - i, columnas - 1 - j);
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

void maze_bt(const std::vector<std::vector<int>>& maze,     // Laberinto
            int i, int j, int k,                            // Posición actual (i, j) y coste acumulado k 
            std::vector<std::vector<long>>& costs,          // Matriz de costes mínimos para llegar a cada celda  
            std::vector<unsigned>& current_path,            // Llevamos la ruta en la mano
            long& bestSol,                                  // Mejor solución encontrada hasta ahora
            std::vector<unsigned>& best_path,               // Aquí guardaremos la ruta ganadora
            Stats& st                                       // Estructura para almacenar las estadísticas del algoritmo
        ) {
    
    st.visitados++;     // Contamos la visita a esta celda

    // Tamaño del laberinto
    int n = maze.size();
    int m = maze[0].size();

    // Si hemos llegado a la meta
    if (i == n - 1 && j == m - 1) {
        st.hojas++;
        // Si el coste de esta solución es mejor que la mejor encontrada hasta ahora, 
        // la actualizamos y guardamos la ruta exacta que nos llevó a esta solución
        if (k < bestSol) {
            bestSol = k;
            best_path = current_path;
        }
    } 
    else {
        // Si no hemos llegado a la meta, exploramos las 8 direcciones posibles en el orden especificado
        st.explorados++;
        for (int p = 0; p < 8; p++) {
            unsigned direccion = orden_dirs[p];
            int inc_i = dirs[direccion][0];
            int inc_j = dirs[direccion][1];
            int isig = i + inc_i;
            int jsig = j + inc_j;

            // 1. Factible, es decir, dentro de los límites del laberinto y sin pared
            if (isig >= 0 && isig < n && jsig >= 0 && jsig < m && maze[isig][jsig] == 1) {
                // 2. Prometedor por coste, es decir, si el coste acumulado hasta esta celda es menor que el mejor coste registrado para esta celda
                if (k + 1 < costs[isig][jsig]) {
                    // 3. Prometedor por heurística , es decir, si el coste acumulado hasta esta celda más la estimación heurística del coste restante es menor que la mejor solución encontrada hasta ahora
                    if (k + 1 + estimacion(n, m, isig, jsig) < bestSol) {
                        
                        // Guardamos el coste anterior para esta celda antes de actualizarlo, para poder hacer backtracking después
                        long coste_anterior = costs[isig][jsig];
                        costs[isig][jsig] = k + 1;
                        
                        // Añadimos el paso a nuestra ruta actual
                        current_path.push_back(direccion); 
                        
                        // Llamada recursiva para explorar esta dirección
                        maze_bt(maze, isig, jsig, k + 1, costs, current_path, bestSol, best_path, st);
                        
                        // Backtracking: Deshacemos el paso y el coste al volver
                        current_path.pop_back(); 
                        costs[isig][jsig] = coste_anterior; 

                    } else {
                        st.descartados_no_prometedores++;
                    }
                } else {
                    st.descartados_no_prometedores++;
                }
            } else {
                st.descartados_no_factibles++;
            }
        }
    }
}

int main(int argc, char* argv[]) {

    // 1. Parseo de argumentos
    args arguments;
    maze_parser(argc, argv, arguments);

    // 2. Lectura del laberinto desde el archivo
    int n, m;
    std::vector<std::vector<int>> maze;
    leer_laberinto(arguments.filename, n, m, maze);

    // Caso especial: Si la celda de inicio es un muro, no hay solución posible, así que imprimimos los resultados correspondientes y salimos.
    if (maze[0][0] == 0) {
        std::cout << "0\n0 0 0 0 0\n0.0\n";
        if (arguments.p2D) std::cout << "0\n";
        if (arguments.p) std::cout << "<0>\n";
        return 0;
    }

    // 3. Inicialización de estructuras para el backtracking
    std::vector<std::vector<long>> costs(n, std::vector<long>(m, INFINITO));
    std::vector<unsigned> current_path; // Vector para la ruta activa
    std::vector<unsigned> best_path;    // Vector para la ruta ganadora
    Stats st;
    long bestSol = INFINITO;
    
    // 4. El coste para la celda de inicio es 1, ya que es el primer paso que damos (estamos en esa celda), y esto nos ayudará a evitar problemas con la estimación heurística y la comparación de costes.
    costs[0][0] = 1;

    // 5. Arrancamos el tiempo de ejecución del backtracking
    auto start = std::chrono::high_resolution_clock::now();

    // 6. Llamada inicial al backtracking, empezando desde la posición (0, 0) con un coste acumulado de 1 (la celda de inicio)
    maze_bt(maze, 0, 0, 1, costs, current_path, bestSol, best_path, st);

    // 7. Paramos el tiempo de ejecución del backtracking
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> duration = end - start;

    // --- Construcción del string y dibujo sobre el mapa (Hacia adelante) ---
    std::string camino_str = "<";
    if (bestSol != INFINITO) {
        int curr_i = 0, curr_j = 0;
        maze[curr_i][curr_j] = 2; // Marcamos el origen
        
        // Recorremos los pasos exactos que guardó la ruta ganadora
        for (unsigned dir : best_path) {
            camino_str += std::to_string(dir);
            curr_i += dirs[dir][0];
            curr_j += dirs[dir][1];
            maze[curr_i][curr_j] = 2; // Marcamos el mapa
        }
    } else {
        camino_str += "0";
    }
    camino_str += ">";

    // --- Impresión ---
    std::cout << (bestSol == INFINITO ? 0 : bestSol) << std::endl;
    std::cout << st.visitados << " " << st.explorados << " " << st.hojas << " " 
              << st.descartados_no_factibles << " " << st.descartados_no_prometedores << std::endl;
    std::cout << duration.count() << std::endl;

    if (arguments.p2D) {
        if (bestSol == INFINITO) {
            std::cout << "0" << std::endl;
        } else {
            for (int i = 0; i < n; i++) {
                for (int j = 0; j < m; j++) {
                    std::cout << (maze[i][j] == 2 ? '*' : (maze[i][j] ? '1' : '0'));
                }
                std::cout << std::endl;
            }
        }
    }

    if (arguments.p) {
        std::cout << camino_str << std::endl;
    }

    return 0;
}