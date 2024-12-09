#include "../auth/auth.h"
#include "../send_json_response.c"
#include "friends.h"
#include "../messages/messages.h"
#include "../request_handlers.h"
#include <json-c/json.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024

void handle_add_friend_request(struct json_object *parsed_json, int client_socket) {
    struct json_object *username, *friend_username;
    if (json_object_object_get_ex(parsed_json, "username", &username) &&
        json_object_object_get_ex(parsed_json, "friend_username", &friend_username)) {

        char* requester = (char*)json_object_get_string(username);
        char* requestee = (char*)json_object_get_string(friend_username);

        const char* result = add_friend_request(requester, requestee);
        send_json_response(client_socket, 200, result, NULL);
    } else {
        send_json_response(client_socket, 400, "Missing username or friend_username", NULL);
    }
}

void handle_respond_friend_request(struct json_object *parsed_json, int client_socket) {
    struct json_object *requestee, *requester, *response;
    if (json_object_object_get_ex(parsed_json, "requestee", &requestee) &&
        json_object_object_get_ex(parsed_json, "requester", &requester) &&
        json_object_object_get_ex(parsed_json, "response", &response)) {

        char* requestee_str = (char*)json_object_get_string(requestee);
        char* requester_str = (char*)json_object_get_string(requester);

        const char* result = respond_friend_request(requestee_str, requester_str, json_object_get_int(response));
        send_json_response(client_socket, 200, result, NULL);
    } else {
        send_json_response(client_socket, 400, "Missing requestee, requester, or response", NULL);
    }
}

void handle_remove_friend_request(struct json_object *parsed_json, int client_socket) {
    struct json_object *username, *friend_username;
    if (json_object_object_get_ex(parsed_json, "username", &username) &&
        json_object_object_get_ex(parsed_json, "friend_username", &friend_username)) {

        char* username_str = (char*)json_object_get_string(username);
        char* friend_username_str = (char*)json_object_get_string(friend_username);

        remove_friend(username_str, friend_username_str);
        send_json_response(client_socket, 200, "Friend removed", NULL);
    } else {
        send_json_response(client_socket, 400, "Missing username or friend_username", NULL);
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
            json_object_put(json_response);
        }
    } else {
        send_json_response(client_socket, 400, "Missing username", NULL);
    }
}

void handle_list_friend_requests(int client_socket) {
    char* response_str = list_friend_requests();
    struct json_object *json_response = json_tokener_parse(response_str);
    const char* json_str = json_object_to_json_string(json_response);
    char response[BUFFER_SIZE];
    snprintf(response, sizeof(response), "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n\r\n%s\n", json_str);
    send(client_socket, response, strlen(response), 0);
    json_object_put(json_response);
    free(response_str); // Serbest bırak
}

void handle_list_users_request(int client_socket) {
    UserDB* users = get_all_users();
    int user_count = get_user_count();

    struct json_object *users_array = json_object_new_array();
    for (int i = 0; i < user_count; i++) {
        struct json_object *user_obj = json_object_new_object();
        json_object_object_add(user_obj, "username", json_object_new_string(users[i].username));
        json_object_array_add(users_array, user_obj);
    }

    struct json_object *json_response = json_object_new_object();
    json_object_object_add(json_response, "status", json_object_new_int(200));
    json_object_object_add(json_response, "users", users_array);
    const char* json_str = json_object_to_json_string(json_response);
    char response[BUFFER_SIZE];
    snprintf(response, sizeof(response), "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n\r\n%s\n", json_str);
    send(client_socket, response, strlen(response), 0);
    json_object_put(json_response);
}