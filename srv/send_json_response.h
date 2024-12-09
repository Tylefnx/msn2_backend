#ifndef SEND_JSON_RESPONSE_H
#define SEND_JSON_RESPONSE_H

void send_json_response(int client_socket, int status_code, const char* message, const char* token);

#endif // SEND_JSON_RESPONSE_H
