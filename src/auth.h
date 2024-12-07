#ifndef AUTH_H
#define AUTH_H

#include <microhttpd.h>
#include <stdbool.h>

// Token yapısını tanımlıyoruz
typedef struct {
    char *token;
    size_t length;
} AuthToken;

// Kullanıcı veritabanı giriş yapacak kullanıcı bilgileri için bir yapı
typedef struct {
    char *username;
    char *password;
} User;

// Kullanıcı veritabanı işlemleri için fonksiyon prototipleri
bool register_user(const char *username, const char *password);
bool login_user(const char *username, const char *password);

// Token doğrulama fonksiyonları
AuthToken* generate_token(const char *username);
bool validate_token(const char *token);

// HTTP isteği işleme fonksiyonları
int handle_register(struct MHD_Connection *connection, const char *upload_data, size_t upload_data_size);
int handle_login(struct MHD_Connection *connection, const char *upload_data, size_t upload_data_size);

// Token doğrulama işlemi için yardımcı fonksiyon
int authenticate_request(const char *token);

#endif /* AUTH_H */
