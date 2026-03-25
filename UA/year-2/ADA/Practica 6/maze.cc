#include <iostream>
#include <vector>
#include <fstream>
#include <cstring>
#include <algorithm>
#include <limits>

#define INFINITO std::numeric_limits<long>::max()
#define SENTINEL -1

struct args {
    bool t = false;
    bool p2D = false;
    bool ignore_naive = false;
    std::string filename;
};

void mostrar_uso() {
    std::cerr << "Usage:\nmaze [-t] [--p2D] [--ignore-naive] -f file" << std::endl;
}

void maze_parser(int argc, char* argv[], args& arguments) {
    for (int i = 1; i < argc; i++) {
        if (std::string(argv[i]) == "-t") {
            arguments.t = true;
        } else if (std::string(argv[i]) == "--p2D") {
            arguments.p2D = true;
        } else if (std::string(argv[i]) == "--ignore-naive") {
            arguments.ignore_naive = true;
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

void show_maze(const std::vector<std::vector<int>>& maze) {
    for (size_t i = 0; i < maze.size(); i++) {
        for (size_t j = 0; j < maze[i].size(); j++) {
            std::cout << maze[i][j] << " ";
        }
        std::cout << std::endl;
    }
}

long maze_naive(const std::vector<std::vector<int>>& maze, int i, int j) {
    if (i < 0 || j < 0 || maze[i][j] == 0)
        return INFINITO;
    else {
        if (i == 0 && j == 0)
            return 1;
        else {

            // Definimos los tres posibles movimientos: diagonal, izquierda y arriba
            long s1 = INFINITO; // Izquierda (maze[i][j-1])
            long s2 = INFINITO; // Arriba (maze[i-1][j])
            long s3 = INFINITO; // Diagonal (maze[i-1][j-1])

            // Vemos si se puede ir a la izquierda
            if (j > 0 && maze[i][j-1] == 1)
                s1 = maze_naive(maze, i, j - 1);

            // Vemos si se puede ir hacia arriba
            if (i > 0 && maze[i-1][j] == 1)
                s2 = maze_naive(maze, i - 1, j);

            // Vemos si se puede ir en diagonal
            if (i > 0 && j > 0 && maze[i-1][j-1] == 1)
                s3 = maze_naive(maze, i - 1, j - 1);
            
            // Devolvemos el mínimo de los tres movimientos más uno (por el movimiento actual)
            if (s1 != INFINITO || s2 != INFINITO || s3 != INFINITO)
                return std::min(s1, std::min(s2, s3)) + 1;
            else
                return INFINITO;
        }
    }
}

long maze_memo(const std::vector<std::vector<int>>&maze, int i, int j, std::vector<std::vector<long>>& memo) {
    // Si ya hemos calculado el resultado para esta posición, lo devolvemos
    if (memo[i][j] != SENTINEL)
        return memo[i][j]; 
    else {
        if (maze[i][j] == 0)
            return memo[i][j] = INFINITO; // Si es un muro, devolvemos infinito
        else {
            if (i == 0 && j == 0)
                return memo[i][j] = 1; // Si estamos en la posición inicial, el número de pasos es 1
            else {
                // Definimos los tres posibles movimientos: diagonal, izquierda y arriba
                long s1 = INFINITO; // Izquierda (maze[i][j-1])
                long s2 = INFINITO; // Arriba (maze[i-1][j])
                long s3 = INFINITO; // Diagonal (maze[i-1][j-1])

                // Vemos si se puede ir a la izquierda
                if (j > 0)
                    s1 = maze_memo(maze, i, j - 1, memo);
                    
                // Vemos si se puede ir hacia arriba
                if (i > 0)
                    s2 = maze_memo(maze, i - 1, j, memo);

                // Vemos si se puede ir en diagonal
                if (i > 0 && j > 0)
                    s3 = maze_memo(maze, i - 1, j - 1, memo);
                    
                // Devolvemos el mínimo de los tres movimientos más uno (por el movimiento actual)
                if (s1 != INFINITO || s2 != INFINITO || s3 != INFINITO)
                    return memo[i][j] = std::min(s1, std::min(s2, s3)) + 1;
                else
                    return memo[i][j] = INFINITO;
        }
        }
    }
}

void memo_print(const std::vector<std::vector<long>>& memo) {
    for (const auto& row : memo) {
        for (const auto& val : row) {
            if (val == INFINITO)
                std::cout << "X ";
            else if (val == SENTINEL)
                std::cout << "- ";
            else
                std::cout << val << " ";
        }
        std::cout << std::endl;
    }
}

int main(int argc, char* argv[]) {

    args arguments;
    maze_parser(argc, argv, arguments);

    int n, m;
    std::vector<std::vector<int>> maze;
    leer_laberinto(arguments.filename, n, m, maze);
    // show_maze(maze);

    // Gestion de maze_naive
    if (arguments.ignore_naive)
        std::cout << "- ";
    else {
        long res_naive = maze_naive(maze, n - 1, m - 1);
        if (res_naive == INFINITO)
            std::cout << "0 ";
        else
            std::cout << res_naive << " ";
    }

    // Gestion de maze_memo
    std::vector<std::vector<long>> memo(n, std::vector<long>(m, SENTINEL));
    long res_memo = maze_memo(maze, n - 1, m - 1, memo);
    if (res_memo == INFINITO)
        std::cout << "0 ";
    else        
        std::cout << res_memo << " ";

    std::cout << "? ? " << std::endl; // Placeholder para memo y iterativo


    if (arguments.t) {
        std::cout << "Memoization table:" << std::endl;
	    memo_print(memo);
    }
    
    return 0;
}

