#include <pthread.h>
#include "request_handlers.h"
#include "auth.h"
#include "friends.h"
#include "messages.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <json-c/json.h>
#include <jwt.h>

#define PORT 8080
#define BUFFER_SIZE 1024

typedef struct {
    int client_socket;
    struct sockaddr_in client_address;
} client_info;

typedef struct {
    char endpoint[50];
    char data[BUFFER_SIZE];
} Request;

// Token doğrulama fonksiyonu
int validate_token(const char* token, char* username) {
    jwt_t *jwt = NULL;
    if (jwt_decode(&jwt, token, (unsigned char*)"secret_key", strlen("secret_key")) != 0) {
        return 0; // Token geçersiz
    }
    const char* sub = jwt_get_grant(jwt, "sub");
    if (sub == NULL) {
        jwt_free(jwt);
        return 0; // Token geçersiz
    }
    strcpy(username, sub);
    jwt_free(jwt);
    return 1; // Token geçerli
}

// Request işleme fonksiyonu
void process_request(int client_socket) {
    char buffer[BUFFER_SIZE] = {0};
    Request req;

    // Gelen veriyi oku
    ssize_t bytes_read = read(client_socket, buffer, BUFFER_SIZE);
    if (bytes_read < 0) {
        perror("read failed");
        close(client_socket);
        return;
    }
    printf("Request received: %s\n", buffer);

    // Veriyi ayrıştır ve işle
    if (sscanf(buffer, "POST %s HTTP/1.1", req.endpoint) == 1) {
        char *json_start = strstr(buffer, "\r\n\r\n");
        if (json_start != NULL) {
            strcpy(req.data, json_start + 4);  // JSON verisini kopyala
            struct json_object *parsed_json = json_tokener_parse(req.data);
            if (!parsed_json) {
                send_json_response(client_socket, 400, "Invalid JSON format", NULL);
                close(client_socket);
                return;
            }

            printf("Processing request for endpoint: %s\n", req.endpoint);  // Debug çıkışı

            struct json_object *token_obj = NULL;
            char username[50];

            if (strcmp(req.endpoint, "/register") == 0) {
                handle_register_request(parsed_json, client_socket);
            } else if (strcmp(req.endpoint, "/login") == 0) {
                handle_login_request(parsed_json, client_socket);
            } else {
                if (!json_object_object_get_ex(parsed_json, "token", &token_obj) ||
                    !validate_token(json_object_get_string(token_obj), username)) {
                    send_json_response(client_socket, 401, "Invalid or missing token", NULL);
                    json_object_put(parsed_json);  // JSON nesnesini serbest bırak
                    close(client_socket);
                    return;
                }
                if (strcmp(req.endpoint, "/add_friend") == 0) {
                    handle_add_friend_request(parsed_json, client_socket);
                } else if (strcmp(req.endpoint, "/remove_friend") == 0) {
                    handle_remove_friend_request(parsed_json, client_socket);
                } else if (strcmp(req.endpoint, "/send_message") == 0) {
                    handle_send_message_request(parsed_json, client_socket);
                } else if (strcmp(req.endpoint, "/list_friends") == 0) {
                    handle_list_friends_request(parsed_json, client_socket);
                } else {
                    send_json_response(client_socket, 400, "Unknown endpoint", NULL);
                }
            }

            json_object_put(parsed_json);  // JSON nesnesini serbest bırak
        } else {
            send_json_response(client_socket, 400, "Invalid request format", NULL);
        }
    } else {
        send_json_response(client_socket, 400, "Invalid request format", NULL);
    }

    // Bağlantıyı kapat
    close(client_socket);
}

void* handle_client(void* arg) {
    client_info* client = (client_info*)arg;
    process_request(client->client_socket);
    free(client);  // Dinamik olarak ayrılan client_info'yu serbest bırak
    pthread_exit(NULL);
}

int main() {
    int server_fd;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    // Kullanıcı verilerini, arkadaş listesini ve mesajları dosyadan yükle
    load_users_from_file();
    print_users();
    load_friends_from_file();
    load_messages_from_file();

    // Soket oluştur
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Adres ve portu ayarla
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Soket ve adresi bağla
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Dinlemeye başla
    if (listen(server_fd, 3) < 0) {
        perror("listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Server is running on port %d...\n", PORT);

    while (1) {
        client_info* client = malloc(sizeof(client_info));
        if ((client->client_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            perror("accept failed");
            free(client);
            continue;
        }
        pthread_t client_thread;
        pthread_create(&client_thread, NULL, handle_client, (void*)client);
        pthread_detach(client_thread);  // İş parçacığını ayrıştır
    }

    return 0;
}
