#CC=gcc
#WARNINGS= -W -Wall -Wextra -Wpedantic -Wdouble-promotion -Wstrict-prototypes -Wshadow
#CFLAGS= $(WARNINGS) -std=c11
#
#SRCDIR= ./src
#SRC= PolyRISC-V.c main.c
#INCDIR= ./include
#
#all: 
#
#riscv: $(INCDIR)/$(SRC)
#	$(CC) -o $@ $^ $(CFLAGS) -I $(INCDIR)
#
#.PHONY: clean mrproper
#
#clean:
#	@rm -rf *.o

# Autopopulating makefile

CC= gcc
EXEC= riscv

SRCDIR= src
OBJDIR= obj
INCDIR= include
BINDIR= bin
LIBDIR= lib

WARNINGS= -W -Wall -Wextra -Wpedantic -Wdouble-promotion -Wstrict-prototypes -Wshadow
CFLAGS= $(WARNINGS) -std=c99 -MMD -MP -march=native
LDFLAGS= #-L ./$(LIBDIR) -Wl,-rpath='$$ORIGIN' #rpath tells where to find .so files to the binaru output
LIBFLAGS= 
INCFLAGS= -I ./$(INCDIR)

SRC= $(wildcard ./$(SRCDIR)/*.c)
OBJ= $(subst $(SRCDIR),$(OBJDIR),$(SRC:.c=.o))
OBJ_D= $(subst $(SRCDIR),$(OBJDIR),$(SRC:.c=_d.o))
DEP= $(OBJ:.o=.d)

# Standard Compile
all: $(EXEC)
	@echo "Standard Compile"

$(EXEC): $(OBJ)
	@mkdir -p ./$(BINDIR)
	$(CC) -o ./$(BINDIR)/$@ $(LDFLAGS) $^ $(LIBFLAGS)

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p ./$(OBJDIR)
	$(CC) -o $@ -c $< $(CFLAGS) $(INCFLAGS)

# Debug Compile
debug: $(EXEC)_d
	@echo "Debug Compile"

$(EXEC)_d: $(OBJ_D)
	@mkdir -p ./$(BINDIR)
	@$(CC) -o ./$(BINDIR)/$@ $(LDFLAGS) $^ $(LIBFLAGS)

$(OBJDIR)/%_d.o: $(SRCDIR)/%.c
	@mkdir -p ./$(OBJDIR)
	@$(CC) -o $@ -c $< $(CFLAGS) $(INCFLAGS) -g

# Cleaning

.PHONY: clean mrproper

clean:
	@echo "Removing obj files."
	@rm -rf ./$(OBJDIR)/*.o
	@rm -rf ./$(OBJDIR)/*.d

mrproper: clean
	@echo "Removing binary."
	@rm -rf ./$(BINDIR)/$(EXEC)
	@rm -rf ./$(BINDIR)/$(EXEC)_d

# Take into account header files modifications
-include $(DEP)
