.386
.model flat, c
.code

PUBLIC bubblesort

bubblesort PROC
    ; --- Prólogo de la función ---
    PUSH EBP
    MOV EBP, ESP

    ; --- Carga de parámetros ---
    MOV ECX, [EBP + 8]   ; ECX = Dirección base del arreglo
    MOV EDX, [EBP + 12]  ; EDX = Tamaño del arreglo

    ; AJUSTE DE SEGURIDAD: 
    ; Como comparamos el elemento actual (i) con el siguiente (i+1),
    ; el bucle solo debe llegar hasta el PENÚLTIMO elemento.
    ; Le restamos 1 al tamaño total para no leer fuera de la memoria.
    DEC EDX              
    
    ; Si el arreglo tiene 1 o 0 elementos, ya está ordenado
    CMP EDX, 0
    JLE DONE

OUTER_LOOP:
    MOV ESI, 0           ; Índice para el bucle interno (i = 0)
    MOV EBX, 0           ; Bandera para verificar si hubo intercambio (0 = Falso)

INNER_LOOP:
    CMP ESI, EDX         ; Comparamos el índice actual con (Tamaño - 1)
    JGE END_INNER_LOOP   ; Si ESI >= (Tamaño - 1), terminamos la pasada

    ; --- Leer elementos adyacentes ---
    ; Aquí aplicamos la fórmula pura de x86: Base + Índice*4 + Desplazamiento
    MOV EAX, [ECX + ESI*4]       ; Elemento actual: arr[i]
    MOV EDI, [ECX + ESI*4 + 4]   ; Siguiente elemento: arr[i+1] (sumamos 4 bytes)

    ; --- Comparación ---
    CMP EAX, EDI         ; Comparamos arr[i] con arr[i+1]
    JLE NO_SWAP          ; Si el actual es Menor o Igual, NO intercambiamos

    ; --- Intercambio (Swap) ---
    ; EAX tiene el mayor y EDI tiene el menor. Los guardamos cruzados en la memoria.
    MOV [ECX + ESI*4], EDI       ; Ponemos el menor en la posición actual
    MOV [ECX + ESI*4 + 4], EAX   ; Ponemos el mayor en la posición siguiente
    
    MOV EBX, 1           ; Levantamos la bandera: ¡Hubo al menos un intercambio!

NO_SWAP:
    INC ESI              ; Incrementamos el índice (i++)
    JMP INNER_LOOP       ; Volvemos al inicio del bucle interno

END_INNER_LOOP:
    ; --- Comprobación de la bandera ---
    CMP EBX, 0           ; ¿Hubo algún intercambio en esta pasada completa?
    JE DONE              ; Si EBX es 0, el arreglo ya está ordenado. ¡Salimos!
    
    ; Opcional: Cada pasada deja el número más grande al final. 
    ; Podríamos hacer un DEC EDX aquí para no volver a comprobar el último, 
    ; pero tu bandera de EBX ya lo hace súper eficiente.
    
    JMP OUTER_LOOP       ; Si hubo intercambios, repetimos el proceso

DONE:
    ; --- Epílogo de la función ---
    POP EBP
    RET                  ; Volvemos a C++
bubblesort ENDP          ; Cierra el bloque del procedimiento

END                      ; Fin absoluto del archivo para MASM