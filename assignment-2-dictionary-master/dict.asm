%include "lib.inc"

%define KEY_OFFSET 16

global find_word
find_word: ;rdi - pointer to string, rsi - pointer to dict
	.loop:
		push rdi
		push rsi
		add rsi, KEY_OFFSET
		call string_equals
		pop rsi
		pop rdi
		
		test rax, rax
		jnz .equal

		mov rsi, [rsi]
		test rsi, rsi
		jnz .loop

	.not_equal:
		mov rax, 0
		ret

	.equal:
		mov rax, rsi
		ret
