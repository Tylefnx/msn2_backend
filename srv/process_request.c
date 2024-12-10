#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <json-c/json.h>
#include <jwt.h>
#include "../auth/auth.h"
#include "../friends/friends.h"
#include "../messages/messages.h"
#include "request_handlers.h"
#include "process_request.h" 
#include "validate_token.h"

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
        printf("DIKKAT JSON_START: %s", json_start);
        if (json_start != NULL) {
            printf("DIKKAT JSON_START: %s", req.data);
            strcpy(req.data, json_start + 4);  // JSON verisini kopyala
            printf("DIKKAT R: %s", req.data);
            struct json_object *parsed_json = json_tokener_parse(req.data);
            if (!parsed_json) {
                send_json_response(client_socket, 400, "Invalid JSON format", NULL);
                close(client_socket);
                return;
            }

            printf("Processing request for endpoint: %s\n", req.endpoint);  // Debug çıkışı

            struct json_object *token_obj = NULL;
            char username[50];
            if (strcmp(req.endpoint, "/list_users") == 0) {
                handle_list_users_request(client_socket);
            } else if (strcmp(req.endpoint, "/register") == 0) {
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
                } else if (strcmp(req.endpoint, "/respond_friend") == 0) {
                    handle_respond_friend_request(parsed_json, client_socket);
                } else if (strcmp(req.endpoint, "/remove_friend") == 0) {
                    handle_remove_friend_request(parsed_json, client_socket);
                } else if (strcmp(req.endpoint, "/send_message") == 0) {
                    handle_send_message_request(parsed_json, client_socket);
                } else if (strcmp(req.endpoint, "/list_chats") == 0) {
                    handle_list_chats_request(parsed_json, client_socket);
                } else if (strcmp(req.endpoint, "/list_messages") == 0) {
                    handle_list_messages_request(parsed_json, client_socket);
                } else if (strcmp(req.endpoint, "/list_friends") == 0) {
                    handle_list_friends_request(parsed_json, client_socket);
                } else if (strcmp(req.endpoint, "/list_friend_requests") == 0) {
                    handle_list_friend_requests(client_socket);
                } else if (strcmp(req.endpoint, "/list_specific_chat") == 0) {
                    handle_list_specific_chat_request(parsed_json, client_socket);
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
