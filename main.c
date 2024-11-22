#include <stdio.h>
#include "auth.h"

int main() {
    char username[50], password[50];

    printf("Kullanıcı adı girin: ");
    scanf("%49s", username);

    printf("Şifre girin: ");
    scanf("%49s", password);

    if (register_user(username, password)) {
        printf("Kayıt başarılı!\n");
    } else {
        printf("Kayıt başarısız.\n");
    }
}