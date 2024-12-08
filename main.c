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

// JSON response oluşturma yardımcı işlevi
void send_json_response(int client_socket, int status_code, const char* message, const char* token) {
    struct json_object *json_response = json_object_new_object();
    json_object_object_add(json_response, "status", json_object_new_int(status_code));
    json_object_object_add(json_response, "message", json_object_new_string(message));
    if (token != NULL) {
        json_object_object_add(json_response, "token", json_object_new_string(token));
    }
    const char* json_str = json_object_to_json_string(json_response);
    char response[BUFFER_SIZE];
    snprintf(response, sizeof(response), "HTTP/1.1 %d OK\r\nContent-Type: application/json\r\n\r\n%s\n", status_code, json_str);
    send(client_socket, response, strlen(response), 0);
    json_object_put(json_response);  // JSON nesnesini serbest bırak
    printf("Response sent: %s\n", response);  // Debug çıkışı
}

// Request işleme fonksiyonu
void process_request(Request req, int client_socket) {
    struct json_object *parsed_json = NULL;
    struct json_object *username = NULL;
    struct json_object *password = NULL;
    struct json_object *friend_username = NULL;
    struct json_object *message = NULL;

    printf("Processing request for endpoint: %s\n", req.endpoint);  // Debug çıkışı

    parsed_json = json_tokener_parse(req.data);
    if (!parsed_json) {
        send_json_response(client_socket, 400, "Invalid JSON format", NULL);
        return;
    }

    printf("Parsed JSON data: %s\n", json_object_to_json_string(parsed_json));  // Debug çıkışı
    
    if (strcmp(req.endpoint, "/register") == 0) {
        if (json_object_object_get_ex(parsed_json, "username", &username) &&
            json_object_object_get_ex(parsed_json, "password", &password)) {
            
            User user;
            strcpy(user.username, json_object_get_string(username));
            strcpy(user.password, json_object_get_string(password));
            
            const char* result = register_user(user);
            send_json_response(client_socket, 200, result, NULL);
        } else {
            send_json_response(client_socket, 400, "Missing username or password", NULL);
        }
    } else if (strcmp(req.endpoint, "/login") == 0) {
        if (json_object_object_get_ex(parsed_json, "username", &username) &&
            json_object_object_get_ex(parsed_json, "password", &password)) {

            User user;
            strcpy(user.username, json_object_get_string(username));
            strcpy(user.password, json_object_get_string(password));
            
            char* token = login_user(user);
            if (strcmp(token, "Invalid username or password!") == 0) {
                send_json_response(client_socket, 401, "Invalid username or password", NULL);
            } else if (token != NULL) {
                send_json_response(client_socket, 200, "Login successful", token);
                free(token);  // Token bellekten serbest bırak
            } else {
                send_json_response(client_socket, 500, "Failed to generate token", NULL);
            }
        } else {
            send_json_response(client_socket, 400, "Missing username or password", NULL);
        }
    } else if (strcmp(req.endpoint, "/add_friend") == 0) {
        if (json_object_object_get_ex(parsed_json, "username", &username) &&
            json_object_object_get_ex(parsed_json, "friend_username", &friend_username)) {

            FriendRequest req_data;
            strcpy(req_data.username, json_object_get_string(username));
            strcpy(req_data.friend_username, json_object_get_string(friend_username));

            add_friend(req_data);
            send_json_response(client_socket, 200, "Friend added", NULL);
        } else {
            send_json_response(client_socket, 400, "Missing username or friend_username", NULL);
        }
    } else if (strcmp(req.endpoint, "/remove_friend") == 0) {
        if (json_object_object_get_ex(parsed_json, "username", &username) &&
            json_object_object_get_ex(parsed_json, "friend_username", &friend_username)) {

            FriendRequest req_data;
            strcpy(req_data.username, json_object_get_string(username));
            strcpy(req_data.friend_username, json_object_get_string(friend_username));
            
            remove_friend(req_data);
            send_json_response(client_socket, 200, "Friend removed", NULL);
        } else {
            send_json_response(client_socket, 400, "Missing username or friend_username", NULL);
        }
    } else if (strcmp(req.endpoint, "/send_message") == 0) {
        if (json_object_object_get_ex(parsed_json, "sender", &username) &&
            json_object_object_get_ex(parsed_json, "receiver", &friend_username) &&
            json_object_object_get_ex(parsed_json, "message", &message)) {

            Message msg;
            strcpy(msg.sender, json_object_get_string(username));
            strcpy(msg.receiver, json_object_get_string(friend_username));
            strcpy(msg.message, json_object_get_string(message));
            
            send_message(msg);
            send_json_response(client_socket, 200, "Message sent", NULL);
        } else {
            send_json_response(client_socket, 400, "Missing sender, receiver, or message", NULL);
        }
    } else if (strcmp(req.endpoint, "/list_friends") == 0) {
        if (json_object_object_get_ex(parsed_json, "username", &username)) {
            UserFriends* user = find_user_friends(json_object_get_string(username));
            if (user == NULL) {
                send_json_response(client_socket, 404, "User not found", NULL);
            } else {
                struct json_object *friends_list = json_object_new_array();
                for (int i = 0; i < user->friend_count; i++) {
                    json_object_array_add(friends_list, json_object_new_string(user->friends[i]));
                }
                struct json_object *json_response = json_object_new_object();
                json_object_object_add(json_response, "status", json_object_new_int(200));
                json_object_object_add(json_response, "friends", friends_list);
                const char* json_str = json_object_to_json_string(json_response);
                char response[BUFFER_SIZE];
                snprintf(response, sizeof(response), "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n\r\n%s\n", json_str);
                send(client_socket, response, strlen(response), 0);
                json_object_put(json_response);  // JSON nesnesini serbest bırak
            }
        } else {
            send_json_response(client_socket, 400, "Missing username", NULL);
        }
    } else {
        send_json_response(client_socket, 400, "Unknown endpoint", NULL);
    }

    json_object_put(parsed_json);

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
