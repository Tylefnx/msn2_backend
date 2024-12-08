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

// Kullanıcıları ve parolaları dosyaya kaydetme/yükleme işlevleri
void load_users_from_file();
void save_users_to_file();

// Parola hash'leme işlevi
void hash_password(const char *password, char *hash);

// Kullanıcı kayıt ve giriş işlevleri
char* register_user(User user);
char* login_user(User user);

#endif // AUTH_H
