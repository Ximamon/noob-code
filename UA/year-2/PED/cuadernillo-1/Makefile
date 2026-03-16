# -----------------------------------------------------------------------------
# Configuración del Compilador
# -----------------------------------------------------------------------------
CC = g++ # Version: 13.2.0
FLAGS =-g -O3 -Wall -Wextra -std=c++11 -I $(INCLUDE_DIR)
EXEC = tad
# Cambiar a "tad" si se quiere ejecutar test

# -----------------------------------------------------------------------------
# Directorios y Archivos
# -----------------------------------------------------------------------------
SRC_DIR = src
LIB_DIR = lib
BUILD_DIR = build
INCLUDE_DIR = include

# Búsqueda de fuentes
#SRCS = $(wildcard $(SRC_DIR)/*.cpp)
SRCS = $(SRC_DIR)/tad.cpp
LIBS = $(wildcard $(LIB_DIR)/*.cpp)

# Generación de la lista de objetos
OBJS = $(patsubst $(LIB_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(LIBS))

# -----------------------------------------------------------------------------
# Reglas Principales
# -----------------------------------------------------------------------------
.PHONY: all run test clean

all: $(EXEC)

# Ejecutar el programa
run: $(EXEC)
	./$(EXEC)

# Ejecutar tests y limpiar
test:
	./evalua.sh

# Limpieza
clean:
	rm -rf $(BUILD_DIR) $(EXEC) src/tad.cpp ComplejoPRUEBA/*.out TVectorCom_PRUEBA/*.out TListaCom_PRUEBA/*.out

# -----------------------------------------------------------------------------
# Reglas de Compilación
# -----------------------------------------------------------------------------

# Linkeo final (Creación del ejecutable)
$(EXEC): $(OBJS)
	$(CC) $(FLAGS) $(SRC_DIR)/$(EXEC).cpp $^ -o $@

# Compilación de objetos desde SRC
#$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	#$(CC) $(FLAGS) -c $< -o $@

# Compilación de objetos desde LIB
$(BUILD_DIR)/%.o: $(LIB_DIR)/%.cpp | $(BUILD_DIR)
	$(CC) $(FLAGS) -c $< -o $@

# Crear directorio de build
$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

# =============================================================================
# Variables automaticas de Make
# =============================================================================
#
# $@  -> El nombre del OBJETIVO (Target).
#        Ej: En 'app: main.o', $@ es 'app'.
#        Uso: Para nombrar el archivo de salida (-o $@).
#
# $<  -> La PRIMERA dependencia solamente.
#        Ej: En 'main.o: main.cpp main.h', $< es 'main.cpp'.
#        Uso: Para compilar un solo archivo fuente (-c $<).
#
# $^  -> TODAS las dependencias (sin duplicados).
#        Ej: En 'app: main.o utils.o', $^ es 'main.o utils.o'.
#        Uso: Para el linkeo final, pasando todos los objetos.
#
# =============================================================================

