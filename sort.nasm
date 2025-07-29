global bubble_sort_uint64
global shaker_sort_uint64
global insertion_sort_uint64
global selection_sort_uint64
global gnome_sort_uint64

section .text

; -------------------------------------------------------------
; rdi - addr
; rsi - len
align 16
times -(bubble_sort_uint64.outer - bubble_sort_uint64) % 16 nop
bubble_sort_uint64:
    cmp rsi, 1
    jbe .end

    mov edx, 8
    lea rsi, [rdi + rsi * 8]    ; ptr to last element + 1

    .outer:
        lea r9, [rdi + rdx] ; ptr to 2nd element
        mov r8, rdi         ; ptr to 1st element
        xor r10, r10        ; count of swaps

        cmp r9, rsi
        jae .end

        .inner:
            mov rax, [r8]
            cmp rax, [r9]
            jbe .next

            xchg rax, [r9]
            mov [r8], rax
            inc r10

        .next:
            add r9, rdx
            add r8, rdx
            cmp r9, rsi
            jb .inner

        test r10, r10   ; no swaps occured, array is sorted
        jz .end

        sub rsi, 8
        jmp .outer

.end:
    ret

; -------------------------------------------------------------
; rdi - addr
; rsi - len
align 16
times -(shaker_sort_uint64.outer - shaker_sort_uint64) % 32 nop
shaker_sort_uint64:
    cmp rsi, 1
    jbe .end

    mov rdx, 8                  ; ptr direction (8, -8)

    lea rsi, [rdi + rsi * 8]    ; upper bound (const)
    lea rdi, [rdi - 8]          ; lower bound (const)

    mov r11, rsi                ; upper bound (mutable)
    mov r10, rdi                ; lower bound (mutable)

    .outer:
        lea r8, [r10 + rdx]     ; 1st elem addr
        lea r9, [r8  + rdx]     ; 2nd elem addr

        cmp r8, rdi
        je .end
        cmp r9, rsi
        je .end

        xor rcx, rcx            ; new upper/lower bound

        cmp r8, r9
        jb .inner

        xchg r8, r9

        .inner:
            mov rax, [r8]
            cmp rax, [r9]
            jbe .next

            xchg rax, [r9]
            mov [r8], rax

            mov rcx, r9
            cmp rdx, -8
            cmove rcx, r8

        .next:
            add r8, rdx
            add r9, rdx

            cmp r8, rdi
            je .switch_direction
            cmp r9, rsi
            je .switch_direction
            jmp .inner

    .switch_direction:
        test rcx, rcx
        jz .end

        cmp    rdx, 8
        cmove  r11, rcx
        cmovne r10, rcx

        neg rdx
        xchg r10, r11
        jmp .outer

.end:
    ret

; -------------------------------------------------------------
; rdi - addr
; rsi - len
align 16
times -(insertion_sort_uint64.select - insertion_sort_uint64) % 16 nop
insertion_sort_uint64:
    cmp rsi, 1
    jbe .end

    dec rsi         ; use rsi as last index

    mov r8, 1       ; select index

    .select:
        mov rax, [rdi + r8 * 8] ; inserted value

        mov r10, r8             ; prev prev element index
        mov r9,  r8             ; prev element index
        dec r10

        .insert:
            cmp r9, 0
            je .next

            mov rdx, [rdi + r10 * 8]

            cmp rdx, rax
            jbe .next

            xchg rdx, [rdi + r9 * 8]
            mov [rdi + r10 * 8], rdx
 
            dec r10
            dec r9
            jnz .insert

    .next:
        mov [rdi + r9 * 8], rax
        inc r8
        cmp r8, rsi
        jbe .select

.end:
    ret

; -------------------------------------------------------------
; rdi - addr
; rsi - len
times -(selection_sort_uint64.outer - selection_sort_uint64) % 16 nop
selection_sort_uint64:
    cmp rsi, 1
    jbe .end

    mov rdx, 8                  ; data size
    lea rcx, [rdi + rsi * 8]    ; last element
    sub rcx, rdx

    .outer:
        cmp rdi, rcx
        je .end

        mov rax, [rdi]          ; min value
        mov r8,  rdi            ; min value ptr
        lea r9,  [rdi + rdx]    ; next value ptr

        .select:
            cmp rax, [r9]
            jbe .select_next

            mov rax, [r9]
            mov r8,  r9

        .select_next:
            add r9, rdx
            cmp r9, rcx
            jbe .select

        cmp r8, rdi
        je .next

        xchg rax, [rdi]
        mov [r8], rax

    .next:
        add rdi, rdx
        jmp .outer

.end:
    ret

; -------------------------------------------------------------
; rdi - addr
; rsi - len
times -(gnome_sort_uint64.loop - gnome_sort_uint64) % 16 nop
gnome_sort_uint64:
    cmp rsi, 1
    jbe .end

    xor r8,  r8
    mov r9,  1
    mov r10, 2

    .loop:
        cmp r9, rsi
        jae .end

        mov rax, [rdi + r8 * 8]
        cmp rax, [rdi + r9 * 8]
        jbe .next

        xchg rax, [rdi + r9 * 8]
        mov [rdi + r8 * 8], rax
        dec r8
        dec r9
        jnz .loop

    .next:
        mov r9, r10
        mov r8, r10
        dec r8
        inc r10
        jmp .loop

.end:
    ret

