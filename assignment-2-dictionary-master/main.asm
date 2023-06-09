%include "lib.inc"
%include "words.inc"
%include "dict.inc"

%define BUFFER_LENGTH 256
%define KEY_OFFSET 16
%define POINTER_TO_VALUE_OFFSET 8

section .bss
buff: resb BUFFER_LENGTH

section .rodata
overflow_error: db `Your input string is too long\n\0`
key_not_found_error: db `Key was not found\n\0`

section .text

global _start

_start:
    jmp main

main:
	mov rdi, buff
	mov rsi, BUFFER_LENGTH
	call read_word

	test rax, rax
	jz .overflow

	mov rdi, buff
	mov rsi, POINTER_TO_NEXT
	call find_word

	test rax, rax
	jz .not_found
	
	mov rdi, rax
	add rdi, POINTER_TO_VALUE_OFFSET
	mov rdi, [rdi]
	call print_string

	call print_newline

	.exit:
		call exit
	
	.overflow:
		mov rdi, overflow_error
		jmp .error
	
	.not_found:
		mov rdi, key_not_found_error

	.error:
		call print_string_to_error
		jmp .exit