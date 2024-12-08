#ifndef FRIENDS_H
#define FRIENDS_H

typedef struct {
    char username[50];
    char friend_username[50];
} FriendRequest;

void add_friend(FriendRequest request);
void remove_friend(FriendRequest request);

typedef struct {
    char username[50];
    char friends[100][50]; // Maksimum 100 arkadaş
    int friend_count;
} UserFriends;

UserFriends* find_user_friends(const char* username);
void save_friends_to_file();
void load_friends_from_file();

// Arkadaş listesi veritabanı
extern UserFriends user_friends[100];
extern int user_friends_count;

#endif // FRIENDS_H
