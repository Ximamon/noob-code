// Julian Hinojosa Gil, 48795869N

#include <iostream>
#include <vector>
#include <fstream>
#include <cstring>
#include <algorithm>
#include <limits>
#include <chrono>

#define INFINITO std::numeric_limits<long>::max()
#define SENTINEL -1

struct args {
    bool p2D = false;
    bool p = false;
    std::string filename;
};

const std::vector<unsigned> ve = {4, 5, 3, 6, 2, 7, 1, 8}; // Orden de las direcciones: derecha, abajo, izquierda, arriba

const int vinc[9][2] = {
    {0, 0},     // 0: No se mueve
    {0, -1},    // 1: izquierda
    {1, 0},     // 2: abajo
    {0, 1},     // 3: derecha
    {-1, 0},    // 4: arriba
    {1, -1},    // 5: abajo-izquierda
    {1, 1},     // 6: abajo-derecha
    {-1, -1},   // 7: arriba-izquierda
    {-1, 1}     // 8: arriba-derecha
};

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

void maze_bt(const std::vector<std::vector<int>>& maze, int i, int j, int k, 
             std::vector<std::vector<long>>& costs, 
             std::vector<std::vector<unsigned>>& from, 
             long& bestSol, 
             std::vector<std::vector<unsigned>>& best_from,
             Stats& st) {
    
    st.visitados++; // (1) Entramos a un nodo

    int n = maze.size();
    int m = maze[0].size();

    // ¿Hemos llegado a la meta?
    if (i == n - 1 && j == m - 1) {
        st.hojas++; // (3) Es un nodo hoja
        if (k < bestSol) {
            bestSol = k;
            best_from = from; // CONGELAMOS la ruta ganadora
        }
    } 
    else {
        // Expandimos en las 8 direcciones posibles
        st.explorados++; // (2) Vamos a generar sus hijos
        for (int p = 0; p < 8; p++) {
            unsigned direccion = ve[p];
            int inc_i = vinc[direccion][0];
            int inc_j = vinc[direccion][1];
            int isig = i + inc_i;
            int jsig = j + inc_j;

            // 1. ¿Es FACTIBLE? (No se sale del mapa y es un camino '1')
            if (isig >= 0 && isig < n && jsig >= 0 && jsig < m && maze[isig][jsig] == 1) {
                
                // 2. ¿Es PROMETEDOR por coste histórico? (No hemos pasado por aquí más rápido)
                if (k + 1 < costs[isig][jsig]) {
                    
                    // 3. ¿Es PROMETEDOR hacia el futuro? (Cota optimista < mejor solución)
                    if (k + 1 + estimacion(n, m, isig, jsig) < bestSol) {
                        
                        // Si pasa todos los filtros, actualizamos registros y llamamos a la recursión
                        long coste_anterior = costs[isig][jsig]; // Guardamos por si hay que deshacer
                        costs[isig][jsig] = k + 1;
                        from[isig][jsig] = direccion;
                        
                        maze_bt(maze, isig, jsig, k + 1, costs, from, bestSol, best_from, st);
                        
                        // Backtracking: Restauramos el coste al volver para no fastidiar otras ramas
                        costs[isig][jsig] = coste_anterior; 

                    } else {
                        st.descartados_no_prometedores++; // (5) Cortado por heurística
                    }
                } else {
                    st.descartados_no_prometedores++; // (5) Cortado por ya visitado mejor
                }
            } else {
                st.descartados_no_factibles++; // (4) Es un muro o fuera del mapa
            }
        }
    }
}

void reconstruir_camino(std::vector<std::vector<int>>& maze, 
                        const std::vector<std::vector<unsigned>>& best_from, 
                        std::string& camino_str) {
    int i = maze.size() - 1;
    int j = maze[0].size() - 1;
    
    // Si no se tocó la meta, devolvemos <0>
    if (best_from[i][j] == 0) {
        camino_str = "<0>";
        return;
    }

    camino_str = ">"; // Empezamos a construir el string desde el final
    maze[i][j] = 2;   // Marcamos la meta con asterisco
    
    while (i != 0 || j != 0) {
        unsigned dir = best_from[i][j];
        camino_str = std::to_string(dir) + camino_str;
        
        // Vamos marcha atrás: restamos el incremento que usamos para llegar
        i -= vinc[dir][0];
        j -= vinc[dir][1];
        
        maze[i][j] = 2; // Marcamos el camino con asterisco
    }
    
    camino_str = "<" + camino_str;
}

int main(int argc, char* argv[]) {
    args arguments;
    maze_parser(argc, argv, arguments);

    int n, m;
    std::vector<std::vector<int>> maze;
    leer_laberinto(arguments.filename, n, m, maze);

    // Si el laberinto de entrada está bloqueado en el origen, no hacemos nada
    if (maze[0][0] == 0) {
        std::cout << "0\n0 0 0 0 0\n0.0\n";
        if (arguments.p2D) std::cout << "0\n";
        if (arguments.p) std::cout << "<0>\n";
        return 0;
    }

    // Inicializamos las estructuras de datos para Backtracking
    std::vector<std::vector<long>> costs(n, std::vector<long>(m, INFINITO));
    std::vector<std::vector<unsigned>> from(n, std::vector<unsigned>(m, 0));
    std::vector<std::vector<unsigned>> best_from(n, std::vector<unsigned>(m, 0));
    Stats st;
    long bestSol = INFINITO;
    
    // El punto de partida ya cuesta 1 paso
    costs[0][0] = 1;

    // --- CRONÓMETRO: INICIO ---
    auto start = std::chrono::high_resolution_clock::now();

    // Llamamos a la función de Vuelta Atrás
    maze_bt(maze, 0, 0, 1, costs, from, bestSol, best_from, st);

    // --- CRONÓMETRO: FIN ---
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> duration = end - start;

    // Reconstruimos el camino (esto preparará el string y los asteriscos en la matriz)
    std::string camino_str;
    if (bestSol != INFINITO) {
        reconstruir_camino(maze, best_from, camino_str);
    } else {
        camino_str = "<0>";
    }

    // ==========================================
    // IMPRESIÓN ESTRICTA SEGÚN EL PDF
    // ==========================================

    // Línea 1: Longitud del camino o 0
    std::cout << (bestSol == INFINITO ? 0 : bestSol) << std::endl;

    // Línea 2: Estadísticas (visitados explorados hojas no_factibles no_prometedores)
    std::cout << st.visitados << " " 
              << st.explorados << " " 
              << st.hojas << " " 
              << st.descartados_no_factibles << " " 
              << st.descartados_no_prometedores << std::endl;

    // Línea 3: Tiempo en milisegundos
    std::cout << duration.count() << std::endl;

    // Línea 4+: Imprimir mapa si se pide --p2D
    if (arguments.p2D) {
        if (bestSol == INFINITO) {
            std::cout << "0" << std::endl;
        } else {
            for (int i = 0; i < n; i++) {
                for (int j = 0; j < m; j++) {
                    if (maze[i][j] == 2) {
                        std::cout << "*";
                    } else {
                        std::cout << maze[i][j];
                    }
                }
                std::cout << std::endl;
            }
        }
    }

    // Línea 5+: Imprimir secuencia numérica si se pide -p
    if (arguments.p) {
        std::cout << camino_str << std::endl;
    }

    return 0;
}

