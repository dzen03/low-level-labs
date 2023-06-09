ASM=nasm
ASM_FLAGS=-F dwarf -g -f elf64
LD=ld
LD_FLAGS=-m elf_x86_64

.PHONY: all
all: program

main.asm: lib.inc colon.inc words.inc
	touch $@

%.o: %.asm
	$(ASM) $(ASM_FLAGS) -o $@ $<

program: lib.o main.o dict.o
	$(LD)  $(LD_FLAGS) -o $@ $^

.PHONY: clean
clean:
	rm *o program