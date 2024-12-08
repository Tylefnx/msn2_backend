#include "messages.h"
#include "friends.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

extern ChatDB chat_db[100]; // friends.c'de tanımlanan chat_db
extern int chat_db_count;

MessageDB messages[1000];
int message_count = 0;

void save_messages_to_file() {
    FILE *file = fopen("messages.txt", "w");
    if (file == NULL) {
        perror("Failed to open file for saving messages");
        return;
    }
    for (int i = 0; i < message_count; i++) {
        fprintf(file, "%s %s %s\n", messages[i].sender, messages[i].receiver, messages[i].message);
    }
    fclose(file);
}

void load_messages_from_file() {
    FILE *file = fopen("messages.txt", "r");
    if (file == NULL) {
        perror("Failed to open file for loading messages");
        return;
    }
    message_count = 0;
    while (fscanf(file, "%s %s %[^\n]", messages[message_count].sender, messages[message_count].receiver, messages[message_count].message) == 3) {
        message_count++;
    }
    fclose(file);
}

void send_message(Message msg) {
    // Mesajı veritabanına kaydet
    if (message_count < 1000) {
        strcpy(messages[message_count].sender, msg.sender);
        strcpy(messages[message_count].receiver, msg.receiver);
        strcpy(messages[message_count].message, msg.message);
        message_count++;

        // ChatDB yapısına ekle
        for (int i = 0; i < chat_db_count; i++) {
            if ((strcmp(chat_db[i].sender, msg.sender) == 0 && strcmp(chat_db[i].receiver, msg.receiver) == 0) ||
                (strcmp(chat_db[i].sender, msg.receiver) == 0 && strcmp(chat_db[i].receiver, msg.sender) == 0)) {
                int msg_index = chat_db[i].messages_count++;
                strcpy(chat_db[i].messages[msg_index].sender, msg.sender);
                strcpy(chat_db[i].messages[msg_index].receiver, msg.receiver);
                strcpy(chat_db[i].messages[msg_index].message, msg.message);
                break;
            }
        }

        save_messages_to_file();
    } else {
        printf("Message storage is full!\n");
    }
}

void list_messages(const char* username, char* response) {
    strcpy(response, "{ \"messages\": [");
    int first = 1;
    for (int i = 0; i < chat_db_count; i++) {
        if (strcmp(chat_db[i].sender, username) == 0 || strcmp(chat_db[i].receiver, username) == 0) {
            for (int j = 0; j < chat_db[i].messages_count; j++) {
                if (!first) {
                    strcat(response, ", ");
                }
                strcat(response, "{ \"sender\": \"");
                strcat(response, chat_db[i].messages[j].sender);
                strcat(response, "\", \"receiver\": \"");
                strcat(response, chat_db[i].messages[j].receiver);
                strcat(response, "\", \"message\": \"");
                strcat(response, chat_db[i].messages[j].message);
                strcat(response, "\" }");
                first = 0;
            }
        }
    }
    strcat(response, "] }");
}
