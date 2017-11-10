# Compilador
CC = gcc

# Arquivo principal
OBJ_FILE = main.c

# Arquivos auxiliares
AUX_FILES = file.c

# Arquivo de saída
OUT_FILE = ./registermanager

all:
	$(CC) $(OBJ_FILE) $(AUX_FILES) -o $(OUT_FILE)
