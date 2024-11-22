#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define USERS_FILE "users.txt"

// Kullanıcıyı dosyaya kaydeder
bool register_user(const char *username, const char *password) {
    FILE *file = fopen(USERS_FILE, "a");
    if (!file) {
        perror("Kullanıcı dosyası açılamadı");
        return false;
    }

    // Kullanıcıya benzersiz bir ID atayın (örnek: mevcut kullanıcı sayısına göre)
    int user_id = 1; // Varsayılan ID
    char line[256];
    FILE *read_file = fopen(USERS_FILE, "r");
    while (fgets(line, sizeof(line), read_file)) {
        user_id++;
    }
    fclose(read_file);

    fprintf(file, "%d:%s:%s\n", user_id, username, password);
    fclose(file);
    return true;
}

// Kullanıcıyı doğrular ve user_id döner
int login_user(const char *username, const char *password) {
    FILE *file = fopen(USERS_FILE, "r");
    if (!file) {
        perror("Kullanıcı dosyası açılamadı");
        return -1;
    }

    char line[256];
    char file_username[128], file_password[128];
    int user_id;

    while (fgets(line, sizeof(line), file)) {
        sscanf(line, "%d:%[^:]:%s", &user_id, file_username, file_password);
        if (strcmp(username, file_username) == 0 && strcmp(password, file_password) == 0) {
            fclose(file);
            return user_id;
        }
    }

    fclose(file);
    return -1; // Kullanıcı bulunamadı veya şifre hatalı
}