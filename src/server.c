#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <microhttpd.h>
#include "auth.h"
#include "messages.h"

// Sunucu için sabit port numarası
#define PORT 8080

// HTTP isteklerini işleme fonksiyonu
static int answer_to_connection(void *cls, struct MHD_Connection *connection, const char *url,
                                const char *method, const char *version, const char *upload_data,
                                size_t *upload_data_size, void **con_cls) {

    // Kimlik doğrulama işlemi (header veya token kontrolü burada yapılabilir)
    if (strcmp(method, "POST") == 0) {
        if (strcmp(url, "/register") == 0) {
            // Kayıt işlemi
            return handle_register(connection, upload_data, *upload_data_size);
        } else if (strcmp(url, "/login") == 0) {
            // Giriş işlemi
            return handle_login(connection, upload_data, *upload_data_size);
        } else if (strcmp(url, "/add_friend") == 0) {
            // Arkadaş ekleme işlemi
            return handle_add_friend(connection, upload_data, *upload_data_size);
        } else if (strcmp(url, "/send_message") == 0) {
            // Mesaj gönderme işlemi
            return handle_send_message(connection, upload_data, *upload_data_size);
        } else {
            // Bilinmeyen bir endpoint
            return MHD_NO;
        }
    } else {
        // Diğer HTTP metotları için 405 - Method Not Allowed
        return MHD_NO;
    }
}

// Sunucuyu başlatma fonksiyonu
int start_server() {
    struct MHD_Daemon *daemon;

    // MHD_daemon başlatılır: sunucu, bağlantı fonksiyonları, port ve HTTP özellikleri
    daemon = MHD_start_daemon(MHD_USE_INTERNAL_POLLING_THREAD, PORT, NULL, NULL, &answer_to_connection, NULL, MHD_OPTION_END);
    if (daemon == NULL) {
        fprintf(stderr, "Sunucu başlatılamadı!\n");
        return 1;
    }

    printf("Sunucu başlatıldı: http://localhost:%d\n", PORT);

    // Sonsuza kadar bekleyin, sunucu çalışmaya devam etsin
    getchar();

    // Sunucu kapatılır
    MHD_stop_daemon(daemon);
    return 0;
}

int main() {
    // Sunucuyu başlat
    return start_server();
}
