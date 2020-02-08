#ifndef NET_H
#define NET_H

#include <stdint.h>
#include <stddef.h>

#include "hash-server-error.h"

typedef int socket_t;

error_t socket_open(socket_t *sock);
error_t socket_close(socket_t sock);
error_t socket_init_server(socket_t sock, uint16_t port, const char *host);
error_t socket_get_client(socket_t serv_sock, socket_t *client_sock);
error_t socket_receive_data(socket_t sock, char *buff, size_t buff_size);
error_t socket_send_data(socket_t sock, const char *buff, size_t buff_size);

#endif // !NET_H