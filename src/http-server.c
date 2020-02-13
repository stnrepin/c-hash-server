#include "http-server.h"

#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "thpool.h"

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
    RouteListNode *cur;

    if (srv->is_listening) {
        err = socket_close(srv->serv_sock);
        if (FAIL(err)) {
            print_error(err);
        }
        srv->is_listening = false;
    }

    cur = srv->routes->head;
    while (cur != NULL) {
        Route_free(cur->route);
        cur = cur->next;
    }
    RouteList_free(srv->routes);
}

void HttpServer_post(HttpServer *srv, const char *path, RouteCallback rc) {
    Route *r;
    r = Route_new(path, METHOD_POST, rc);
    RouteList_add(srv->routes, r);
}

void HttpServer_use(HttpServer *srv, RouteCallback rc) {
    Route *r;
    r = Route_new("*", METHOD_ALL, rc);
    RouteList_add(srv->routes, r);
}

error_t HttpServer_listen(HttpServer *srv, uint16_t port, const char *host,
                          HttpServerOnStartCallback ic) {
    error_t err;
    threadpool pool;
    int rc;

    TRY(socket_open(&srv->serv_sock));
    srv->is_listening = true;

    TRY(socket_init_server(srv->serv_sock, port, host));

    ic();

    pool = thpool_init(MAX_PARALLEL_CONN_COUNT);
    if (pool == NULL) {
        return E_THREAD_POOL;
    }
    while(1) {
        ClientHandlerArgs *args = malloc(sizeof(ClientHandlerArgs));
        args->server = srv;

        err = socket_get_client(srv->serv_sock, &args->client_socket);
        if (FAIL(err)) {
            print_error(err);
        }
        rc = thpool_add_work
                (pool, (void*)HttpServer_handle_client_wrapper, args);
        if (rc == -1) {
            panic(E_THREAD_POOL);
        }
    }

    // Unreachable.
    //
	thpool_destroy(pool);
    return SUCCESS;
}

void HttpServer_handle_client_wrapper(ClientHandlerArgs *args) {
    error_t err;
    // Note: error isn't ocurred, if client closes a connection.
    err = HttpServer_handle_client(args->server, args->client_socket);
    if (FAIL(err) && err != E_SOCKET_NOT_AVAILABLE) {
        print_error(err);
    }
    err = socket_close(args->client_socket);
    if (FAIL(err) && err != E_SOCKET_NOT_AVAILABLE) {
        print_error(err);
    }
    free(args);
}

error_t HttpServer_handle_client(const HttpServer *srv,
                                 socket_t client_socket)
{
    error_t err;
    char data[MAX_HTTP_REQUEST_SIZE]; // Used for request and response.
    size_t data_len, data_size;
    Request req;
    Response res;

    TRY(socket_receive_data(client_socket, data, MAX_HTTP_REQUEST_SIZE,
                            &data_len));
    data[data_len] = '\0';

    err = Request_init_from_str(&req, data);
    if (FAIL(err)) {
        Request_init(&req, "", METHOD_NO, CONTENT_TYPE_TEXT_PLAIN, "", 0);
    }

    Response_init(&res);

    err = HttpServer_handle_request(srv, err, &req, &res);
    if (FAIL(err)) {
        Response_deinit(&res);
        Response_init(&res);
        Response_end(&res, HTTP_CODE_INTERNAL_ERROR);
    }

    // Reuse data, so don't create yet another array.
    err = Response_to_str(&res, data, MAX_HTTP_REQUEST_SIZE, &data_size);
    if (FAIL(err)) {
        goto out_req_res; // That's the first time I use goto!
    }
    // NOTE: Do not send '\0' byte.
    err = socket_send_data(client_socket, data, data_size-1);

out_req_res:
    Request_deinit(&req);
    Response_deinit(&res);
    return err;
}

error_t HttpServer_handle_request(const HttpServer *serv, error_t err,
                                  Request *req, Response *res)
{
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
