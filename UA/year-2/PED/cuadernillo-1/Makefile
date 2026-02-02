CC = g++ # Version: 13.2.0
FLAGS = -g -Wall -Wextra -std=c++11
EXEC = main

# Directorios
SRC_DIR = src
LIB_DIR = lib
BUILD_DIR = build

# Archivos fuente y objetos
SRCS = $(wildcard $(SRC_DIR)/*.cpp)
LIBS = $(wildcard $(LIB_DIR)/*.cpp)
# Los objetos se generan en el directorio de build
OBJS = $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SRCS)) \
	   $(patsubst $(LIB_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(LIBS))


all: $(EXEC)

# Regla para el ejecutable final
$(EXEC): $(OBJS)
	$(CC) $(FLAGS) $^ -o $@

# Compilamos los objetos de lib y src en el directorio build
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	$(CC) $(FLAGS) -c $< -o $@
$(BUILD_DIR)/%.o: $(LIB_DIR)/%.cpp | $(BUILD_DIR)
	$(CC) $(FLAGS) -c $< -o $@

# Regla para crear el directorio de build si no existe
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Regla para limpiar los archivos generados
clean:
	rm -rf $(BUILD_DIR) $(EXEC)

.PHONY: clean