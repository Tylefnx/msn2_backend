CC = gcc
CFLAGS = -Wall -Wextra -Werror
LDFLAGS = -ljson-c -ljwt -lssl -lcrypto

# Dosya yollarını belirtelim
SOURCES = main.c auth/auth.c friends/friends.c messages/messages.c request_handlers.c
OBJECTS = main.o auth/auth.o friends/friends.o messages/messages.o request_handlers.o
EXECUTABLE = server

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Her bir .c dosyasını kendi .o dosyasıyla ilişkilendirelim
main.o: main.c
	$(CC) $(CFLAGS) -c main.c -o main.o

auth/auth.o: auth/auth.c
	$(CC) $(CFLAGS) -c auth/auth.c -o auth/auth.o

friends/friends.o: friends/friends.c
	$(CC) $(CFLAGS) -c friends/friends.c -o friends/friends.o

messages/messages.o: messages/messages.c
	$(CC) $(CFLAGS) -c messages/messages.c -o messages/messages.o

request_handlers.o: request_handlers.c
	$(CC) $(CFLAGS) -c request_handlers.c -o request_handlers.o

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)

.PHONY: clean
