#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include <stdint.h>

#include "hash-server-error.h"
#include "http-primitives.h"
#include "router-list.h"

typedef void (*HttpServerOnStartCallback)();

typedef struct {
    RouterList *routes;
} HttpServer;

error_t HttpServer_init(HttpServer **srv);
error_t HttpServer_free(HttpServer *srv);
error_t HttpServer_post(HttpServer *srv, const char *path, RouterCallback *rc);
error_t HttpServer_use(RouterCallback *rc);
error_t HttpServer_listen(int16_t port, HttpServerOnStartCallback);

#endif // !HTTP_SERVER_H
