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

void HttpServer_init(HttpServer *srv);
void HttpServer_deinit(HttpServer *srv);
void HttpServer_post(HttpServer *srv, const char *path, RouteCallback rc);
void HttpServer_use(HttpServer *srv, RouteCallback rc);
error_t HttpServer_listen(HttpServer *srv, uint16_t port, const char *host,
                          HttpServerOnStartCallback ic);
error_t HttpServer_handle_request(HttpServer *serv, error_t err, Request *req,
                                  Response *res);
#endif // !HTTP_SERVER_H
