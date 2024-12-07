#include <stdio.h>
#include <string.h>
#include "friendship.h"
#include "json-c/json.h"

// Arkadaş ekleme işlemi
int handle_add_friend(struct MHD_Connection *connection, const char *upload_data, size_t upload_data_size) {
    json_object *json_obj = json_tokener_parse(upload_data);
    const char *username = json_object_get_string(json_object_object_get(json_obj, "username"));
    const char *friend_username = json_object_get_string(json_object_object_get(json_obj, "friend_username"));
    printf("Add friend: %s wants to add %s\n", username, friend_username);

    // Arkadaş ekleme işlemi burada yapılacak

    const char *response_msg = "{\"status\": \"success\", \"message\": \"Friend added\"}";
    struct MHD_Response *response = MHD_create_response_from_buffer(strlen(response_msg), (void *)response_msg, MHD_RESPMEM_MUST_FREE);
    MHD_add_response_header(response, "Content-Type", "application/json");
    int ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
    MHD_destroy_response(response);
    return ret;
}
