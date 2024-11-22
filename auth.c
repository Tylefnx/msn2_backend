#include "./libs/auth.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct {
    char username[50];
    char password[50];
} User;

#define MAX_USERS 100
User users[MAX_USERS];
int user_count = 0;

bool register_user(const char *username, const char *password) {
    if (user_count >= MAX_USERS) {
        printf("Maksimum kullanıcı limitine ulaşıldı.\n");
        return false;
    }

    strcpy(users[user_count].username, username);
    strcpy(users[user_count].password, password);
    user_count++;
    printf("Kullanıcı başarıyla kaydedildi.\n");
    return true;
}

bool authenticate_user(const char *username, const char *password) {
    for (int i = 0; i < user_count; i++) {
        if (strcmp(users[i].username, username) == 0 && strcmp(users[i].password, password) == 0) {
            printf("Kullanıcı doğrulandı.\n");
            return true;
        }
    }

    printf("Kullanıcı doğrulama başarısız.\n");
    return false;
}