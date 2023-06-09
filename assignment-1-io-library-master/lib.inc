section .data

section .text

%define CHAR_NEWLINE 0xA
%define CHAR_SPACE 0x20
%define CHAR_TAB 0x9
%define CHAR_CTRL_D 0x4


; Принимает код возврата и завершает текущий процесс
exit: ; rdi - exit code
    mov rax, 60 ; 'exit' syscall number
    syscall ; exit(rdi)
    ret 


; Принимает указатель на нуль-терминированную строку, возвращает её длину
string_length: ; rdi - pointer to string
    xor rax, rax ;  counter = 0
    .loop:
        cmp byte [rdi + rax], 0 ; if str[rdi] == null-terminator
        je .end ; goto .end

        inc rax ; else rax += 1
        jmp .loop
    .end:
    ret


; Принимает указатель на нуль-терминированную строку, выводит её в stdout
print_string: ; rdi - pointer to string
    mov rsi, rdi ; string address
    call string_length ; fetch string length
    mov rdx, rax ; string length in bytes
    mov rdi, 1  ; stdout descriptor
    mov rax, 1 ; 'write' syscall number
    syscall ; write
    ret


; Принимает код символа и выводит его в stdout
print_char: ; rdi - symbol code
    push rdi
    mov rsi, rsp ; string address
    mov rdx, 1 ; string length in bytes
    mov rdi, 1  ; stdout descriptor
    mov rax, 1 ; 'write' syscall number
    syscall ; write

    pop rdi

    ret


; Переводит строку (выводит символ с кодом 0xA)
print_newline:
    mov rdi, CHAR_NEWLINE
    call print_char
    ret


; Выводит беззнаковое 8-байтовое число в десятичном формате 
; Совет: выделите место в стеке и храните там результаты деления
; Не забудьте перевести цифры в их ASCII коды.
print_uint: ; rdi - uint
    push rbx
    mov r8, rsp ; save SP, we will override it for string saving

    push 0 ; end of str - null-terminator
    mov rax, rdi
    mov rcx, 0 ; len of dec int
    mov rbx, 10
    .loop:
        xor rdx, rdx
        div rbx ; get digit
        add rdx, '0' ; to ascii

        dec rsp
        mov [rsp], dl ; save digit

        inc rcx ; len += 1
        
        test rax, rax
        jnz .loop ; while rax != 0
    .end:

    mov rdi, rsp
    call print_string ; print string leaning on stack

    mov rsp, r8 ; restore SP
    pop rbx
    ret


; Выводит знаковое 8-байтовое число в десятичном формате 
print_int: ; rdi - int

    push rdi
    test rdi, rdi
    jge .positive ; than number is negative
    .negative:
        mov rdi, '-'

        push rdi
        call print_char ; print '-'
        pop rdi
        
        pop rdi

        neg rdi ; rdi = -rdi
        push rdi

    .positive:
        pop rdi
        call print_uint ; print number as positive
        
    ret


; Принимает два указателя на нуль-терминированные строки, возвращает 1 если они равны, 0 иначе
string_equals: ; rdi, rsi - pointers to strings
    push rbx
    xor rax, rax ;  counter = 0
    .loop:
        movsx rbx, byte [rsi + rax]
        movsx rcx, byte [rdi + rax]
        cmp bl , cl ; if rdi[rax] != rsi[rax] (even if one string is less than other)
        jne .not_equal ; goto .not_equal

        test bl, bl
        je .equal ; if both ended then equal

        inc rax ; else rax += 1
        jmp .loop

    .equal:
        mov rax, 1
        pop rbx
        ret
    
    .not_equal:
        xor rax, rax
        pop rbx
        ret
    ret


; Читает один символ из stdin и возвращает его. Возвращает 0 если достигнут конец потока
read_char:
    push rax
    mov rsi, rsp ; string address
    mov rdx, 1 ; string length in bytes
    mov rdi, 0  ; stdin descriptor
    mov rax, 0 ; 'read' syscall number
    syscall ; read

    test rax, rax
    pop rax

    je .end_of_stream ; if we read 0 symbols

    jmp .return

    .end_of_stream:
        xor rax, rax

    .return:
    ret 


