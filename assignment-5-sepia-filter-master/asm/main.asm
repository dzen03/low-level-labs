%define O_RDONLY 0x0 
%define O_RDWR 0x2
%define O_CREAT 0x40
%define PROT_READ 0x1
%define PROT_WRITE 0x2
%define MAP_PRIVATE 0x2
%define MAP_SHARED 0x1
%define MAP_ANONYMOUS 0x20

%define SYS_READ 0
%define SYS_WRITE 1
%define SYS_OPEN 2
%define SYS_CLOSE 3
%define SYS_MMAP 9
%define SYS_MUNMAP 11
%define SYS_EXIT 60
%define SYS_FTRUNCATE 77
%define SYS_CREAT 85

%define FD_STDOUT 1

%define BIT_COUNT 24


section .rodata
	argc_error db `You have incorrect number of arguments\n\0`
	mmap_error db `Can not map file\n\0`
	open_error db `Can not open file\n\0`
	generic_error db `Error\n\0`
	filetype_error db `Thats not BMP\n\0`
	bitcount_error db `Thats not 24bit BMP\n\0`

	align 16
	red_mul dd 0.393, 0.349, 0.272, 0.0
	align 16
	green_mul dd 0.769, 0.686, 0.534, 0.0
	align 16
	blue_mul dd 0.189, 0.168, 0.131, 0.0
	align 16
	max_value dd 255, 255, 255, 0


section .data
	in_fd dq 0
	out_fd dq 0

	file_in_ptr dq 0
	image_in_ptr dq 0
	image_out_ptr dq 0

	filesize dd 0
	filetype dw 0

	width dd 0
	height dd 0

	real_width dd 0

	align 8
	image_size dd 0
	padding_size dd 0

	off_bits dd 0

section .text

global _start

_start:
	jmp main


string_length: ; rdi - pointer to string
    xor rax, rax ;  counter = 0
    .loop:
        cmp byte [rdi + rax], 0 ; if str[rdi] == null-terminator
        je .end ; goto .end

        inc rax ; else rax += 1
        jmp .loop
    .end:
    ret


print_string_to_error: ; rdi - pointer to string
    mov rsi, rdi ; string address
    call string_length ; fetch string length
    mov rdx, rax ; string length in bytes
    mov rdi, 2  ; stderr descriptor
    mov rax, 1 ; 'write' syscall number
    syscall ; write
    ret

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
    call print_string_to_error ; print string leaning on stack

    mov rsp, r8 ; restore SP
    pop rbx
    ret



