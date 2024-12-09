#include "request_handlers.h"
#include "process_request.h"
#include <pthread.h>
#include "handle_client.h"
#include <unistd.h>

void* handle_client(void* arg) {
    client_info* client = (client_info*)arg;
    process_request(client->client_socket);
    close(client->client_socket);
    free(client);
    return NULL;
}
