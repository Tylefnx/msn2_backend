#include "auth.h"
#include <string.h>
#include <stdlib.h>
#include <jwt.h>
#include <stdio.h>
#include <openssl/sha.h>

UserDB users[100]; // Maksimum 100 kullanıcı
int user_count = 0;

UserDB* get_all_users() { 
    return users; 
} 
int get_user_count() { 
    return user_count; 
}

// Parola hash'leme fonksiyonu
void hash_password(const char *password, char *hash) {
    unsigned char hash_bytes[SHA256_DIGEST_LENGTH];
    SHA256((unsigned char*)password, strlen(password), hash_bytes);
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        sprintf(hash + (i * 2), "%02x", hash_bytes[i]);
    }
    hash[SHA256_DIGEST_LENGTH * 2] = '\0';
}

// Kullanıcıları dosyaya kaydetme fonksiyonu
void save_users_to_file() {
    FILE *file = fopen("db/users.txt", "w"); // Dosyayı yazma modunda aç
    if (file == NULL) {
        perror("Failed to open file for saving users");
        return;
    }
    for (int i = 0; i < user_count; i++) {
        // Kullanıcı bilgilerini doğru formatta yazdığımızdan emin olalım
        if (fprintf(file, "%s %s\n", users[i].username, users[i].password) < 0) {
            perror("Failed to write user to file");
        }
    }
    if (fflush(file) != 0) {
        perror("Failed to flush file buffer");
    }
    if (fclose(file) != 0) {
        perror("Failed to close file");
    }
}

// Dosyadan kullanıcıları yükleme fonksiyonu
void load_users_from_file() {
    FILE *file = fopen("db/users.txt", "r");
    if (file == NULL) {
        perror("Failed to open file for loading users");
        return;
    }
    user_count = 0;
    char line[120]; // Kullanıcı adı ve parolayı birlikte tutacak tampon
    while (fgets(line, sizeof(line), file)) {
        // Satırın sonunda yeni satır karakteri varsa kaldır
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') {
            line[len - 1] = '\0';
        }

        // Kullanıcı adı ve parolayı boşluk karakterine göre ayır
        char *username = strtok(line, " ");
        char *password = strtok(NULL, " ");

        if (username && password) {
            strncpy(users[user_count].username, username, sizeof(users[user_count].username) - 1);
            users[user_count].username[sizeof(users[user_count].username) - 1] = '\0';
            strncpy(users[user_count].password, password, sizeof(users[user_count].password) - 1);
            users[user_count].password[sizeof(users[user_count].password) - 1] = '\0';
            user_count++;
        }
    }
    if (fclose(file) != 0) {
        perror("Failed to close file");
    }
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

char* register_user(UserDB user) {
    // Kullanıcının zaten kayıtlı olup olmadığını kontrol et
    for (int i = 0; i < user_count; i++) {
        if (strcmp(users[i].username, user.username) == 0) {
            return "Username already exists!";
        }
    }

    // Kullanıcıyı kaydet
    strcpy(users[user_count].username, user.username);
    hash_password(user.password, users[user_count].password);
    user_count++;
    save_users_to_file(); // Kullanıcıları dosyaya kaydet

    // Kullanıcı listesini yeniden yükle
    load_users_from_file();

    return "Register successful!";
}

char* login_user(UserDB user) {
    char hashed_password[65];
    hash_password(user.password, hashed_password);
    // Kullanıcıyı doğrula
    for (int i = 0; i < user_count; i++) {
        if (strcmp(users[i].username, user.username) == 0 && strcmp(users[i].password, hashed_password) == 0) {
            // Kullanıcı doğrulandı, JWT token oluştur
            return generate_jwt_token(user.username);
        }
    }

    return "Invalid username or password!";
}

// Kullanıcıları yazdırma fonksiyonu (Test için)
void print_users() {
    for (int i = 0; i < user_count; i++) {
        printf("Username: %s, Password: %s\n", users[i].username, users[i].password);
    }
}
