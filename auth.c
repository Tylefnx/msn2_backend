#include "auth.h"
#include <string.h>
#include <stdlib.h>
#include <jwt.h>
#include <stdio.h>

UserDB users[100]; // Maksimum 100 kullanıcı
int user_count = 0;

// Kullanıcıları dosyaya kaydetme fonksiyonu
void save_users_to_file() {
    FILE *file = fopen("users.txt", "w");
    if (file == NULL) {
        perror("Failed to open file for saving users");
        return;
    }
    for (int i = 0; i < user_count; i++) {
        fprintf(file, "%s %s\n", users[i].username, users[i].password);
    }
    fclose(file);
}

// Dosyadan kullanıcıları yükleme fonksiyonu
void load_users_from_file() {
    FILE *file = fopen("users.txt", "r");
    if (file == NULL) {
        perror("Failed to open file for loading users");
        return;
    }
    user_count = 0;
    while (fscanf(file, "%s %s", users[user_count].username, users[user_count].password) == 2) {
        user_count++;
    }
    fclose(file);
}

// JWT Token oluşturma işlevi
char* generate_jwt_token(char* username) {
    jwt_t *jwt = NULL;
    if (jwt_new(&jwt) != 0) {
        return NULL; // JWT oluşturulamazsa NULL döner
    }
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
    save_users_to_file(); // Kullanıcıları dosyaya kaydet

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
