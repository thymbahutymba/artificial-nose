CC = gcc
BIN = main

SRC_DIR = src
OBJ_DIR = obj
INC_DIR = include

SRC = $(wildcard $(SRC_DIR)/*.c)
OBJ = $(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

LDFLAGS = -pthread -lrt -ltensorflow -lalleg
CFLAGS = -Wall -Wextra -I$(INC_DIR)

all: $(BIN)

$(BIN): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(OBJ_DIR)/%.o : $(SRC_DIR)/%.c
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm $(OBJ_DIR)/*.o
	rm main