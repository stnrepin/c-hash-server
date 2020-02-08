#include "http-server.h"

#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <stdint.h>

#include "socket.h"
#include "json.h"
#include "hash-server-error.h"

void HttpServer_init(HttpServer *srv) {
    srv->is_listening = false;
    srv->serv_sock = -1;
    srv->routes = RouteList_new();
}

void HttpServer_deinit(HttpServer *srv) {
    error_t err;

    if (srv->is_listening) {
        err = socket_close(srv->serv_sock);
        if (FAIL(err)) {
            print_error(err);
        }
    }
    RouteList_free(srv->routes);
}

void HttpServer_post(HttpServer *srv, const char *path, RouteCallback rc) {
    Route r;
    Route_init(&r, path, METHOD_POST, rc);
    RouteList_add(srv->routes, &r);
}

void HttpServer_use(HttpServer *srv, RouteCallback rc) {
    Route r;
    Route_init(&r, "*", METHOD_ALL, rc);
    RouteList_add(srv->routes, &r);
}

error_t HttpServer_listen(HttpServer *srv, uint16_t port, const char *host,
                          HttpServerOnStartCallback ic) {
    error_t err;
    TRY(socket_open(&srv->serv_sock));

    TRY(socket_init_server(srv->serv_sock, port, host));

    ic();

    while(1) {
        char data[2000];
        socket_t client_socket;
        err = socket_get_client(srv->serv_sock, &client_socket);
        if (FAIL(err)) {
            print_error(err);
            goto out_client; // That's the first time I use goto!
        }

        err = socket_receive_data(client_socket, data, 2000);
        if (FAIL(err)) {
            print_error(err);
            goto out_client;
        }

        printf("Received:\n %s\n", data);

        err = socket_send_data(client_socket, data, 2000);
        if (FAIL(err)) {
            print_error(err);
            goto out_client;
        }

out_client:
        err = socket_close(client_socket);
        if (FAIL(err)) {
            print_error(err);
        }
    }
    // Unreachable.
    return SUCCESS;
}