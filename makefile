CC = gcc
CFLAGS = -Wall -Wextra -Werror
LDFLAGS = -ljson-c -ljwt
SOURCES = main.c auth.c friends.c messages.c request_handlers.c
OBJECTS = $(SOURCES:.c=.o)
EXECUTABLE = server

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)

.PHONY: clean
