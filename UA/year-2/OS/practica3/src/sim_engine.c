#include "sim_engine.h"
#include "ficheros.h"
#include "file_utils.h"
#include <stdio.h>
#include <string.h>

void inicializar_memoria(Memoria *m) {
    // Inicializamos la memoria con un único hueco que ocupa toda la memoria disponible
    m->cant_particiones = 1;

    // El último índice asignado empieza en 0
    m->ultimo_indice_asignado = 0;

    // Única partición: Hueco de tamaño MEMORIA_TOTAL al inicio
    m->particiones[0].dir_inicio = 0;
    m->particiones[0].tamano = MEMORIA_TOTAL;
    // Estado 0 indica que es un hueco libre
    m->particiones[0].estado = 0;
    // Nombre del proceso "HUECO" para indicar que está libre
    strcpy(m->particiones[0].nombre_proceso, "HUECO");
}

void mostrar_estado(Memoria *m) {
    // Recorremos todas las particiones y mostramos su estado en formato [dir_inicio nombre_proceso tamano]
    for (int i = 0; i < m->cant_particiones; i++) {
        printf("[%d %s %d]", m->particiones[i].dir_inicio, m->particiones[i].nombre_proceso, m->particiones[i].tamano);
    }
    printf("\n");
}

int ocupar_memoria(Memoria *m, int indice_hueco, Proceso p) {
    // Obtenemos un puntero a la partición hueco que vamos a ocupar
    Particion *hueco = &m->particiones[indice_hueco];
    
    // Verificamos que el hueco es libre y que cabe el proceso si no, error
    if (hueco->estado != 0 || hueco->tamano < p.mem_requerida) {
        printf("ERROR: no se puede asignar el proceso \n");
        return 0;
    }

    // CASO A: Ajuste Exacto (El proceso ocupa todo el hueco)
    if (hueco->tamano == p.mem_requerida) {
        hueco->estado = 1;
        strcpy(hueco->nombre_proceso, p.nombre);
    } else {
        // CASO B: Hay que dividir (El hueco es más grande)
        // 1. Verificamos que no desbordamos el array
        if (m->cant_particiones >= MAX_PARTICIONES) return 1;

        // 2. DESPLAZAMIENTO: Movemos todo una posición a la derecha
        // desde el final hasta el siguiente al hueco actual
        for (int i = m->cant_particiones; i > indice_hueco + 1; i--) {
            m->particiones[i] = m->particiones[i - 1];
        }

        int tamano_restante = hueco->tamano - p.mem_requerida;
        int dir_nueva = hueco->dir_inicio + p.mem_requerida;

        // 3. Crear el NUEVO hueco restante en la posición siguiente (i+1)
        m->particiones[indice_hueco + 1].dir_inicio = dir_nueva;
        m->particiones[indice_hueco + 1].tamano = tamano_restante;
        m->particiones[indice_hueco + 1].estado = 0; // LIBRE
        strcpy(m->particiones[indice_hueco + 1].nombre_proceso, "HUECO");

        // 4. Actualizar el hueco actual para convertirlo en PROCESO
        hueco->tamano = p.mem_requerida;
        hueco->estado = 1; // OCUPADO
        strcpy(hueco->nombre_proceso, p.nombre);

        // 5. Actualizamos contadores
        m->cant_particiones++;
    }
    // Actualizamos el puntero para Next Fit (para cuando lo implementemos)
    m->ultimo_indice_asignado = indice_hueco;
    
    return 1; // Éxito
}

void compactar(Memoria *m) {
    // Recorremos hasta el penúltimo elemento
    for (int i = 0; i < m->cant_particiones - 1; i++) {
        // Si la partición actual y la siguiente son huecos, las unimos
        if (m->particiones[i].estado == 0 && m->particiones[i+1].estado == 0) {
            // 1. Sumamos tamaños de las particiones
            m->particiones[i].tamano += m->particiones[i+1].tamano;
            // 2. Movemos todas las particiones siguientes una posición a la izquierda
            for (int j = i + 1; j < m->cant_particiones - 1; j++) {
                m->particiones[j] = m->particiones[j+1];
            }
            // 3. Actualizamos el contador de particiones
            m->cant_particiones--;
            // 4. Retrocedemos el índice para revisar la nueva partición fusionada
            i--;

            printf("[DEBUG] Compactacion realizada del proceso %s\n", m->particiones[i].nombre_proceso);
        }
    }
}

bool liberar_proceso(Memoria *m, char *nombre_proceso) {
    for (int i = 0; i < m->cant_particiones; i++) {
        // Si encontramos el proceso y esta ocupado, lo liberamos
        if (m->particiones[i].estado == 1 && strcmp(m->particiones[i].nombre_proceso, nombre_proceso) == 0) {
            // 1. Lo convertimos en hueco pasando a estado 0
            m->particiones[i].estado = 0;
            strcpy(m->particiones[i].nombre_proceso, "HUECO");
            printf("[DEBUG] Proceso %s liberado en dir %d\n", nombre_proceso, m->particiones[i].dir_inicio);

            // 2. Compactamos la memoria para unir huecos adyacentes
            compactar(m);

            return true;
        }
    }
    return false;
}

