#include "main.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "socket.h"
#include "hash-server-error.h"
#include "config.h"

int main(int argc, char **argv) {
    error_t err;
    char data[MAX_HTTP_REQ_RES_BUFFER_SIZE];
    socket_t server_socket;

    socket_open(&server_socket);
    if (FAIL(err)) {
        panic(err);
    }
    // TODO: host and port as params.
    err = socket_init_server(server_socket);
    if (FAIL(err)) {
        panic(err);
    }

    printf("Server listening at %s:%d\n", SERVER_HOST, SERVER_PORT);

    while(1) {
        socket_t client_socket;
        err = socket_get_client(server_socket, &client_socket);
        if (FAIL(err)) {
            print_error(err);
        }

        err = socket_receive_data(client_socket, data, MAX_HTTP_REQ_RES_BUFFER_SIZE);
        if (FAIL(err)) {
            print_error(err);
            // HttpServer should try to send a 500 code?
            goto out_client; // That's the first time I use goto!
        }

        printf("Received: %s\n", data);

        err = socket_send_data(client_socket, data, MAX_HTTP_REQ_RES_BUFFER_SIZE);

out_client:
        socket_close(client_socket);
    }
    socket_close(server_socket);

    return 0;
}
