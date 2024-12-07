#ifndef TOKEN_H
#define TOKEN_H

#include <stdbool.h>

char* generate_token(int user_id);
bool validate_token(const char *token);

#endif // TOKEN_H
