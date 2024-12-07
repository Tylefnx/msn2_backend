#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <openssl/sha.h> // OpenSSL kütüphanesi

#define USERS_FILE "users.txt"

// Kullanıcı kaydetme fonksiyonu
bool register_user(const char *username, const char *password) {
    FILE *file = fopen(USERS_FILE, "a");
    if (!file) {
        perror("Kullanıcı dosyası açılamadı");
        return false;
    }

    fprintf(file, "%s:%s\n", username, password);
    fclose(file);
    return true;
}

// SHA-256 hash fonksiyonu
void generate_token(const char *username, const char *random_value, char *output) {
    char input[256];
    snprintf(input, sizeof(input), "%s:%s", username, random_value);

    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256((unsigned char *)input, strlen(input), hash);

    // Hash'i hex formatına çevir
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        sprintf(output + (i * 2), "%02x", hash[i]);
    }
    output[SHA256_DIGEST_LENGTH * 2] = '\0';
}

// Kullanıcı girişini doğrulama ve token oluşturma
bool login_user(const char *username, const char *password, char *token) {
    FILE *file = fopen(USERS_FILE, "r");
    if (!file) {
        perror("Kullanıcı dosyası açılamadı");
        return false;
    }

    char line[256];
    char file_username[128], file_password[128];

    while (fgets(line, sizeof(line), file)) {
        sscanf(line, "%127[^:]:%127s", file_username, file_password);

        if (strcmp(username, file_username) == 0 && strcmp(password, file_password) == 0) {
            // Kullanıcı doğrulandı, token oluştur
            char random_value[16];
            snprintf(random_value, sizeof(random_value), "%ld", random());
            generate_token(username, random_value, token);
            fclose(file);
            return true;
        }
    }

    fclose(file);
    return false; // Kullanıcı bulunamadı veya şifre hatalı
}