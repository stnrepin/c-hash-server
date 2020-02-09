#include "main.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <openssl/sha.h>

#include "socket.h"
#include "http-server.h"
#include "http-primitives.h"
#include "hash-server-error.h"
#include "config.h"
#include "json.h"

error_t post_root_route(error_t err, Request *req, Response *res) {
    char data[MAX_HASHING_STRING_SIZE],
         hash[SHA512_DIGEST_LENGTH+1],
         out[2+6+3+SHA512_DIGEST_LENGTH+2+1];

    if (FAIL(err)) {
        return err;
    }

    printf("Request to '/'");
    json_decode_single_pair(res->data, "data", data, MAX_HASHING_STRING_SIZE);
    SHA512(data, strlen(data), hash);
    hash[SHA512_DIGEST_LENGTH] = '\0';
    json_encode_single_pair(out, sizeof(out), "sha512", hash);

    Response_send(res, out, sizeof(out));

    return SUCCESS;
}

error_t not_found_route(error_t err, Request *req, Response *res) {
    if (FAIL(err)) {
        return err;
    }
    printf("404");
    Response_end(res, HTTP_CODE_NOT_FOUND);
    return SUCCESS;
}

error_t internal_error_route(error_t err, Request *req, Response *res) {
    if(SUCC(err)) {
        printf("internal_error_route called but there is no error");
    }
    else {
        print_error(err);
    }
    Response_end(res, HTTP_CODE_INTERNAL_ERROR);
    return SUCCESS;
}

void server_start() {
    printf("Server listening at %s:%d\n", SERVER_HOST, SERVER_PORT);
}

int main(int argc, char **argv) {
    error_t err;
    char data[MAX_HTTP_REQ_RES_BUFFER_SIZE];
    socket_t server_socket;

    HttpServer serv;

    HttpServer_init(&serv);

    HttpServer_post(&serv, "/", post_root_route);
    HttpServer_use(&serv, not_found_route);
    HttpServer_use(&serv, internal_error_route);

    err = HttpServer_listen(&serv, SERVER_PORT, SERVER_HOST, server_start);
    if (FAIL(err)) {
        print_error(err);
    }

    HttpServer_deinit(&serv);

    return 0;
}
