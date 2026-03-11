COMPILER = gcc
C_FILES = src/*.c
INCLUDES=-Iinclude
FLAGS=-O1 -Wall -std=c99 -Wno-missing-braces
LIBS=-Llib -lglfw3 -lgdi32
OUTPUT_FOLDER = bin/app

build:
	$(COMPILER) $(C_FILES) $(INCLUDES) $(LIBS) -o $(OUTPUT_FOLDER) $(FLAGS)

