#include "friends.h"
#include "messages.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <json-c/json.h>

FriendRequest friend_requests[100];
int friend_request_count = 0;

UserFriends user_friends[100]; // Maksimum 100 kullanıcı
int user_friends_count = 0;

ChatDB chat_db[100]; // Maksimum 100 sohbet
int chat_db_count = 0;

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

void save_chat_db_to_file() {
    FILE *file = fopen("chat_db.txt", "w");
    if (file == NULL) {
        perror("Failed to open file for saving chat database");
        return;
    }
    for (int i = 0; i < chat_db_count; i++) {
        fprintf(file, "%s %s %d", chat_db[i].sender, chat_db[i].receiver, chat_db[i].messages_count);
        for (int j = 0; j < chat_db[i].messages_count; j++) {
            fprintf(file, " %s %s %s", chat_db[i].messages[j].sender, chat_db[i].messages[j].receiver, chat_db[i].messages[j].message);
        }
        fprintf(file, "\n");
    }
    fclose(file);
}

void load_chat_db_from_file() {
    FILE *file = fopen("chat_db.txt", "r");
    if (file == NULL) {
        perror("Failed to open file for loading chat database");
        return;
    }
    chat_db_count = 0;
    while (fscanf(file, "%s %s %d", chat_db[chat_db_count].sender, chat_db[chat_db_count].receiver, &chat_db[chat_db_count].messages_count) == 3) {
        for (int i = 0; i < chat_db[chat_db_count].messages_count; i++) {
            if (fscanf(file, "%s %s %[^\n]", chat_db[chat_db_count].messages[i].sender, chat_db[chat_db_count].messages[i].receiver, chat_db[chat_db_count].messages[i].message) != 3) {
                perror("Failed to read chat message");
                fclose(file);
                return;
            }
        }
        chat_db_count++;
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

char* add_friend_request(char* requester, char* requestee) {
    // Kullanıcıların zaten arkadaş olup olmadığını kontrol et
    UserFriends* user_friends = find_user_friends(requester);
    if (user_friends != NULL) {
        for (int i = 0; i < user_friends->friend_count; i++) {
            if (strcmp(user_friends->friends[i], requestee) == 0) {
                return "You are already friends!";
            }
        }
    }

    // Daha önce arkadaşlık isteği gönderilip gönderilmediğini kontrol et
    for (int i = 0; i < friend_request_count; i++) {
        if (strcmp(friend_requests[i].requester, requester) == 0 &&
            strcmp(friend_requests[i].requestee, requestee) == 0) {
            return "Friend request already sent!";
        }
    }

    // Yeni arkadaşlık isteği ekle
    strncpy(friend_requests[friend_request_count].requester, requester, sizeof(friend_requests[friend_request_count].requester) - 1);
    strncpy(friend_requests[friend_request_count].requestee, requestee, sizeof(friend_requests[friend_request_count].requestee) - 1);
    friend_requests[friend_request_count].status = 0;
    friend_request_count++;
    return "Friend request sent!";
}

char* respond_friend_request(char* requestee, char* requester, int response) {
    for (int i = 0; i < friend_request_count; i++) {
        if (strcmp(friend_requests[i].requester, requester) == 0 &&
            strcmp(friend_requests[i].requestee, requestee) == 0) {
            if (response == 1) {
                friend_requests[i].status = 1;
                add_friend_to_user(requester, requestee);
                add_friend_to_user(requestee, requester);

                // Kullanıcılar arasında zaten bir sohbet olup olmadığını kontrol et
                int chat_exists = 0;
                for (int j = 0; j < chat_db_count; j++) {
                    if ((strcmp(chat_db[j].sender, requester) == 0 && strcmp(chat_db[j].receiver, requestee) == 0) ||
                        (strcmp(chat_db[j].sender, requestee) == 0 && strcmp(chat_db[j].receiver, requester) == 0)) {
                        chat_exists = 1;
                        break;
                    }
                }

                // Sohbet yoksa yeni sohbet oluştur
                if (!chat_exists) {
                    strcpy(chat_db[chat_db_count].sender, requester);
                    strcpy(chat_db[chat_db_count].receiver, requestee);
                    chat_db[chat_db_count].messages_count = 0;
                    chat_db_count++;
                    save_chat_db_to_file();
                }

                return "Friend request accepted!";
            } else if (response == 2) {
                friend_requests[i].status = 2;
                return "Friend request declined!";
            }
        }
    }
    return "No friend request found!";
}


void add_friend_to_user(char* username, char* friend_username) {
    UserFriends* user = find_user_friends(username);
    if (user == NULL) {
        strcpy(user_friends[user_friends_count].username, username);
        user_friends[user_friends_count].friend_count = 0;
        user = &user_friends[user_friends_count];
        user_friends_count++;
    }
    strcpy(user->friends[user->friend_count], friend_username);
    user->friend_count++;
    save_friends_to_file();

    // ChatDB yapısını oluştur
    strcpy(chat_db[chat_db_count].sender, username);
    strcpy(chat_db[chat_db_count].receiver, friend_username);
    chat_db[chat_db_count].messages_count = 0;
    chat_db_count++;
    save_chat_db_to_file();
}

void remove_friend(char* username, char* friend_username) {
    UserFriends* user = find_user_friends(username);
    if (user == NULL) {
        printf("User not found!\n");
        return;
    }
    int found = 0;
    for (int i = 0; i < user->friend_count; i++) {
        if (strcmp(user->friends[i], friend_username) == 0) {
            found = 1;
            for (int j = i; j < user->friend_count - 1; j++) {
                strcpy(user->friends[j], user->friends[j + 1]);
            }
            user->friend_count--;
            break;
        }
    }
    if (found) {
        save_friends_to_file();
        printf("Friend removed successfully!\n");
    } else {
        printf("Friend not found!\n");
    }
}

char* list_friend_requests() {
    struct json_object *json_response = json_object_new_object();
    struct json_object *requests_array = json_object_new_array();

    for (int i = 0; i < friend_request_count; i++) {
        struct json_object *request_obj = json_object_new_object();
        json_object_object_add(request_obj, "requester", json_object_new_string(friend_requests[i].requester));
        json_object_object_add(request_obj, "requestee", json_object_new_string(friend_requests[i].requestee));
        json_object_object_add(request_obj, "status", json_object_new_int(friend_requests[i].status));
        json_object_array_add(requests_array, request_obj);
    }

    json_object_object_add(json_response, "friend_requests", requests_array);
    const char* response_str = json_object_to_json_string(json_response);
    char* response = strdup(response_str);
    json_object_put(json_response);  // JSON nesnesini serbest bırak
    return response;
}