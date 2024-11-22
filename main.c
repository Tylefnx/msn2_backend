#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "auth.c" // auth.c'yi dahil ediyor

#define PORT 8080
#define BUFFER_SIZE 1024

void handle_request(int client_socket) {
    char buffer[BUFFER_SIZE];
    int read_size = read(client_socket, buffer, BUFFER_SIZE - 1);

    if (read_size <= 0) {
        close(client_socket);
        return;
    }

    buffer[read_size] = '\0';
    printf("Gelen İstek:\n%s\n", buffer);

    // HTTP metodunu ve veriyi çözümle
    char method[8], path[256], body[BUFFER_SIZE];
    sscanf(buffer, "%s %s", method, path);
    char *body_start = strstr(buffer, "\r\n\r\n");
    if (body_start) {
        strcpy(body, body_start + 4);
    }

    // Yanıtı hazırlama
    char response[BUFFER_SIZE];
    if (strcmp(path, "/register") == 0 && strcmp(method, "POST") == 0) {
        char username[128], password[128];
        sscanf(body, "username=%127[^&]&password=%127s", username, password);

        if (register_user(username, password)) {
            snprintf(response, sizeof(response),
                     "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n\r\n"
                     "{\"status\": \"success\", \"message\": \"Registration successful\"}");
        } else {
            snprintf(response, sizeof(response),
                     "HTTP/1.1 500 Internal Server Error\r\nContent-Type: application/json\r\n\r\n"
                     "{\"status\": \"error\", \"message\": \"Registration failed\"}");
        }
    } else if (strcmp(path, "/login") == 0 && strcmp(method, "POST") == 0) {
        char username[128], password[128];
        sscanf(body, "username=%127[^&]&password=%127s", username, password);

        char token[65]; // SHA-256 token (64 karakter + null sonlandırıcı)
        if (login_user(username, password, token)) {
            snprintf(response, sizeof(response),
                     "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n\r\n"
                     "{\"status\": \"success\", \"token\": \"%s\"}", token);
        } else {
            snprintf(response, sizeof(response),
                     "HTTP/1.1 401 Unauthorized\r\nContent-Type: application/json\r\n\r\n"
                     "{\"status\": \"error\", \"message\": \"Invalid username or password\"}");
        }
    } else {
        snprintf(response, sizeof(response),
                 "HTTP/1.1 404 Not Found\r\nContent-Type: application/json\r\n\r\n"
                 "{\"status\": \"error\", \"message\": \"Path not found\"}");
    }

    write(client_socket, response, strlen(response));
    close(client_socket);
}

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);

    // Sunucu soketini oluştur
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Socket oluşturulamadı");
        exit(EXIT_FAILURE);
    }

    // Adres yapısını ayarla
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Bağla
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Bağlantı başarısız");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    // Dinle
    if (listen(server_socket, 10) == -1) {
        perror("Dinleme başarısız");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    printf("Sunucu %d portunda çalışıyor...\n", PORT);

    while (1) {
        // İstemciden gelen bağlantıyı kabul et
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_len);
        if (client_socket == -1) {
            perror("Bağlantı kabul edilemedi");
            continue;
        }

        handle_request(client_socket);
    }

    close(server_socket);
    return 0;
}
