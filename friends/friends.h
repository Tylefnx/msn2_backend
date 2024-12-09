#ifndef FRIENDS_H
#define FRIENDS_H
#include "../messages/messages.h"

typedef struct {
    char requester[50];
    char requestee[50];
    int status; // 0: Beklemede, 1: Kabul edildi, 2: Reddedildi
} FriendRequest;

extern FriendRequest friend_requests[100];
extern int friend_request_count;

typedef struct {
    char sender[50];
    char receiver[50];
    MessageDB messages[1000];
    int messages_count; // Toplam mesaj sayısı
} ChatDB;

extern ChatDB chat_db[100];
extern int chat_db_count;

typedef struct {
    char username[50];
    char friends[100][50]; // Maksimum 100 arkadaş
    int friend_count;
} UserFriends;

extern UserFriends user_friends[100];
extern int user_friends_count;

void save_friends_to_file();
void load_friends_from_file();
void save_chat_db_to_file();

void add_friend_to_user(char* username, char* friend_username);

char* respond_friend_request(char* requestee, char* requester, int response);
char* add_friend_request(char* requester, char* requestee);
void remove_friend(char* username, char* friend_username);
void remove_friend_request(int index);
UserFriends* find_user_friends(const char* username);
char* list_friend_requests();
#endif // FRIENDS_H
