#ifndef AUTH_H
#define AUTH_H

typedef struct {
    char username[50];
    char password[65];
} UserDB;

typedef struct {
    char username[50];
    char password[65];
} User;

// Kullanıcıları ve parolaları dosyaya kaydetme/yükleme işlevleri
UserDB* get_all_users();
int get_user_count();


void load_users_from_file();
void save_users_to_file();

// Parola hash'leme işlevi
void hash_password(const char *password, char *hash);

// Kullanıcı kayıt ve giriş işlevleri
char* register_user(User user);
char* login_user(User user);
void print_users();
#endif // AUTH_H
