#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <json-c/json.h>
#include <jwt.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "messages/messages.h"
#include "auth/auth.h"
#include "friends/friends.h"
#include "srv/request_handlers.h"
#include "srv/process_request.h"
#include "srv/handle_client.h"
#include "srv/validate_token.h"

#define PORT 8080


int main() {
    int server_fd;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    // Kullanıcı verilerini, arkadaş listesini ve mesajları dosyadan yükle
    load_users_from_file();
    print_users();
    load_friends_from_file();
    load_messages_from_file();

    // Soket oluştur
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Adres ve portu ayarla
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Soket ve adresi bağla
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Dinlemeye başla
    if (listen(server_fd, 3) < 0) {
        perror("listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Server is running on port %d...\n", PORT);

    while (1) {
        client_info* client = malloc(sizeof(client_info));
        if ((client->client_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            perror("accept failed");
            free(client);
            continue;
        }
        pthread_t client_thread;
        pthread_create(&client_thread, NULL, handle_client, (void*)client);
        pthread_detach(client_thread);  // İş parçacığını ayrıştır
    }

    return 0;
}