int buscar_hueco(Memoria *m, int mem_requerida, TipoAlgo tipo_algo) {
    // Algoritmo Primer Hueco
    if (tipo_algo == ALGO_PRIMER_HUECO) {
        // Recorremos todas las particiones buscando el primer hueco que quepa
        for (int i = 0; i < m->cant_particiones; i++) {
            // Si es un hueco libre y cabe el proceso
            if (m->particiones[i].estado == 0 && m->particiones[i].tamano >= mem_requerida) {
                return i; // Devolvemos el índice del hueco encontrado
            }
        }
    }

    // Algoritmo Siguiente Hueco
    if (tipo_algo == ALGO_SIGUIENTE_HUECO) {
        // El punto de partida es el último índice asignado
        int inicio = m->ultimo_indice_asignado;
        // Sacamos el total de particiones para poder hacer la búsqueda circular
        int total = m->cant_particiones;

        // Recorremos todas las particiones empezando desde 'inicio' de forma circular
        for (int j = 0; j < total; j++) {
            // Calculamos el índice real con módulo para la circularidad
            int i = (inicio + j) % total;

            // Si es un hueco libre y cabe el proceso devolvemos su índice en la memoria
            if (m->particiones[i].estado == 0 && m->particiones[i].tamano >= mem_requerida)
                return i;
        }
    }

    return -1;
}

int alinear_size(int size) {
    int tam_final; // Variable para el tamaño final alineado

    // Si el tamaño es mayor que la unidad mínima, lo alineamos
    if (size > UNIDAD_MINIMA) {
        int bloques = size / 100; // Cantidad de bloques completos de 100
        if (size % 100 != 0) // Si hay resto
            tam_final = UNIDAD_MINIMA * bloques + 100; // Sumamos un bloque más
        else 
            tam_final = bloques * 100; // Exacto, no hay resto
    } else 
        tam_final = UNIDAD_MINIMA; // Si es menor o igual a 100, lo ajustamos a 100
    
    return tam_final;
}

bool asignar_proceso(Memoria *m, Proceso p, TipoAlgo tipo_algo) {
    // Paso 1 -> Calculamos el tamaño real alineado
    int tam_real = alinear_size(p.mem_requerida);

    // [Opcional] Debug para ver el cambio
    if (tam_real != p.mem_requerida)
        printf("[DEBUG Alineacion] Proceso %s pide %d pero ocupara %d\n", p.nombre, p.mem_requerida, tam_real);
    
    // Paso 2 -> Buscamos un hueco adecuado con el tamaño real y el algoritmo indicado
    int pos_mem = buscar_hueco(m, tam_real, tipo_algo);

    // Si no se encontró hueco, devolvemos false
    if (pos_mem == -1)
        return false;
    
    // Paso 3 -> Actualizamos el requerimiento de memoria del proceso al tamaño real
    p.mem_requerida = tam_real;
    
    // Paso 4 -> Intentamos ocupar el hueco encontrado
    return ocupar_memoria(m, pos_mem, p);
}

void avanzar_tiempo(Memoria *m, Proceso procesos[], int num_procesos, int *reloj_actual, TipoAlgo algo, const char* ruta_log) {
    printf("\n-----INSTANTE %d----\n", *reloj_actual);

    // Paso 1 y 2 -> Envejecimiento y finalización de procesos
    for (int i = 0; i < num_procesos; i++) {
        // Solo procesamos los que están en memoria y no han finalizado
        if (procesos[i].en_memoria && !procesos[i].finalizado) {
            // DEBUG: Ver cuánto le queda antes de restar ---
            printf("[DEBUG Vida] %s tiene %d ticks restantes.\n", procesos[i].nombre    , procesos[i].t_restante);
            procesos[i].t_restante--; // Disminuimos su tiempo restante

            // Si ya no le queda tiempo, lo finalizamos
            if (procesos[i].t_restante <= 0) {
                printf("-> FIN: El proceso %s ha terminado. Liberando memoria...\n", procesos[i].nombre);

                // Liberamos su memoria
                liberar_proceso(m, procesos[i].nombre);

                // Marcamos el proceso como finalizado
                procesos[i].en_memoria = false;
                procesos[i].finalizado = true;
            }
        }
    }

    // Paso 3 -> Llegada de nuevos procesos
    for (int i = 0; i < num_procesos; i++) {
        // Se busca proceso que no esté en memoria, no finalizado y que haya llegado o su momento ya haya pasado
        if (!procesos[i].en_memoria && !procesos[i].finalizado && procesos[i].t_llegada <= *reloj_actual) {
            printf("-> LLEGADA: %s intenta entrar (Requiere %d)\n", procesos[i].nombre, procesos[i].mem_requerida);

            // Intentamos asignarlo en memoria
            if (asignar_proceso(m, procesos[i], algo)) {
                printf("EXITO: %s asignado en memoria \n", procesos[i].nombre);
                procesos[i].en_memoria = true;
                // Inicializamos su tiempo restante
                procesos[i].t_restante = procesos[i].t_ejecucion;
            } else {
                // No se pudo asignar, se queda esperando, al ser t_llegada <= reloj_actual, volverá a intentar en el siguiente tick
                printf("ESPERA: No hay hueco para %s. Esperara al siguente tick \n", procesos[i].nombre);
            }
        }
    }

    // Paso 4 -> Guardar estado en log
    guardar_estado(ruta_log, m, *reloj_actual);

    // Paso 5 -> Incrementamos el reloj
    (*reloj_actual)++;
}