#include "request_handlers.h"
#include <json-c/json.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024

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
    json_object_put(json_response);
    printf("Response sent: %s\n", response);
}