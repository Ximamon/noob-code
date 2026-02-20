.386
.model flat, c      ; El ', c' al final es MAGIA. Le dice a MASM que ponga el '_' automáticamente.
.code

PUBLIC sumar        ; C++ lo buscará como _sumar, pero gracias al ', c' de arriba, esto coincidirá.

sumar PROC
    PUSH EBP
    MOV EBP, ESP

    ; Lógica de la suma
    MOV EAX, [EBP + 8]   ; Primer parámetro (a)
    ADD EAX, [EBP + 12]  ; Segundo parámetro (b)

    POP EBP
    RET
sumar ENDP

END