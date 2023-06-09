ASM=nasm
ASM_FLAGS=-F dwarf -g -f elf64
LD=ld
LD_FLAGS=-m elf_x86_64

.PHONY: all
all: program

main.asm:
	touch $@

%.o: %.asm
	$(ASM) $(ASM_FLAGS) -o $@ $<

program: main.o
	$(LD) $(LD_FLAGS) -o $@ $^

.PHONY: clean
clean:
	rm *o program