#include "auth.h"
#include "friends.h"
#include "messages.h"
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
    char response[BUFFER_SIZE];
    memset(response, 0, sizeof(response));  // response'u sıfırla
    
    struct json_object *parsed_json;
    struct json_object *username;
    struct json_object *password;
    struct json_object *friend_username;
    struct json_object *message;

    parsed_json = json_tokener_parse(req.data);
    
    if (strcmp(req.endpoint, "/register") == 0) {
        json_object_object_get_ex(parsed_json, "username", &username);
        json_object_object_get_ex(parsed_json, "password", &password);
        
        User user;
        strcpy(user.username, json_object_get_string(username));
        strcpy(user.password, json_object_get_string(password));
        
        snprintf(response, sizeof(response), "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n%s\n", register_user(user));
    } else if (strcmp(req.endpoint, "/login") == 0) {
        json_object_object_get_ex(parsed_json, "username", &username);
        json_object_object_get_ex(parsed_json, "password", &password);

        User user;
        strcpy(user.username, json_object_get_string(username));
        strcpy(user.password, json_object_get_string(password));
        
        snprintf(response, sizeof(response), "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nToken: %s\n", login_user(user));
    } else if (strcmp(req.endpoint, "/add_friend") == 0) {
        json_object_object_get_ex(parsed_json, "username", &username);
        json_object_object_get_ex(parsed_json, "friend_username", &friend_username);

        FriendRequest req_data;
        strcpy(req_data.username, json_object_get_string(username));
        strcpy(req_data.friend_username, json_object_get_string(friend_username));

        add_friend(req_data);
        snprintf(response, sizeof(response), "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nFriend added.\n");
    } else if (strcmp(req.endpoint, "/remove_friend") == 0) {
        json_object_object_get_ex(parsed_json, "username", &username);
        json_object_object_get_ex(parsed_json, "friend_username", &friend_username);

        FriendRequest req_data;
        strcpy(req_data.username, json_object_get_string(username));
        strcpy(req_data.friend_username, json_object_get_string(friend_username));
        
        remove_friend(req_data);
        snprintf(response, sizeof(response), "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nFriend removed.\n");
    } else if (strcmp(req.endpoint, "/send_message") == 0) {
        json_object_object_get_ex(parsed_json, "sender", &username);
        json_object_object_get_ex(parsed_json, "receiver", &friend_username);
        json_object_object_get_ex(parsed_json, "message", &message);

        Message msg;
        strcpy(msg.sender, json_object_get_string(username));
        strcpy(msg.receiver, json_object_get_string(friend_username));
        strcpy(msg.message, json_object_get_string(message));
        
        send_message(msg);
        snprintf(response, sizeof(response), "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nMessage sent.\n");
    } else if (strcmp(req.endpoint, "/list_friends") == 0) {
        json_object_object_get_ex(parsed_json, "username", &username);
        
        UserFriends* user = find_user_friends(json_object_get_string(username));
        if (user == NULL) {
            snprintf(response, sizeof(response), "HTTP/1.1 404 Not Found\r\nContent-Type: text/plain\r\n\r\nUser not found.\n");
        } else {
            snprintf(response, sizeof(response), "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nFriends of %s:\n", json_object_get_string(username));
            send(client_socket, response, strlen(response), 0);
            for (int i = 0; i < user->friend_count; i++) {
                snprintf(response, sizeof(response), "- %s\n", user->friends[i]);
                send(client_socket, response, strlen(response), 0);
            }
            json_object_put(parsed_json);  // JSON nesnesini serbest bırak
            return; // Yanıtı gönderdiğimiz için fonksiyonu bitir
        }
    } else {
        snprintf(response, sizeof(response), "HTTP/1.1 400 Bad Request\r\nContent-Type: text/plain\r\n\r\nUnknown endpoint: %s\n", req.endpoint);
    }

    send(client_socket, response, strlen(response), 0);
    json_object_put(parsed_json);  // JSON nesnesini serbest bırak
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
                char *response = "HTTP/1.1 400 Bad Request\r\nContent-Type: text/plain\r\n\r\nInvalid request format.\n";
                send(new_socket, response, strlen(response), 0);
            }
        } else {
            char *response = "HTTP/1.1 400 Bad Request\r\nContent-Type: text/plain\r\n\r\nInvalid request format.\n";
            send(new_socket, response, strlen(response), 0);
        }

        // Bağlantıyı kapat
        close(new_socket);
    }

    return 0;
}
