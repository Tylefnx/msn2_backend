#include "messages.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

// Mesajlar için basit bir veritabanı yapısı
MessageDB messages[1000]; // Maksimum 1000 mesaj
int message_count = 0;

// Mesajları dosyaya kaydetme fonksiyonu
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

// Dosyadan mesajları yükleme fonksiyonu
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
        save_messages_to_file(); // Mesajları dosyaya kaydet
        printf("Message sent successfully!\n");
    } else {
        printf("Message storage is full!\n");
    }
}
