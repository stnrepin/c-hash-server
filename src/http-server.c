#include "http-server.h"

#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <openssl/sha.h>

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

void to_hex_str_fast(unsigned char num, char *str) {
    // Compiler do replace %, / by binary opetations. Checked.
    //
    const char HEX[16] = { '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
    for (int i = 0; i < 1; i++) {
        str[i] = HEX[num % 16];
        num /= 16;
    }
}

error_t HttpServer_listen(HttpServer *srv, uint16_t port, const char *host,
                          HttpServerOnStartCallback ic) {
    error_t err;
    TRY(socket_open(&srv->serv_sock));

    TRY(socket_init_server(srv->serv_sock, port, host));

    ic();

    while(1) {
        // Read header
        // Get len of data and path and meth
        // read data
        // Create Request
        // Create response
        // Find route
        //  execute handler (parse json, get hash, encode json)
        // If response end and SUCC(error) then send throught socket
        // find next route

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

        size_t ds = strlen(data);
        data[ds-2] = data[ds-1];
        data[ds-1] = data[ds];
        int i = 0;
        for (i = 0; i < ds; i++) {
            if (i > 1 && i < ds-1 &&
                data[i-1] == '\r' &&
                data[i] == '\n' &&
                data[i+1] == '\r')
            {
                i += 3;
                break;
            }
        }

        for (int j = 0; (j+i) < ds; j++) {
            data[j] = data[j+i];
        }
        data[i] = '\0';
        printf("Received:\n%s\n", data);

        unsigned char hash[SHA512_DIGEST_LENGTH+1];
        char data_j[MAX_HASHING_STRING_SIZE],
             out[2+6+3+SHA512_DIGEST_LENGTH+2+1];
        err = json_decode_single_pair(data, "data", data_j, MAX_HASHING_STRING_SIZE);
        if (FAIL(err)) {
            goto out_client;
        }
        printf("Data: %s\n", data_j);
        SHA512(data_j, strlen(data_j), hash);
        hash[SHA512_DIGEST_LENGTH] = '\0';
        printf("Hash: %s\n", hash);
        printf("%02x %02x %02x\n", hash[0], hash[1], hash[2]);
        char hash_str[513];
        for (int k = 0; k < 64; k++) {
            to_hex_str_fast(hash[k], hash_str+k*2);
        }
        json_encode_single_pair(out, 2+6+3+SHA512_DIGEST_LENGTH*2+2+1, "sha512", hash_str);
        if (FAIL(err)) {
            goto out_client;
        }

        err = socket_send_data(client_socket, out, sizeof(out));
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