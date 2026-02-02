/**
 * @file main.c
 * @brief Punto de entrada del simulador de gestión de memoria.
 * 
 * Implementa dos interfaces de usuario:
 * - **GUI**: Interfaz gráfica con Raylib (proceso hijo)
 * - **TUI**: Interfaz de terminal para depuración (proceso padre)
 * 
 * Usa fork() para ejecutar ambas interfaces en paralelo.
 * 
 * @author Julian Hinojosa Gil
 * @date 2025
 * @version 1.0
 */

#include <raylib.h>

#include "sim_engine.h"
#include "ficheros.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

/** @brief Ancho de la ventana gráfica en píxeles */
#define WIN_WIDTH 1200
/** @brief Alto de la ventana gráfica en píxeles */
#define WIN_HEIGHT 750
/** @brief Posición Y de la barra de memoria */
#define Y_BARRA 250
/** @brief Alto de la barra de memoria */
#define ALTO_BARRA 100
/** @brief Margen izquierdo de la interfaz */
#define MARGEN_IZQ 50
/** @brief Margen derecho de la interfaz */
#define MARGEN_DER 50

/* Declaraciones adelantadas */
void test_sim();
void run_gui(Memoria *m, Proceso *procesos, int num_procesos);

/**
 * @brief Punto de entrada principal del programa.
 * 
 * Crea dos procesos mediante fork():
 * - Proceso hijo (pid == 0): Ejecuta la interfaz gráfica (GUI)
 * - Proceso padre (pid > 0): Ejecuta la interfaz de terminal (TUI) y espera al hijo
 * 
 * @param argc Número de argumentos (no utilizado)
 * @param argv Array de argumentos (no utilizado)
 * 
 * @return 0 si la ejecución fue exitosa
 * @return -1 si hubo error en fork()
 */
int main(int argc, char const* argv[])
{
    pid_t pid = fork();

    if (pid < 0) {
        perror("Error en fork");
        return -1;
    }
    else if (pid == 0) {
        // Proceso Hijo: GUI (Raylib)
        Memoria m;
        inicializar_memoria(&m);
        
        Proceso procesos[MAX_PROCESOS];
        int num_procesos = cargar_procesos("entrada.txt", procesos);

        run_gui(&m, procesos, num_procesos);
        system("clear");
        _exit(0); // El hijo termina después de cerrar la GUI
    } else {
        system("clear");
        // Proceso Padre: TUI (Terminal)
        test_sim();
        // Esperar a que el hijo (GUI) termine
        waitpid(pid, NULL, 0);
    }

    return 0;
}

/**
 * @brief Ejecuta la simulación en modo terminal (TUI).
 * 
 * Carga procesos desde "entrada.txt" y ejecuta la simulación
 * paso a paso, mostrando el estado en consola.
 * 
 * @note Usa getchar() para avanzar manualmente entre ticks
 * @note Genera log en "particiones_tui.txt"
 * @note Usa algoritmo First Fit por defecto
 */
void test_sim() {
    system("clear"); // Limpiamos la consola al iniciar

    Memoria m;
    inicializar_memoria(&m);

    TipoAlgo algoritmo_actual = ALGO_SIGUIENTE_HUECO;
    
    limpiar_log("particiones_tui.txt");
    

    // // Simulamos procesos falsos para probar
    // Proceso procesos[2] = {
    //     {"P1", 0, 243, 5, 5, false, false}, // ID, Llegada, Tam, Duracion, Restante...
    //     {"P2", 1, 45, 2, 2, false, false}
    //     // {"P3", 2, 500, 5, 5, false, false},
    //     // {"P4", 3, 800, 3, 3, false, false}
    // };

    // Ahora con argumento de fichero
    Proceso procesos[MAX_PROCESOS]; // Maximo 100 procesos
    int num_procesos = cargar_procesos("entrada.txt", procesos);

    int reloj_sim = 0;

    int tiempo_total = 0; // Simulamos 20 instantes

    for (int i = 0; i < num_procesos; i++) {
        tiempo_total += procesos[i].t_ejecucion;
    }

    // --- BUCLE DE SIMULACIÓN ---
    printf("=== INICIO DE LA SIMULACIÓN COMPLEJA ===\n");
    printf("Memoria Total: %d\n\n", MEMORIA_TOTAL);

    for (int i = 0; i < tiempo_total; i++) {
        // Llamamos al motor de tiempo
        avanzar_tiempo(&m, procesos, num_procesos, &reloj_sim, algoritmo_actual, "particiones_tui.txt");
        
        // Visualizamos la memoria
        printf("MEMORIA: ");
        mostrar_estado(&m); // Mostramos el estado de la memoria con el formato adecuado [0 P1 500]...

        // Visualizamos quién está esperando (Cola de espera)
        printf("COLA ESPERA: ");
        int esperando = 0;
        for(int j=0; j<4; j++) {
            if (!procesos[j].en_memoria && !procesos[j].finalizado && procesos[j].t_llegada <= reloj_sim-1) {
                printf("[%s req:%d] ", procesos[j].nombre, procesos[j].mem_requerida);
                esperando++;
            }
        }
        if(esperando == 0) printf("(Nadie)");
        printf("\n----------------------------------------------------\n");
        
        printf("Presiona ENTER para continuar..."); // <-- AÑADE ESTO
        fflush(stdout);  // <-- Asegura que se muestre antes del getchar
        getchar(); // Descomenta para ir paso a paso con ENTER
    }
}

