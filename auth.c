#include "auth.h"
#include <string.h>
#include <stdlib.h>
#include <jwt.h>

// Basit bir kullanıcı veritabanı için örnek yapı
typedef struct {
    char username[50];
    char password[50];
} UserDB;

UserDB users[100]; // Maksimum 100 kullanıcı
int user_count = 0;

// JWT Token oluşturma işlevi
char* generate_jwt_token(char* username) {
    jwt_t *jwt = NULL;
    jwt_new(&jwt);
    jwt_add_grant(jwt, "sub", username);
    jwt_set_alg(jwt, JWT_ALG_HS256, (unsigned char*)"secret_key", strlen("secret_key"));
    char* token = jwt_encode_str(jwt);
    jwt_free(jwt);
    return token;
}

char* register_user(User user) {
    // Kullanıcının zaten kayıtlı olup olmadığını kontrol et
    for (int i = 0; i < user_count; i++) {
        if (strcmp(users[i].username, user.username) == 0) {
            return "Username already exists!";
        }
    }

    // Kullanıcıyı kaydet
    strcpy(users[user_count].username, user.username);
    strcpy(users[user_count].password, user.password);
    user_count++;

    return "Register successful!";
}

char* login_user(User user) {
    // Kullanıcıyı doğrula
    for (int i = 0; i < user_count; i++) {
        if (strcmp(users[i].username, user.username) == 0 && strcmp(users[i].password, user.password) == 0) {
            // Kullanıcı doğrulandı, JWT token oluştur
            return generate_jwt_token(user.username);
        }
    }

    return "Invalid username or password!";
}
