#ifndef AUTH_H
#define AUTH_H
#define PORT 8080
#define BUFFER_SIZE 1024
#include <stdbool.h>

// Kullanıcı doğrulama prototipleri

bool register_user(const char *username, const char *password);
bool login_user(const char *username, const char *password);

#endif // AUTH_H
