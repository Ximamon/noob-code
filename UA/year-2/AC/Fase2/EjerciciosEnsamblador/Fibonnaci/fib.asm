.586
.model flat, c
.code

; Función: fibonacci_asm
; Recibe:
;   - fib: Puntero al array (DWORD / 32 bits)
;   - n_actual: Número de términos a calcular
;   - inicio: Valor para fib[1]
fibonacci_asm PROC fib:PTR DWORD, n_actual:DWORD, inicio:DWORD
    
    ; Guardar los registros que vamos a modificar (convención de llamadas)
    push esi
    push ebx

    ; Cargar parámetros
    mov esi, fib        ; ESI = puntero al array
    mov ecx, n_actual   ; ECX = N
    mov ebx, inicio     ; EBX = valor de fib[1]
    mov eax, 0          ; EAX = valor de fib[0]

    ; Inicializar memoria (fib[0] y fib[1])
    mov [esi], eax
    mov [esi + 4], ebx

    ; Preparar el bucle
    sub ecx, 2          ; Restamos 2 términos ya calculados
    add esi, 8          ; Avanzamos el puntero a fib[2]

fib_loop:
    ; EDX = EAX + EBX
    mov edx, ebx
    add edx, eax

    ; Guardar resultado en memoria
    mov [esi], edx
    add esi, 4

    ; Deslizar ventana
    mov eax, ebx
    mov ebx, edx

    ; En lugar de usar LOOP, usamos DEC y JNZ (más rápido en CPUs modernas)
    dec ecx
    jnz fib_loop

    ; Restaurar registros
    pop ebx
    pop esi

    ret
fibonacci_asm ENDP

END