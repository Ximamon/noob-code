# ==========================================
# Makefile Estructurado con Comprobación, Setup y Consola
# ==========================================

# Compilador y flags
NVCC = nvcc
NVCC_FLAGS = -O2
LIBS = -lcudart

# Nombre del archivo principal (sin extensión)
TARGET = hello

# Carpetas del proyecto
SRC_DIR = src
BUILD_DIR = build
RESULT_DIR = result

# Ruta base de GPGPU-Sim y configuración (ajusta si es necesario)
GPGPUSIM_ROOT = $(HOME)/gpgpu-sim_distribution
CONFIG_DIR = $(GPGPUSIM_ROOT)/configs/tested-cfgs/SM7_QV100

# ==========================================
# Reglas principales
# ==========================================

# Regla por defecto: comprueba, prepara y compila
all: check_dirs setup $(BUILD_DIR)/$(TARGET)

# 0. FASE DE COMPROBACIÓN: Verifica y crea carpetas si no existen
check_dirs:
	@echo "--- [0/3] Comprobando directorios ---"
	@if [ ! -d $(BUILD_DIR) ]; then echo "Creando carpeta $(BUILD_DIR)/..."; mkdir -p $(BUILD_DIR); fi
	@if [ ! -d $(RESULT_DIR) ]; then echo "Creando carpeta $(RESULT_DIR)/..."; mkdir -p $(RESULT_DIR); fi
	@if [ ! -d $(SRC_DIR) ]; then echo "Creando carpeta $(SRC_DIR)/..."; mkdir -p $(SRC_DIR); fi

# 1. FASE DE SETUP: Copia configuraciones si no están presentes
setup: check_dirs
	@echo "--- [1/3] Preparando entorno GPGPU-Sim ---"
	@if [ ! -f $(BUILD_DIR)/gpgpusim.config ]; then \
		echo "Copiando configuración desde GPGPU-Sim a $(BUILD_DIR)/..."; \
		cp -r $(CONFIG_DIR)/* $(BUILD_DIR)/; \
	else \
		echo "La configuración ya está presente en $(BUILD_DIR)/ (omitido)."; \
	fi

# 2. FASE DE COMPILACIÓN: Compila el código .cu
$(BUILD_DIR)/$(TARGET): $(SRC_DIR)/$(TARGET).cu | check_dirs
	@echo "--- [2/3] Compilando $(TARGET).cu ---"
	$(NVCC) $(NVCC_FLAGS) $< -o $@ $(LIBS)
	@echo "Compilación exitosa."

# ==========================================
# Fase de Ejecución y volcado de resultados
# ==========================================

# 3. FASE DE EJECUCIÓN: Lanza el simulador, filtra e imprime
run: all
	@echo "--- [3/3] Ejecutando GPGPU-Sim ---"
	@cd $(BUILD_DIR) && ./$(TARGET) > ../$(RESULT_DIR)/resultado_completo.txt
	@echo "Simulación terminada. Extrayendo salida limpia..."
	@awk ' \
		BEGIN { p = 0 } \
		/Extracting specific PTX file named/ { p = 1; next } \
		/^kernel_name =/ { p = 0 } \
		/^gpu_sim_cycle/ { p = 0 } \
		/detected inactive GPU simulation thread/ { p = 1; next } \
		/^GPGPU-Sim/ { next } \
		/^self exe links to:/ { next } \
		/^Running md5sum using/ { next } \
		/^Destroy streams for kernel/ { next } \
		NF == 0 { next } \
		p == 1 { print $$0 } \
	' $(RESULT_DIR)/resultado_completo.txt > $(RESULT_DIR)/resultado_kernel.txt
	@if [ ! -s $(RESULT_DIR)/resultado_kernel.txt ]; then echo "No se encontró salida (¿El programa no imprime nada o hubo un error?)" > $(RESULT_DIR)/resultado_kernel.txt; fi
	@echo ""
	@echo "================ SALIDA DEL PROGRAMA =============="
	@cat $(RESULT_DIR)/resultado_kernel.txt
	@echo "==================================================="
	@echo "\nEl log completo del simulador está en $(RESULT_DIR)/resultado_completo.txt"

# ==========================================
# Limpieza profunda
# ==========================================

clean:
	@echo "Limpiando el directorio de trabajo..."
	@rm -rf $(BUILD_DIR)/* $(RESULT_DIR)/*
	@echo "¡Carpetas build/ y result/ vaciadas y listas para usar!"

.PHONY: all check_dirs setup run clean