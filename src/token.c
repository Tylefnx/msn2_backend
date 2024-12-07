#include "token.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Basit bir token üretme fonksiyonu. Gerçek bir uygulamada güvenli ve rastgele bir token kullanmalısınız.
char* generate_token(int user_id) {
    // Token'in geçerlilik süresi 1 saat olacak şekilde basit bir token üretelim
    time_t rawtime;
    struct tm *timeinfo;
    char* token = malloc(256);
    
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    timeinfo->tm_hour += 1; // Token 1 saat geçerli olsun
    
    // Tokeni formatlı bir şekilde oluşturuyoruz
    snprintf(token, 256, "user_%d_%ld", user_id, mktime(timeinfo));

    return token;
}

// Token doğrulama fonksiyonu
bool validate_token(const char *token) {
    // Tokeni doğrulamak için burada birkaç basit kontrol yapalım:
    // 1. Token'in geçerlilik süresi kontrol edilir.
    // 2. Token'in doğru formatta olup olmadığına bakılır.
    
    if (token == NULL || strlen(token) == 0) {
        return false;
    }

    // Basitçe token'i kullanıcı id'si ve geçerlilik süresi olarak ayırıyoruz.
    char* token_copy = strdup(token);
    char* token_user_id = strtok(token_copy, "_");
    char* token_expiry_time = strtok(NULL, "_");

    if (token_user_id == NULL || token_expiry_time == NULL) {
        free(token_copy);
        return false;
    }

    // Geçerlilik süresini kontrol et
    time_t current_time;
    time(&current_time);
    long expiry_time = atol(token_expiry_time); // Geçerlilik zamanını al
    if (current_time > expiry_time) {
        free(token_copy);
        return false;  // Token süresi dolmuş
    }

    free(token_copy);
    return true;  // Token geçerli
}
