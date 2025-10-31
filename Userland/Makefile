include Makefile.inc

MODULE=0000-sampleCodeModule.bin
SAMPLE_DATA=0001-sampleDataModule.bin
SOURCES=$(wildcard *.c)
SOURCES_SHELL=$(wildcard Shell/*.c)
SOURCES_USERLIB=$(wildcard Userlib/*.c)
SOURCES_TRON=$(wildcard Tron/*.c)
SOURCES_ASM=$(wildcard Asm/*.asm)
OBJECTS=$(SOURCES:.c=.o)
OBJECTS_SHELL=$(SOURCES_SHELL:.c=.o)
OBJECTS_USERLIB=$(SOURCES_USERLIB:.c=.o)
OBJECTS_TRON=$(SOURCES_TRON:.c=.o)
OBJECTS_ASM=$(SOURCES_ASM:.asm=.o)

all: $(MODULE) sampleDataModule

$(MODULE): $(OBJECTS) $(OBJECTS_SHELL) $(OBJECTS_USERLIB) $(OBJECTS_TRON) $(OBJECTS_ASM)
	$(LD) $(LDFLAGS) -T sampleCodeModule.ld -o $(MODULE) $(OBJECTS) $(OBJECTS_SHELL) $(OBJECTS_USERLIB) $(OBJECTS_TRON) $(OBJECTS_ASM)

%.o: %.c
	$(GCC) $(GCCFLAGS) -I. -IShell -IUserlib -ITron -c $< -o $@

%.o: %.asm
	$(ASM) $(ASMFLAGS) $< -o $@

clean:
	rm -rf Asm/*.o Shell/*.o Userlib/*.o Tron/*.o *.o $(MODULE) $(SAMPLE_DATA)

sampleDataModule:
	printf "This is sample data." >> $(SAMPLE_DATA) && dd if=/dev/zero bs=1 count=1 >> $(SAMPLE_DATA)

.PHONY: all clean sampleDataModule