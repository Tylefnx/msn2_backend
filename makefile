CC = gcc
CFLAGS = -Wall -I./include

# Derleme hedefleri
OBJ = src/auth.o src/messages.o src/server.o
TARGET = server

# Derleme komutları
$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET) -lmicrohttpd

src/auth.o: src/auth.c
	$(CC) $(CFLAGS) -c src/auth.c -o src/auth.o

src/messages.o: src/messages.c
	$(CC) $(CFLAGS) -c src/messages.c -o src/messages.o

src/server.o: src/server.c
	$(CC) $(CFLAGS) -c src/server.c -o src/server.o

clean:
	rm -f src/*.o $(TARGET)
