#include "messages.h"
#include <microhttpd.h>
#include <string.h>

int handle_send_message(struct MHD_Connection *connection, const char *upload_data, size_t upload_data_size) {
    // Mesaj gönderme işlevinin implementasyonu
    // Örnek: Yanıt oluşturma
    const char *response_msg = "{\"status\":\"message sent\"}";
    struct MHD_Response *response = MHD_create_response_from_buffer(strlen(response_msg), (void *)response_msg, MHD_RESPMEM_MUST_FREE);
    MHD_add_response_header(response, "Content-Type", "application/json");
    int ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
    MHD_destroy_response(response);
    return ret;
}

int handle_add_friend(struct MHD_Connection *connection, const char *upload_data, size_t upload_data_size) {
    // Arkadaş ekleme işlevinin implementasyonu
    // Örnek: Yanıt oluşturma
    const char *response_msg = "{\"status\":\"friend added\"}";
    struct MHD_Response *response = MHD_create_response_from_buffer(strlen(response_msg), (void *)response_msg, MHD_RESPMEM_MUST_FREE);
    MHD_add_response_header(response, "Content-Type", "application/json");
    int ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
    MHD_destroy_response(response);
    return ret;
}
