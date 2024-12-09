#include "../auth/auth.h"
#include "../friends/friends.h"
#include "messages.h"
#include "../request_handlers.h"
#include <json-c/json.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024

void handle_send_message_request(struct json_object *parsed_json, int client_socket) {
    struct json_object *sender, *receiver, *message;
    if (json_object_object_get_ex(parsed_json, "sender", &sender) &&
        json_object_object_get_ex(parsed_json, "receiver", &receiver) &&
        json_object_object_get_ex(parsed_json, "message", &message)) {

        MessageDB msg;
        strcpy(msg.sender, json_object_get_string(sender));
        strcpy(msg.receiver, json_object_get_string(receiver));
        strcpy(msg.message, json_object_get_string(message));
        
        send_message(msg);
        send_json_response(client_socket, 200, "Message sent", NULL);
    } else {
        send_json_response(client_socket, 400, "Missing sender, receiver, or message", NULL);
    }
}


void handle_list_messages_request(struct json_object *parsed_json, int client_socket) {
    struct json_object *username;
    if (json_object_object_get_ex(parsed_json, "username", &username)) {
        struct json_object *messages_array = json_object_new_array();
        for (int i = 0; i < chat_db_count; i++) {
            if (strcmp(chat_db[i].sender, json_object_get_string(username)) == 0 || strcmp(chat_db[i].receiver, json_object_get_string(username)) == 0) {
                for (int j = 0; j < chat_db[i].messages_count; j++) {
                    struct json_object *message_obj = json_object_new_object();
                    json_object_object_add(message_obj, "sender", json_object_new_string(chat_db[i].messages[j].sender));
                    json_object_object_add(message_obj, "receiver", json_object_new_string(chat_db[i].messages[j].receiver));
                    json_object_object_add(message_obj, "message", json_object_new_string(chat_db[i].messages[j].message));
                    json_object_array_add(messages_array, message_obj);
                }
            }
        }
        struct json_object *json_response = json_object_new_object();
        json_object_object_add(json_response, "status", json_object_new_int(200));
        json_object_object_add(json_response, "messages", messages_array);
        const char* json_str = json_object_to_json_string(json_response);
        char response[BUFFER_SIZE];
        snprintf(response, sizeof(response), "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n\r\n%s\n", json_str);
        send(client_socket, response, strlen(response), 0);
        json_object_put(json_response);
    } else {
        send_json_response(client_socket, 400, "Missing username", NULL);
    }
}

void handle_list_chats_request(struct json_object *parsed_json, int client_socket) {
    struct json_object *username;
    if (json_object_object_get_ex(parsed_json, "username", &username)) {
        struct json_object *chats_array = json_object_new_array();
        for (int i = 0; i < chat_db_count; i++) {
            if (strcmp(chat_db[i].sender, json_object_get_string(username)) == 0 || strcmp(chat_db[i].receiver, json_object_get_string(username)) == 0) {
                struct json_object *chat_obj = json_object_new_object();
                json_object_object_add(chat_obj, "sender", json_object_new_string(chat_db[i].sender));
                json_object_object_add(chat_obj, "receiver", json_object_new_string(chat_db[i].receiver));
                json_object_array_add(chats_array, chat_obj);
            }
        }
        struct json_object *json_response = json_object_new_object();
        json_object_object_add(json_response, "status", json_object_new_int(200));
        json_object_object_add(json_response, "chats", chats_array);
        const char* json_str = json_object_to_json_string(json_response);
        char response[BUFFER_SIZE];
        snprintf(response, sizeof(response), "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n\r\n%s\n", json_str);
        send(client_socket, response, strlen(response), 0);
        json_object_put(json_response);
    } else {
        send_json_response(client_socket, 400, "Missing username", NULL);
    }
}



void handle_list_specific_chat_request(struct json_object *parsed_json, int client_socket) {
    struct json_object *username1, *username2;
    if (json_object_object_get_ex(parsed_json, "username1", &username1) &&
        json_object_object_get_ex(parsed_json, "username2", &username2)) {

        struct json_object *messages_array = json_object_new_array();
        const char* user1 = json_object_get_string(username1);
        const char* user2 = json_object_get_string(username2);

        // Mesajları ChatDB'den almak
        for (int i = 0; i < chat_db_count; i++) {
            if ((strcmp(chat_db[i].sender, user1) == 0 && strcmp(chat_db[i].receiver, user2) == 0) ||
                (strcmp(chat_db[i].sender, user2) == 0 && strcmp(chat_db[i].receiver, user1) == 0)) {
                for (int j = 0; j < chat_db[i].messages_count; j++) {
                    struct json_object *message_obj = json_object_new_object();
                    json_object_object_add(message_obj, "sender", json_object_new_string(chat_db[i].messages[j].sender));
                    json_object_object_add(message_obj, "receiver", json_object_new_string(chat_db[i].messages[j].receiver));
                    json_object_object_add(message_obj, "message", json_object_new_string(chat_db[i].messages[j].message));
                    json_object_array_add(messages_array, message_obj);
                }
            }
        }

        struct json_object *json_response = json_object_new_object();
        json_object_object_add(json_response, "status", json_object_new_int(200));
        json_object_object_add(json_response, "messages", messages_array);
        const char* json_str = json_object_to_json_string(json_response);
        char response[BUFFER_SIZE];
        snprintf(response, sizeof(response), "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n\r\n%s\n", json_str);
        send(client_socket, response, strlen(response), 0);
        json_object_put(json_response);
    } else {
        send_json_response(client_socket, 400, "Missing username1 or username2", NULL);
    }
}
