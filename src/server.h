#ifndef SERVER_H
#define SERVER_H

#include <microhttpd.h>
#include <json-c/json.h>

#define PORT 8080

// Sunucu başlatma fonksiyonu
void start_server();

// İstek işleme callback fonksiyonu
int answer_to_connection(void *cls, struct MHD_Connection *connection, const char *url,
                         const char *method, const char *version, const char *upload_data,
                         size_t *upload_data_size, void **con_cls);

#endif // SERVER_H
