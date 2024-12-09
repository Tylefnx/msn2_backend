CC = gcc
CFLAGS = -Wall -Wextra -Werror
LDFLAGS = -ljson-c -ljwt -lssl -lcrypto

# Dosya yollarını belirtelim
SOURCES = main.c auth/auth.c auth/auth_request_handler.c friends/friends.c friends/friends_request_handler.c messages/messages.c messages/messages_request_handler.c srv/process_request.c srv/handle_client.c srv/send_json_response.c srv/validate_token.c
OBJECTS = main.o auth/auth.o auth/auth_request_handler.o friends/friends.o friends/friends_request_handler.o messages/messages.o messages/messages_request_handler.o srv/process_request.o srv/handle_client.o srv/send_json_response.o srv/validate_token.o
EXECUTABLE = server

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Her bir .c dosyasını kendi .o dosyasıyla ilişkilendirelim
main.o: main.c
	$(CC) $(CFLAGS) -c main.c -o main.o

auth/auth.o: auth/auth.c
	$(CC) $(CFLAGS) -c auth/auth.c -o auth/auth.o

auth/auth_request_handler.o: auth/auth_request_handler.c
	$(CC) $(CFLAGS) -c auth/auth_request_handler.c -o auth/auth_request_handler.o

friends/friends.o: friends/friends.c
	$(CC) $(CFLAGS) -c friends/friends.c -o friends/friends.o

friends/friends_request_handler.o: friends/friends_request_handler.c
	$(CC) $(CFLAGS) -c friends/friends_request_handler.c -o friends/friends_request_handler.o

messages/messages_request_handler.o: messages/messages_request_handler.c
	$(CC) $(CFLAGS) -c messages/messages_request_handler.c -o messages/messages_request_handler.o

messages/messages.o: messages/messages.c
	$(CC) $(CFLAGS) -c messages/messages.c -o messages/messages.o

srv/process_request.o: srv/process_request.c
	$(CC) $(CFLAGS) -c srv/process_request.c -o srv/process_request.o

srv/send_json_response.o: srv/send_json_response.c
	$(CC) $(CFLAGS) -c srv/send_json_response.c -o srv/send_json_response.o

srv/validate_token.o: srv/validate_token.c
	$(CC) $(CFLAGS) -c srv/validate_token.c -o srv/validate_token.o

src/handle_client.o: srv/handle_client.c
	$(CC) $(CFLAGS) -c srv/handle_client.c -o srv/handle_client.o

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)

.PHONY: clean
