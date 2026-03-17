#include <iostream>
#include <vector>
#include <fstream>
#include <cstring>

struct args {
    bool t = false;
    bool p2D = false;
    bool ignore_naive = false;
    std::string filename;
};

struct Data{
    std::vector<std::vector<int>> maze;
    int fils, cols;
};

void maze_parser(int argc, char* argv[], args& arguments);
void mostrar_uso();
void leer_laberinto(const std::string& filename, int& n, int& m, std::vector<std::vector<int>>& maze);
void show_maze(const std::vector<std::vector<int>>& maze);

int main(int argc, char* argv[]) {

    args arguments;
    maze_parser(argc, argv, arguments);

    int n, m;
    std::vector<std::vector<int>> maze;
    leer_laberinto(arguments.filename, n, m, maze);
    show_maze(maze);

    
    return 0;
}

void mostrar_uso() {
    std::cerr << "Usage:\n maze [-t] [--p2D] [--ignore-naive] -f file" << std::endl;
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