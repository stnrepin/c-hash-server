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
    char *data, // The length of a request body can be variable so use heap.
         hash[SHA512_DIGEST_LENGTH],
         out[2+6+3+SHA512_DIGEST_LENGTH+2+1], // 2 - {", 6 - sha512,
                                              // 3 - ":", 2 - "}, 1 - \0.
         hash_str[513];

    if (FAIL(err)) {
        return err;
    }

    printf("Request to '/'");

    data = Request_get_body(req);
    printf("Data: %s\n", data);

    SHA512(data, strlen(data), hash);

    for (int k = 0; k < SHA512_DIGEST_LENGTH; k++) {
        to_hex_str(hash[k], hash_str+k*2);
    }

    TRY(json_encode_single_pair(out, 2+6+3+SHA512_DIGEST_LENGTH*2+2+1,
            "sha512", hash_str));

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
