# Makefile for CS 40 Homework 1

# Executables to be built using "make all"
EXECUTABLES = 40image

# List all your header files here (if you have any)
INCLUDES = reader.h transforms.h quan.h 

# Compiler
CC = gcc

# Comp 40 and Hanson directories
COMP40 = /comp/40
HANSON = /usr/sup/cii40

# Include paths
IFLAGS = -I. -I$(COMP40)/build/include -I$(HANSON)/include/cii 

# Compiler flags
CFLAGS = -g -std=c99 -Wall -Wextra -Werror -Wfatal-errors -pedantic $(IFLAGS)

# Linker flags
LDFLAGS = -g -L$(COMP40)/build/lib -L$(HANSON)/lib64 

# Libraries needed for linking
LDLIBS = -lpnmrdr -lcii40 -lm -larith40

# Default target: build all executables
all: $(EXECUTABLES)

# Clean compiled files
clean:
	rm -f $(EXECUTABLES) *.o

# Compile .c files into .o files
%.o: %.c $(INCLUDES)
	$(CC) $(CFLAGS) -c $< -o $@

# Linking rule for 40image
40image: 40image.o compress40.o reader.o transforms.o quan.o bitpack.o
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)
