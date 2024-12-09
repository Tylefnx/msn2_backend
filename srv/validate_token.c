#include <jwt.h>
#include <string.h>
#include "validate_token.h"

int validate_token(const char* token, char* username) {
    jwt_t *jwt = NULL;
    if (jwt_decode(&jwt, token, (unsigned char*)"secret_key", strlen("secret_key")) != 0) {
        return 0; // Token geçersiz
    }
    const char* sub = jwt_get_grant(jwt, "sub");
    if (sub == NULL) {
        jwt_free(jwt);
        return 0; // Token geçersiz
    }
    strcpy(username, sub);
    jwt_free(jwt);
    return 1; // Token geçerli
}