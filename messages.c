#include "messages.h"
#include "friends.h"
#include <string.h>
#include <stdio.h>

// Mesajlar için basit bir veritabanı yapısı
typedef struct {
    char sender[50];
    char receiver[50];
    char message[256];
} MessageDB;

MessageDB messages[1000]; // Maksimum 1000 mesaj
int message_count = 0;

void send_message(Message msg) {
    // Mesajı veritabanına kaydet
    if (message_count < 1000) {
        strcpy(messages[message_count].sender, msg.sender);
        strcpy(messages[message_count].receiver, msg.receiver);
        strcpy(messages[message_count].message, msg.message);
        message_count++;
        printf("Message sent successfully!\n");
    } else {
        printf("Message storage is full!\n");
    }
}

void list_friends(char* username) {
    // Kullanıcının arkadaş listesini gösterme
    UserFriends* user = find_user_friends(username);
    if (user == NULL) {
        printf("User not found!\n");
        return;
    }

    printf("Friends of %s:\n", username);
    for (int i = 0; i < user->friend_count; i++) {
        printf("- %s\n", user->friends[i]);
    }
}
