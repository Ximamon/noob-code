# ==========================================
# Makefile Estructurado con Comprobación, Setup y Consola
# ==========================================

# Compilador y flags
NVCC = nvcc
NVCC_FLAGS = -O2
LIBS = --cudart shared

# Nombre del archivo principal (sin extensión)
TARGET = test_performance

# Carpetas del proyecto
SRC_DIR = src
BUILD_DIR = build
RESULT_DIR = result

# Ruta base de GPGPU-Sim y configuración (ajusta si es necesario)
GPGPUSIM_ROOT = $(HOME)/hpc_workspace/gpgpu-sim_distribution
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
	@echo "Simulación terminada. Generando resumen..."
	@awk ' \
		/^gpu_sim_cycle =/ { cycles = $$3 } \
		/^gpu_ipc =/ { ipc = $$3 } \
		/^T1:/ { t_line = $$0 } \
		/^TIEMPO M/ { t_min = $$0 } \
		END { \
			print t_line; \
			print t_min; \
			print "Ciclos Sim.   : " cycles; \
			print "IPC Sim.      : " ipc; \
		}' $(RESULT_DIR)/resultado_completo.txt > $(RESULT_DIR)/resultado_kernel.txt
	@echo ""
	@echo "=============== DATOS PARA LA TABLA ==============="
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