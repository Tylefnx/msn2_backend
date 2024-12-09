#include "auth.h"
#include "../srv/request_handlers.h"
#include <json-c/json.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024

void handle_register_request(struct json_object *parsed_json, int client_socket) {
    struct json_object *username, *password;
    if (json_object_object_get_ex(parsed_json, "username", &username) &&
        json_object_object_get_ex(parsed_json, "password", &password)) {
        
        UserDB user;
        strcpy(user.username, json_object_get_string(username));
        strcpy(user.password, json_object_get_string(password));
        
        const char* result = register_user(user);
        send_json_response(client_socket, 200, result, NULL);
    } else {
        send_json_response(client_socket, 400, "Missing username or password", NULL);
    }
}

void handle_login_request(struct json_object *parsed_json, int client_socket) {
    struct json_object *username, *password;
    if (json_object_object_get_ex(parsed_json, "username", &username) &&
        json_object_object_get_ex(parsed_json, "password", &password)) {

        UserDB user;
        strcpy(user.username, json_object_get_string(username));
        strcpy(user.password, json_object_get_string(password));
        
        char* token = login_user(user);
        if (strcmp(token, "Invalid username or password!") == 0) {
            send_json_response(client_socket, 401, "Invalid username or password", NULL);
        } else if (token != NULL) {
            send_json_response(client_socket, 200, "Login successful", token);
            free(token);
        } else {
            send_json_response(client_socket, 500, "Failed to generate token", NULL);
        }
    } else {
        send_json_response(client_socket, 400, "Missing username or password", NULL);
    }
}
