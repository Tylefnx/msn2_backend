#ifndef MESSAGES_H
#define MESSAGES_H

typedef struct {
    char sender[50];
    char receiver[50];
    char message[256];
} Message;

typedef struct {
    char sender[50];
    char receiver[50];
    char message[256];
} MessageDB;

void send_message(Message msg);
void save_messages_to_file();
void load_messages_from_file();
void list_messages(const char* username, char* response);

extern MessageDB messages[1000];
extern int message_count;

#endif // MESSAGES_H
