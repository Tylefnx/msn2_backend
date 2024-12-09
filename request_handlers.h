#ifndef REQUEST_HANDLERS_H
#define REQUEST_HANDLERS_H

#include <json-c/json.h>

// Fonksiyon bildirimleri
void send_json_response(int client_socket, int status_code, const char* message, const char* token);
void handle_register_request(struct json_object *parsed_json, int client_socket);
void handle_login_request(struct json_object *parsed_json, int client_socket);
void handle_add_friend_request(struct json_object *parsed_json, int client_socket);
void handle_respond_friend_request(struct json_object *parsed_json, int client_socket);
void handle_remove_friend_request(struct json_object *parsed_json, int client_socket);
void handle_send_message_request(struct json_object *parsed_json, int client_socket);
void handle_list_friends_request(struct json_object *parsed_json, int client_socket);
void handle_list_messages_request(struct json_object *parsed_json, int client_socket); // Yeni fonksiyon bildirimini ekleyin
void handle_list_users_request(int client_socket);
void handle_list_chats_request(struct json_object *parsed_json, int client_socket);
void handle_list_friend_requests(int client_socket);
#endif // REQUEST_HANDLERS_H
