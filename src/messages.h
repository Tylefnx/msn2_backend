#ifndef MESSAGES_H
#define MESSAGES_H

#include <stddef.h> 
#include <microhttpd.h> 

int handle_send_message(struct MHD_Connection *connection, const char *upload_data, size_t upload_data_size);
int handle_add_friend(struct MHD_Connection *connection, const char *upload_data, size_t upload_data_size);

#endif // MESSAGES_H
