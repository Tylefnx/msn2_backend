#include "friends.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

// Arkadaş listesi için basit bir yapı
UserFriends user_friends[100]; // Maksimum 100 kullanıcı
int user_friends_count = 0;

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
        printf("Friend removed successfully!\n");
    } else {
        printf("Friend not found!\n");
    }
}
