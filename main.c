#include "request_handlers.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <json-c/json.h>

#define PORT 8080
#define BUFFER_SIZE 1024

// Basit bir request yapısı
typedef struct {
    char endpoint[50];
    char data[BUFFER_SIZE];
} Request;

// Request işleme fonksiyonu
void process_request(Request req, int client_socket) {
    struct json_object *parsed_json = NULL;

    printf("Processing request for endpoint: %s\n", req.endpoint);  // Debug çıkışı

    parsed_json = json_tokener_parse(req.data);
    if (!parsed_json) {
        send_json_response(client_socket, 400, "Invalid JSON format", NULL);
        return;
    }

    printf("Parsed JSON data: %s\n", json_object_to_json_string(parsed_json));  // Debug çıkışı
    
    if (strcmp(req.endpoint, "/register") == 0) {
        handle_register_request(parsed_json, client_socket);
    } else if (strcmp(req.endpoint, "/login") == 0) {
        handle_login_request(parsed_json, client_socket);
    } else if (strcmp(req.endpoint, "/add_friend") == 0) {
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

    json_object_put(parsed_json);  // JSON nesnesini serbest bırak
    printf("Request processing completed for endpoint: %s\n", req.endpoint);  // Debug çıkışı
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};
    Request req;

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
        // Yeni bağlantıları kabul et
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            perror("accept failed");
            continue;
        }

        // Gelen veriyi oku
        ssize_t bytes_read = read(new_socket, buffer, BUFFER_SIZE);
        if (bytes_read < 0) {
            perror("read failed");
            close(new_socket);
            continue;
        }
        printf("Request received: %s\n", buffer);

        // Veriyi ayrıştır ve işle
        if (sscanf(buffer, "POST %s HTTP/1.1", req.endpoint) == 1) {
            char *json_start = strstr(buffer, "\r\n\r\n");
            if (json_start != NULL) {
                strcpy(req.data, json_start + 4);  // JSON verisini kopyala
                process_request(req, new_socket);
            } else {
                send_json_response(new_socket, 400, "Invalid request format", NULL);
            }
        } else {
            send_json_response(new_socket, 400, "Invalid request format", NULL);
        }

        // Bağlantıyı kapat
        close(new_socket);
    }

    return 0;
}
