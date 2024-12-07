#include "auth.h"
#include <microhttpd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define PORT 8080

// Kullanıcı kaydı işleme fonksiyonu
int handle_register(struct MHD_Connection *connection, const char *upload_data, size_t upload_data_size) {
    const char *response_msg = "{\"status\":\"registered\"}";
    struct MHD_Response *response = MHD_create_response_from_buffer(strlen(response_msg), (void *)response_msg, MHD_RESPMEM_MUST_FREE);
    
    MHD_add_response_header(response, "Content-Type", "application/json");
    
    // Yanıtı kuyrukla
    int ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
    MHD_destroy_response(response);
    
    return ret;
}

// Kullanıcı girişi işlemi
int handle_login(struct MHD_Connection *connection, const char *upload_data, size_t upload_data_size) {
    const char *response_msg = "{\"status\":\"logged_in\"}";
    struct MHD_Response *response = MHD_create_response_from_buffer(strlen(response_msg), (void *)response_msg, MHD_RESPMEM_MUST_FREE);
    
    // Token doğrulama işlemi burada yapılacak (örneğin, JWT kullanarak)
    // Şu an için basit bir "dummy_token" geçiyor
    const char *token = "dummy_token"; 
    
    // Başlıklar ve yanıt ekleniyor
    MHD_add_response_header(response, "Content-Type", "application/json");
    
    // Token doğrulama
    if (token != NULL) {
        // Başarılı giriş
        int ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
        MHD_destroy_response(response);
        return ret;
    } else {
        // Yetkisiz giriş
        const char *unauthorized_msg = "{\"error\":\"unauthorized\"}";
        struct MHD_Response *unauthorized_response = MHD_create_response_from_buffer(strlen(unauthorized_msg), (void *)unauthorized_msg, MHD_RESPMEM_MUST_FREE);
        MHD_add_response_header(unauthorized_response, "Content-Type", "application/json");
        int ret = MHD_queue_response(connection, MHD_HTTP_UNAUTHORIZED, unauthorized_response);
        MHD_destroy_response(unauthorized_response);
        return ret;
    }
}
