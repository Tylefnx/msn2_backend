CC = gcc
CFLAGS = -Wall -Wextra -std=c11
TARGET = program
SRC = main.c auth.c

all:
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

clean:
	rm -f $(TARGET)