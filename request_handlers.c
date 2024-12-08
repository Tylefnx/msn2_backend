#include "request_handlers.h"
#include "auth.h"
#include "friends.h"
#include "messages.h"
#include <json-c/json.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024

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

void handle_register_request(struct json_object *parsed_json, int client_socket) {
    struct json_object *username, *password;
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
}

void handle_login_request(struct json_object *parsed_json, int client_socket) {
    struct json_object *username, *password;
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
}

void handle_add_friend_request(struct json_object *parsed_json, int client_socket) {
    struct json_object *username, *friend_username;
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
}

void handle_remove_friend_request(struct json_object *parsed_json, int client_socket) {
    struct json_object *username, *friend_username;
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
}

void handle_send_message_request(struct json_object *parsed_json, int client_socket) {
    struct json_object *sender, *receiver, *message;
    if (json_object_object_get_ex(parsed_json, "sender", &sender) &&
        json_object_object_get_ex(parsed_json, "receiver", &receiver) &&
        json_object_object_get_ex(parsed_json, "message", &message)) {

        Message msg;
        strcpy(msg.sender, json_object_get_string(sender));
        strcpy(msg.receiver, json_object_get_string(receiver));
        strcpy(msg.message, json_object_get_string(message));
        
        send_message(msg);
        send_json_response(client_socket, 200, "Message sent", NULL);
    } else {
        send_json_response(client_socket, 400, "Missing sender, receiver, or message", NULL);
    }
}

void handle_list_friends_request(struct json_object *parsed_json, int client_socket) {
    struct json_object *username;
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
}

void handle_list_messages_request(struct json_object *parsed_json, int client_socket) {
    struct json_object *username;
    if (json_object_object_get_ex(parsed_json, "username", &username)) {
        struct json_object *messages_array = json_object_new_array();
        for (int i = 0; i < message_count; i++) {
            if (strcmp(messages[i].receiver, json_object_get_string(username)) == 0) {
                struct json_object *message_obj = json_object_new_object();
                json_object_object_add(message_obj, "sender", json_object_new_string(messages[i].sender));
                json_object_object_add(message_obj, "message", json_object_new_string(messages[i].message));
                json_object_array_add(messages_array, message_obj);
            }
        }
        struct json_object *json_response = json_object_new_object();
        json_object_object_add(json_response, "status", json_object_new_int(200));
        json_object_object_add(json_response, "messages", messages_array);
        const char* json_str = json_object_to_json_string(json_response);
        char response[BUFFER_SIZE];
        snprintf(response, sizeof(response), "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n\r\n%s\n", json_str);
        send(client_socket, response, strlen(response), 0);
        json_object_put(json_response);  // JSON nesnesini serbest bırak
    } else {
        send_json_response(client_socket, 400, "Missing username", NULL);
    }
}