/**
 * @brief Ejecuta la interfaz gráfica con Raylib.
 * 
 * Crea una ventana de WIN_WIDTH x WIN_HEIGHT píxeles y muestra:
 * - Barra de memoria con particiones coloreadas
 * - Lista de procesos y su estado
 * - Controles interactivos
 * 
 * @param[in,out] m Puntero a la estructura de memoria del simulador
 * @param[in,out] procesos Array de procesos a simular
 * @param[in] num_procesos Cantidad de procesos en el array
 * 
 * @par Controles:
 * - ESPACIO: Avanzar un tick
 * - P: Activar/desactivar auto-play
 * - R: Reiniciar simulación
 * - ESC: Salir
 * 
 * @note Genera log en "particiones.txt"
 * @note Velocidad auto-play: 1 tick/segundo
 */
void run_gui(Memoria *m, Proceso *procesos, int num_procesos) {
    InitWindow(WIN_WIDTH, WIN_HEIGHT, "Gestomemoria - Memory Simulator");
    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second

    limpiar_log("particiones.txt");

    int reloj_sim = 0;
    TipoAlgo algoritmo_actual = ALGO_SIGUIENTE_HUECO;

    //Variables para repro automatica
    bool auto_play = false;
    float tiempo_acumulado = 0.0f;
    float velocidad = 1.0f; // 1 segundo por tick

    // Variables para los colores de los procesos
    Color colores_prcesos[] = {
        SKYBLUE,
        PINK,
        ORANGE,
        LIME,
        GOLD,
        GREEN,
        VIOLET,
        MAROON,
        BLUE,
        RED,
        PURPLE
    };
    int num_colores = sizeof(colores_prcesos) / sizeof(colores_prcesos[0]);

    // Main app loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {

        // Opcion A: Paso Manual(Tecla espacio)
        if (IsKeyPressed(KEY_SPACE))
            avanzar_tiempo(m, procesos, num_procesos, &reloj_sim, algoritmo_actual, "particiones.txt");

        // Opcion B: Auto Play(Tecla P)
        if (IsKeyPressed(KEY_P))
            auto_play = !auto_play;

        // Logica del auto play
        if (auto_play) {
            tiempo_acumulado += GetFrameTime();
            if (tiempo_acumulado >= velocidad) {
                avanzar_tiempo(m, procesos, num_procesos, &reloj_sim, algoritmo_actual, "particiones.txt");
                tiempo_acumulado = 0.0f;
            }
        }

        // Opcion C: Resetear (Tecla R)
        if (IsKeyPressed(KEY_R)) {
            inicializar_memoria(m);
            num_procesos = cargar_procesos("entrada.txt", procesos);
            reloj_sim = 0;
            limpiar_log("particiones.txt");
            auto_play = false;
        }
        
        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(RAYWHITE);
            
            // 1. Textos de informacion
            DrawText("Simulador de Gestion de Memoria", 20, 20, 30, DARKGRAY);
            DrawText(TextFormat("Instante Actual: %d", reloj_sim), 20, 60, 20, BLACK);

            DrawText("Pulsa ESPACIO para avanzar, P para auto play, R para resetear", 20, 90, 20, DARKGRAY);
            DrawText("Pulsa ESC para salir", 20, 120, 20, DARKGRAY);

            if (auto_play) DrawText("Auto Play: ON (P para Desactivar)", 20, 150, 20, DARKGREEN);
            else DrawText("Auto Play: OFF (P para Activar)", 20, 150, 20, RED);
            
            DrawText(TextFormat("Procesos Cargados: %d", num_procesos), 20, 180, 15, DARKGRAY);

            // 2. Dibujar la barra de memoria
            // Escala: Pantalla / Memoria -> WIN_WIDTH / MEMORIA_TOTAL
            float ancho_util = WIN_WIDTH - MARGEN_IZQ - MARGEN_DER;
            float escala = ancho_util / MEMORIA_TOTAL;

            for (int i = 0; i < m->cant_particiones; i++) {
                Particion p = m->particiones[i];

                // Coordenadas
                float x = MARGEN_IZQ + (p.dir_inicio * escala);
                float ancho = p.tamano * escala;

                // Colores de los bloques
                Color colorBloque;
                Color colorHueco = (Color){227, 217, 132, 255};

                // Determinar color segun estado
                // Si es hueco, colorHueco, si es proceso, color segun indice de color_procesos
                if (p.estado == 0) {
                    colorBloque = colorHueco;
                } else {
                    // Buscar indice del proceso por nombre para asignar color
                    int indice_color = 0;
                    for (int j = 0; j < num_procesos; j++) {
                        if (strcmp(procesos[j].nombre, p.nombre_proceso) == 0) {
                            // Si hay mas procesos que colores, reutilizamos por ejemplo, con 6 colores en 15 procesos, el 15 usará el color 3
                            indice_color = j % num_colores;
                            break;
                        }
                    }
                    colorBloque = colores_prcesos[indice_color];
                }

                // Dibujar el rectangulo
                DrawRectangle(x, Y_BARRA, ancho, ALTO_BARRA, colorBloque);
                DrawRectangleLines(x, Y_BARRA, ancho, ALTO_BARRA, Fade(BLACK, 0.5f));

                // Informacion del bloque (si cabe)
                if (ancho > 40) {
                    DrawText(p.nombre_proceso, x + 5, Y_BARRA + 35, 15, BLACK);
                    DrawText(TextFormat("%d", p.tamano), x + 5, Y_BARRA + 50, 15, DARKGRAY);
                }

                // Direccion de memoria (arriba del bloque)
                DrawText(TextFormat("%d", p.dir_inicio), x, Y_BARRA - 15, 15, BLACK);
            }

            // Marca final (2000)
            DrawText("2000", MARGEN_IZQ + ancho_util - 30, Y_BARRA - 15, 15, BLACK);

            // Tablas de procesos
            // -------------------------------------------------------------
        // 3. DIBUJAR TABLA DE PROCESOS (Debajo de la barra de memoria)
        // -------------------------------------------------------------
        int y_inicio_tabla = Y_BARRA + ALTO_BARRA + 40; // Empezamos 40px debajo de la barra
        int x_columna = 20;
        int ancho_columna = 120; // Espacio entre columnas
        
        DrawText("ESTADO DE PROCESOS:", x_columna, y_inicio_tabla - 25, 20, DARKGRAY);

        // Cabecera simple
        // DrawText("ID   Mem   Vida   Estado", x_columna, y_inicio_tabla, 10, BLACK);

        for (int i = 0; i < num_procesos; i++) {
            // Solo dibujamos si cabe en pantalla, si hay muchos habría que hacer paginación
            if (y_inicio_tabla + (i * 20) > WIN_HEIGHT - 20) break;

            // Determinar Color y Estado Texto
            Color colorEstado = LIGHTGRAY;
            const char* textoEstado = "FUTURO";

            if (procesos[i].finalizado) {
                colorEstado = GRAY;     // Gris oscuro para acabados
                textoEstado = "FIN";
            } 
            else if (procesos[i].en_memoria) {
                colorEstado = GREEN;    // Verde para activos
                textoEstado = "ACTIVO";
            } 
            else if (procesos[i].t_llegada <= reloj_sim) {
                colorEstado = RED;      // Rojo para cola de espera
                textoEstado = "COLA DE ESPERA";
            }
            else {
                colorEstado = LIGHTGRAY; // Gris claro para los que aun no llegan
                textoEstado = TextFormat("T=%d", procesos[i].t_llegada);
            }

            // Dibujamos la fila del proceso
            // Calculamos posición (hacemos varias columnas si hay muchos, opcional)
            int fila_y = y_inicio_tabla + (i * 20); 
            
            // 1. ID y Tamaño
            DrawText(TextFormat("%s (%d)", procesos[i].nombre, procesos[i].mem_requerida), 
                     x_columna, fila_y, 20, BLACK);
            
            // 2. Barra de vida (Pequeña barrita visual)
            if (procesos[i].en_memoria) {
                // Vida restante vs Total
                float porcentaje = (float)procesos[i].t_restante / procesos[i].t_ejecucion;
                DrawRectangle(x_columna + 100, fila_y + 5, 50 * porcentaje, 10, BLUE);
                DrawRectangleLines(x_columna + 100, fila_y + 5, 50, 10, BLACK);
            }

            // 3. Estado (Texto coloreado)
            DrawText(textoEstado, x_columna + 160, fila_y, 20, colorEstado);
        }

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------
}