; Принимает: адрес начала буфера, размер буфера
; Читает в буфер слово из stdin, пропуская пробельные символы в начале, .
; Пробельные символы это пробел 0x20, табуляция 0x9 и перевод строки 0xA.
; Останавливается и возвращает 0 если слово слишком большое для буфера
; При успехе возвращает адрес буфера в rax, длину слова в rdx.
; При неудаче возвращает 0 в rax
; Эта функция должна дописывать к слову нуль-терминатор
read_word: ; rdi - pointer to buffer, rsi - buffer size
    dec rsi ; leave space for null-terminator
    mov rcx, rsi

    xor rdx, rdx ;  counter = 0

    .loop:
        push rdi
        push rdx
        push rcx
        call read_char
        pop rcx
        pop rdx
        pop rdi


        test al, al
        je .end

        cmp al, CHAR_SPACE ; if read char is space
        je .decider
        cmp al, CHAR_TAB ; if read char is tab
        je .decider
        cmp al, CHAR_NEWLINE ; if read char is '\n'
        je .decider
        cmp al, CHAR_CTRL_D ; if read char is 'CTRL + D'
        je .end

        mov byte [rdi + rdx], al
        
        inc rdx
        loop .loop
    jmp .error

    .decider:
        test rdx, rdx
        je .loop
        jmp .end


    .end:

    mov byte[rdi + rdx], 0 ; append to the string null-terminator
    mov rax, rdi

    jmp .return

    .error:
        xor rax, rax

    .return:
    ret
    
 
; Принимает указатель на строку, пытается
; прочитать из её начала беззнаковое число.
; Возвращает в rax: число, rdx : его длину в символах
; rdx = 0 если число прочитать не удалось
parse_uint: ; rdi - pointer to string
    push rbx

    xor rcx, rcx
    xor rbx, rbx ; number = 0
    xor r11, r11 ;  counter = 0
    xor r8, r8
    mov r9, 10
    mov r10, 1

    .loop_to_right:
        movsx rax, byte [rdi + rcx]

        cmp al, '0' ; if str[rdi] < '0'
        jb .end ; goto .end
        cmp al, '9' ; if str[rdi] > '9'
        ja .end ; goto .end
        push rax
        inc rcx
        jmp .loop_to_right
    .end:

    test rcx, rcx
    je .end_all

    .loop_to_left:
        pop rax
        
        sub rax, '0' ; get digit
        mul r10 ; digit * 10**r11

        add r8, rax ; sum digits

        mov rax, r10
        mul r9 ; get 10**r11 * 10
        mov r10, rax

        inc r11
        loop .loop_to_left
    
    .end_all:

    mov rax, r8
    mov rdx, r11

    pop rbx
    ret


; Принимает указатель на строку, пытается
; прочитать из её начала знаковое число.
; Если есть знак, пробелы между ним и числом не разрешены.
; Возвращает в rax: число, rdx : его длину в символах (включая знак, если он был) 
; rdx = 0 если число прочитать не удалось
parse_int:
    movsx rax, byte [rdi]
    cmp rax, '-'
    jne .positive
    .negative:
        inc rdi

        call parse_uint ; if negative we parse like positive

        test rdx, rdx ; and than multiply to -1
        je .return
        neg rax
        inc rdx
        jmp .return
    .positive:
        call parse_uint
    .return:
    ret 

; Принимает указатель на строку, указатель на буфер и длину буфера
; Копирует строку в буфер
; Возвращает длину строки если она умещается в буфер, иначе 0
string_copy: ; rdi - pointer to string to copy from, rsi - pointer to buffer to copy to, rdx - buffer size
    push rsi
    mov rsi, rdi
    
    call string_length ; rax = len
    inc rax

    pop rdi

    cmp rax, rdx
    ja .error ; can't fit in buffer

    cld ; make left to right
    mov rcx, rax
    rep movsb ; copy bytes from rsi to rdx times rcx

    jmp .success

    .error:
        xor rax, rax

    .success:
        ret
