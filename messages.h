// messages.h
#ifndef MESSAGES_H
#define MESSAGES_H

typedef struct {
    char sender[50];
    char receiver[50];
    char message[256];
} Message;

void send_message(Message msg);
void list_friends(char* username);

#endif // MESSAGES_H
