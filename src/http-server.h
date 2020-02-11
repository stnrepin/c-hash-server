#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include <stdint.h>
#include <stdbool.h>

#include "socket.h"
#include "hash-server-error.h"
#include "http-primitives.h"
#include "route-list.h"

typedef void (*HttpServerOnStartCallback)();

typedef struct {
    bool is_listening;
    socket_t serv_sock;
    RouteList *routes;
} HttpServer;

typedef struct {
    socket_t client_socket;
    HttpServer *server;
} ClientHandlerArgs;

void HttpServer_init(HttpServer *srv);
void HttpServer_deinit(HttpServer *srv);
void HttpServer_post(HttpServer *srv, const char *path, RouteCallback rc);
void HttpServer_use(HttpServer *srv, RouteCallback rc);
error_t HttpServer_listen(HttpServer *srv, uint16_t port, const char *host,
                          HttpServerOnStartCallback ic);
void HttpServer_handler_client_wrapper(ClientHandlerArgs *args);
error_t Http_server_handler_client(const HttpServer *srv,
                                   socket_t client_socket);
error_t HttpServer_handle_request(const HttpServer *serv, error_t err,
                                  Request *req, Response *res);
#endif // !HTTP_SERVER_H
