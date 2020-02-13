#include "socket.h"

#include <string.h>
#include <stdint.h>
#include <stddef.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "config.h"
#include "hash-server-error.h"

error_t socket_open(socket_t *sock) {
    *sock = socket(PF_INET, SOCK_STREAM, 0);
    return SUCC_OR_ERR(*sock != -1, E_SOCKET_OPEN) ;
}

error_t socket_close(socket_t st) {
    int rc;
    rc = close(st);
    return SUCC_OR_ERR(rc != -1, E_SOCKET_CLOSE);
}

error_t socket_init_server(socket_t sock, uint16_t port, const char *host) {
    int rc;
    struct sockaddr_in addr;
    int so_reuseaddr;

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(host);
    memset(addr.sin_zero, '\0', sizeof(addr.sin_zero));

    // Fix TIME_WAIT issue.
    // See http://alas.matf.bg.ac.rs/manuals/lspe/snode=104.html
    so_reuseaddr = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR,
               &so_reuseaddr, sizeof(so_reuseaddr));

    rc = bind(sock, (struct sockaddr *) &addr, sizeof(struct sockaddr_in));
    if (rc == -1) {
        return E_SOCKET_BIND;
    }

    rc = listen(sock, MAX_PARALLEL_CONN_COUNT);
    if (rc == -1) {
        return E_SOCKET_LISTEN;
    }

    return SUCCESS;
}

error_t socket_get_client(socket_t serv_sock, socket_t *client_sock) {
    struct sockaddr_storage storage;
    socklen_t addr_size = sizeof(struct sockaddr_storage);

    *client_sock = accept(serv_sock, (struct sockaddr *) &storage, &addr_size);

    return SUCC_OR_ERR(*client_sock != -1, E_SOCKET_ACCEPT);
}

error_t socket_receive_data(socket_t sock, char *buff, size_t buff_size,
                            size_t *buf_actual_size)
{
    ssize_t received_byte_count;
    received_byte_count = recv(sock , buff , buff_size , 0);
    if (received_byte_count == -1) {
        return E_SOCKET_RECEIVE;
    }
    else if (received_byte_count == 0) {
        return E_SOCKET_NOT_AVAILABLE;
    }
    *buf_actual_size = received_byte_count;
    return SUCC_OR_ERR(received_byte_count != -1, E_SOCKET_RECEIVE);
}

error_t socket_send_data(socket_t sock, const char *buff, size_t buff_size) {
    ssize_t sent_byte_count;
    sent_byte_count = send(sock, buff, buff_size, 0);
    return SUCC_OR_ERR(sent_byte_count != -1, E_SOCKET_SEND);
}
