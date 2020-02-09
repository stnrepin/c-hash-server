#include "http-server.h"

#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <openssl/sha.h>
#include <openssl/crypto.h>
#include <openssl/x509v3.h>

#include "socket.h"
#include "json.h"
#include "hash-server-error.h"
#include "config.h"

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
    // The method is big so I declair the vars near by the place they used.

    TRY(socket_open(&srv->serv_sock));

    TRY(socket_init_server(srv->serv_sock, port, host));

    ic();

    while(1) {
        char data[MAX_HTTP_REQUEST_SIZE]; // Used for request and response.
        socket_t client_socket;

        err = socket_get_client(srv->serv_sock, &client_socket);
        if (FAIL(err)) {
            print_error(err);
            goto out_client; // That's the first time I use goto!
        }

        err = socket_receive_data(client_socket, data, MAX_HTTP_REQUEST_SIZE);
        if (FAIL(err)) {
            print_error(err);
            goto out_client;
        }

        Request req;
        err = Request_init_from_str(&req, data);
        if (FAIL(err)) {
            Request_init(&req, "", METHOD_NO, CONTENT_TYPE_PLAIN_TEXT, "", 1);
        }
        Response res;
        Response_init(&res);

        err = HttpServer_handle_request(srv, err, &req, &res);
        if (FAIL(err)) {
            print_error(err);
            Response_deinit(&res);
            Response_init(&res);
            Response_end(&res, HTTP_CODE_INTERNAL_ERROR);
        }

        size_t data_size = 0;
        // Reuse data, so don't create yet another array.
        Response_to_str(&res, data, MAX_HTTP_REQUEST_SIZE, &data_size);
        err = socket_send_data(client_socket, data, data_size);
        if (FAIL(err)) {
            print_error(err);
            goto out_client;
        }

        Request_deinit(&req);
        Response_deinit(&res);

out_client:
        err = socket_close(client_socket);
        if (FAIL(err)) {
            print_error(err);
        }
    }
    // Unreachable.
    return SUCCESS;
}

error_t HttpServer_handle_request(HttpServer *serv, error_t err, Request *req, Response *res) {
    RouteListNode *cur;

    cur = serv->routes->head;
    while (cur != NULL) {
        if (Route_satisfies_path(cur->route, req->path, req->method)) {
            err = cur->route->rc(err, req, res);
        }
        if (SUCC(err) && res->is_end) {
            break;
        }
        cur = cur->next;
    }
    if (!res->is_end) {
        return E_SERVER_HANDLER_NOT_FOUND;
    }
    return err;
}
