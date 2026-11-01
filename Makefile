CC = gcc
CFLAGS = -Wall -Wextra -Iinclude
LDFLAGS = -lreadline

SRC = src/main.c src/shell.c src/execute.c
TARGET = bin/psh

all: $(TARGET)

$(TARGET): $(SRC)
	mkdir -p bin
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS)

clean:
	rm -rf bin
	rm -f obj/*.o
