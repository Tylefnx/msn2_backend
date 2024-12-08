#ifndef AUTH_H
#define AUTH_H

typedef struct {
    char username[50];
    char password[50];
} UserDB;

typedef struct {
    char username[50];
    char password[50];
} User;

void load_users_from_file();
void save_users_to_file();
char* register_user(User user);
char* login_user(User user);

#endif // AUTH_H
