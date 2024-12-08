#include "friends.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

// Arkadaş listesi için basit bir yapı
UserFriends user_friends[100]; // Maksimum 100 kullanıcı
int user_friends_count = 0;

// Arkadaş listesini dosyaya kaydetme fonksiyonu
void save_friends_to_file() {
    FILE *file = fopen("friends.txt", "w");
    if (file == NULL) {
        perror("Failed to open file for saving friends");
        return;
    }
    for (int i = 0; i < user_friends_count; i++) {
        fprintf(file, "%s %d", user_friends[i].username, user_friends[i].friend_count);
        for (int j = 0; j < user_friends[i].friend_count; j++) {
            fprintf(file, " %s", user_friends[i].friends[j]);
        }
        fprintf(file, "\n");
    }
    fclose(file);
}

// Dosyadan arkadaş listesini yükleme fonksiyonu
void load_friends_from_file() {
    FILE *file = fopen("friends.txt", "r");
    if (file == NULL) {
        perror("Failed to open file for loading friends");
        return;
    }
    user_friends_count = 0;
    while (fscanf(file, "%s %d", user_friends[user_friends_count].username, &user_friends[user_friends_count].friend_count) == 2) {
        for (int i = 0; i < user_friends[user_friends_count].friend_count; i++) {
            if (fscanf(file, " %s", user_friends[user_friends_count].friends[i]) != 1) {
                perror("Failed to read friend's name");
                fclose(file);
                return;
            }
        }
        user_friends_count++;
    }
    fclose(file);
}

UserFriends* find_user_friends(const char* username) {
    for (int i = 0; i < user_friends_count; i++) {
        if (strcmp(user_friends[i].username, username) == 0) {
            return &user_friends[i];
        }
    }
    return NULL;
}

void add_friend(FriendRequest request) {
    // Kullanıcıyı bul veya oluştur
    UserFriends* user = find_user_friends(request.username);
    if (user == NULL) {
        strcpy(user_friends[user_friends_count].username, request.username);
        user_friends[user_friends_count].friend_count = 0;
        user = &user_friends[user_friends_count];
        user_friends_count++;
    }

    // Arkadaş ekle
    for (int i = 0; i < user->friend_count; i++) {
        if (strcmp(user->friends[i], request.friend_username) == 0) {
            printf("Already friends!\n");
            return;
        }
    }

    strcpy(user->friends[user->friend_count], request.friend_username);
    user->friend_count++;
    save_friends_to_file(); // Arkadaş listesini dosyaya kaydet
    printf("Friend added successfully!\n");
}

void remove_friend(FriendRequest request) {
    // Kullanıcıyı bul
    UserFriends* user = find_user_friends(request.username);
    if (user == NULL) {
        printf("User not found!\n");
        return;
    }

    // Arkadaş çıkar
    int found = 0;
    for (int i = 0; i < user->friend_count; i++) {
        if (strcmp(user->friends[i], request.friend_username) == 0) {
            found = 1;
            for (int j = i; j < user->friend_count - 1; j++) {
                strcpy(user->friends[j], user->friends[j + 1]);
            }
            user->friend_count--;
            break;
        }
    }

    if (found) {
        save_friends_to_file(); // Arkadaş listesini dosyaya kaydet
        printf("Friend removed successfully!\n");
    } else {
        printf("Friend not found!\n");
    }
}
