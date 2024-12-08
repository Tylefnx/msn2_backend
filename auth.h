// auth.h
#ifndef AUTH_H
#define AUTH_H

typedef struct {
    char username[50];
    char password[50];
} User;

char* register_user(User user);
char* login_user(User user);

#endif // AUTH_H
