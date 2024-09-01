SRC=./src
OBJ=./obj
OUTPUT=main

SOURCES=$(wildcard $(SRC)/*.c)
OBJECTS=$(patsubst %.c,%.o,$(patsubst $(SRC)/%,$(OBJ)/%,$(SOURCES)))
CFLAGS=-lglfw -lm -lOpenGL
CFLAGS+=-I./include
CFLAGS+=-g -O0

.PHONY: all

all: $(OUTPUT) Makefile

$(OUTPUT): $(OBJECTS)
	gcc $(OBJ)/*.o -o $(OUTPUT) $(CFLAGS)

$(OBJ)/%.o: $(SRC)/%.c Makefile ./include/* 
	gcc -c $< -o $@ $(CFLAGS)

clean:
	rm ./main
	rm ./obj/*