main:
	cmp qword [rsp], 3
	jne .error_argc

	mov  rax, SYS_OPEN
	mov  rdi, qword [rsp+8*2]
	mov  rsi, O_RDONLY    ; Open file read only
	mov  rdx, 0
	syscall
	; rax holds the opened file descriptor now

	cmp rax, -1
	je .error_open

	mov [in_fd], rax

	mov  rax, SYS_CREAT
	mov  rdi, qword [rsp+8*3]
	mov  rsi, O_RDWR    ; Creates file for write
	syscall
	; rax holds the opened file descriptor now

	cmp rax, -1
	je .error_open

	mov [out_fd], rax

	sub rsp, 2+4 ; To read bfType + bfileSize

	mov rax, SYS_READ
	mov rdi, [in_fd]
	mov rsi, rsp
	mov rdx, 2+4

	syscall

	cmp rax, 2+4
	jne .error_generic

	pop word [filetype]

	cmp word [filetype], 0x4D42 ; TODO add const
	jne .error_filetype

	mov eax, [rsp] ; there is no pop r32, lol
	mov dword [filesize], eax
	add rsp, 4

	mov rax, SYS_MMAP
	mov rdi, 0
	movsx rsi, dword [filesize]
	mov rdx, PROT_READ
	mov r10, MAP_PRIVATE
	mov r8, [in_fd]
	mov r9, 0

	syscall

	cmp rax, -1
	je .error_mmap

	mov [file_in_ptr], rax
	mov r8, rax

	mov r8, [file_in_ptr]

	mov ax, word [r8 + 28] ; biBitCount
	cmp ax, BIT_COUNT
	jne .error_bitcount

	mov r9d, dword [r8 + 10] ; bOffBits
	mov dword [off_bits], r9d

	mov eax, dword [r8 + 22] ; biHeight
	mov dword [height], eax

	mov eax, dword [r8 + 34] ; biSizeImage
	mov dword [image_size], eax

	movsx rax, dword [r8 + 18] ; biWidth
	mov dword [width], eax

	xor rdx, rdx
	mov rcx, BIT_COUNT
	mul rcx

	mov rcx, 32

	div rcx

	test rdx, rdx
	jz .continue

	add rax, 1

	.continue:
	xor rdx, rdx
	mov rcx, 4
	mul rcx

	mov dword [real_width], eax

	mov [padding_size], eax
	mov eax, [width]
	sub [padding_size], eax
	sub [padding_size], eax
	sub [padding_size], eax


	movsx r8, dword [off_bits]


	mov rax, SYS_WRITE ; output header for 
	mov rdi, [out_fd]
	mov rsi, [file_in_ptr]
	mov rdx, r8

	syscall

	cmp rax, r8
	jne .error_generic

	mov rax, SYS_MMAP
	mov rdi, 0
	movsx rsi, dword [image_size]
	mov rdx, PROT_WRITE
	mov r10, MAP_PRIVATE | MAP_ANONYMOUS
	mov r8, -1
	mov r9, 0

	syscall

	cmp rax, -1
	je .error_mmap

	mov [image_out_ptr], rax
	


	mov rdi, [file_in_ptr]
	movsx r8, dword [off_bits]
	add rdi, r8
	mov [image_in_ptr], rdi

	call apply_sepia_with_print

	mov rax, SYS_WRITE ; output header for 
	mov rdi, [out_fd]
	mov rsi, [image_out_ptr]
	movsx rdx, dword [image_size]

	syscall



	.exit:
		mov rax, SYS_MUNMAP
		mov rdi, [file_in_ptr]
		mov rsi, [filesize]
		syscall

		mov rax, SYS_MUNMAP
		mov rdi, [image_out_ptr]
		mov rsi, [filesize]
		syscall

		mov rdi, [in_fd]
		mov rax, SYS_CLOSE
		syscall

		mov rdi, [out_fd]
		mov rax, SYS_CLOSE
		syscall
		
		xor rdi, rdi
		mov rax, SYS_EXIT ; 'exit' syscall number
		syscall ; exit(0)

	.error_bitcount:
		mov rdi, bitcount_error
		jmp .error

	.error_filetype:
		mov rdi, filetype_error
		jmp .error

	.error_generic:
		mov rdi, generic_error
		jmp .error

	.error_open:
		mov rdi, open_error
		jmp .error

	.error_mmap:
		mov rdi, mmap_error
		jmp .error

	.error_argc:
		mov rdi, argc_error

	.error:
		call print_string_to_error
		jmp .exit



apply_sepia_with_print:
	mov rdi, [image_in_ptr]
	mov rsi, [image_out_ptr]


	movaps xmm5, [red_mul]
	movaps xmm4, [green_mul]
	movaps xmm3, [blue_mul]
	movaps xmm6, [max_value]

	movsx r8, dword [height]
	movsx rax, dword [padding_size]

	align 8
	.loop_h:
		movsx r9, dword [width]
		align 8
		.loop_w:
			xor rcx, rcx
			mov cl, byte [rdi]
			pxor xmm0, xmm0
			pinsrb xmm0, ecx, 0
			pinsrb xmm0, ecx, 4
			pinsrb xmm0, ecx, 8
			cvtdq2ps xmm0, xmm0
			mulps xmm0, xmm3

			xor rcx, rcx
			mov cl, byte [rdi + 1]
			pxor xmm1, xmm1
			pinsrb xmm1, ecx, 0
			pinsrb xmm1, ecx, 4
			pinsrb xmm1, ecx, 8
			cvtdq2ps xmm1, xmm1
			mulps xmm1, xmm4
			
			xor rcx, rcx
			mov cl, byte [rdi + 2]
			pxor xmm2, xmm2
			pinsrb xmm2, ecx, 0
			pinsrb xmm2, ecx, 4
			pinsrb xmm2, ecx, 8
			cvtdq2ps xmm2, xmm2
			mulps xmm2, xmm5

			addps xmm0, xmm1
			addps xmm0, xmm2

			cvtps2dq xmm0, xmm0
			pminud xmm0, xmm6

			pextrb byte [rsi + 2], xmm0, 0
			pextrb byte [rsi + 1], xmm0, 4
			pextrb byte [rsi], xmm0, 8


			add rdi, 3
			add rsi, 3

			dec r9
			
		jnz .loop_w

		add rdi, rax
		add rsi, rax

		dec r8
	jnz .loop_h
	ret
