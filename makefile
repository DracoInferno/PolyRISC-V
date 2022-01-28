# Autopopulating makefile

CC= gcc
EXEC= riscvcpu
ELF= elfriscv
RAW= rawriscv

SRCDIR= src
OBJDIR= obj
INCDIR= include
BINDIR= bin
LIBDIR= lib

WARNINGS= -W -Wall -Wextra -Wpedantic -Wdouble-promotion -Wstrict-prototypes -Wshadow
CFLAGS= $(WARNINGS) -std=c11 -MMD -MP -march=native -O2
LDFLAGS= #-L ./$(LIBDIR) -Wl,-rpath='$$ORIGIN' #rpath tells where to find .so files to the binaru output
LIBFLAGS= 
INCFLAGS= -I ./$(INCDIR)

ASFLAGS= -march=rv32i

SRC= $(wildcard ./$(SRCDIR)/*.c)
OBJ= $(subst $(SRCDIR),$(OBJDIR),$(SRC:.c=.o))
OBJ_D= $(subst $(SRCDIR),$(OBJDIR),$(SRC:.c=_d.o))
DEP= $(OBJ:.o=.d)

############################### C ##################################

# Standard Compile
all: $(BINDIR)/$(EXEC)
	@echo "Standard Compile"

$(BINDIR)/$(EXEC): $(OBJ)
	@mkdir -p ./$(BINDIR)
	$(CC) -o $@ $(LDFLAGS) $^ $(LIBFLAGS)

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p ./$(OBJDIR)
	$(CC) -o $@ -c $< $(CFLAGS) $(INCFLAGS)

# Debug Compile
debug: $(EXEC)_d
	@echo "Debug Compile"

$(BINDIR)/$(EXEC)_d: $(OBJ_D)
	@mkdir -p ./$(BINDIR)
	@$(CC) -o $@ $(LDFLAGS) $^ $(LIBFLAGS)

$(OBJDIR)/%_d.o: $(SRCDIR)/%.c
	@mkdir -p ./$(OBJDIR)
	@$(CC) -o $@ -c $< $(CFLAGS) $(INCFLAGS) -g

############################## ASM ##################################

elf: $(BINDIR)/$(ELF)
	@echo "Assembling riscv instructions"

$(BINDIR)/$(ELF): $(SRCDIR)/main.s
	@mkdir -p ./$(BINDIR)
	riscv32-elf-as $(ASFLAGS) $^ -o $@

elfdump: $(BINDIR)/$(ELF)
	hexdump $^

raw: $(BINDIR)/$(RAW)
	@echo "Getting raw riscv instructions"

$(BINDIR)/$(RAW): $(BINDIR)/$(ELF)
	@mkdir -p ./$(BINDIR)
	riscv32-elf-objcopy -O binary $^ $@

rawdump: $(BINDIR)/$(RAW)
	hexdump $^


# Cleaning

.PHONY: clean mrproper

clean:
	@echo "Removing obj files."
	@rm -rf ./$(OBJDIR)/*.o
	@rm -rf ./$(OBJDIR)/*.d

mrproper: clean
	@echo "Removing binaries."
	@rm -rf ./$(BINDIR)/$(EXEC)
	@rm -rf ./$(BINDIR)/$(EXEC)_d

# Take into account header files modifications
-include $(DEP)
