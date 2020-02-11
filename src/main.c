#include "main.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <signal.h>
#include <openssl/sha.h>
#include "gosthash.h"

#include "socket.h"
#include "http-server.h"
#include "http-primitives.h"
#include "hash-server-error.h"
#include "config.h"
#include "json.h"

void to_hex_str(unsigned char num, char *str) {
    // Compiler do replace % and / by binary opetations. Checked.
    //
    static const char HEX[16] = { '0', '1', '2', '3', '4', '5', '6', '7',
                                  '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };
    // Each byte -- 2 symbols.
    for (int i = 0; i < 2; i++) {
        str[1-i] = HEX[num % 16];
        num /= 16;
    }
}

void get_sha512_as_str(const char *d, size_t ds, char *hash_str) {
    unsigned char hash[SHA512_DIGEST_LENGTH];

    SHA512(d, ds, hash);
    for (int k = 0; k < SHA512_DIGEST_LENGTH; k++) {
        to_hex_str(hash[k], hash_str+k*2);
    }
}

void get_gost_as_str(const char *d, size_t ds, char *hash_str) {
    unsigned char hash[GOST_DIGEST_LENGTH];

    gost_hash_ctx gost_ctx;
    init_gost_hash_ctx(&gost_ctx, &GostR3411_94_CryptoProParamSet);
    start_hash(&gost_ctx);
    hash_block(&gost_ctx, d, ds);
    finish_hash(&gost_ctx, hash);
    // See main() in gostsum.c
    for (int i = 0; i < 32; i++) {
        sprintf(hash_str + 2 * i, "%02x", hash[31 - i]);
    }
}

error_t post_root_route(error_t err, Request *req, Response *res) {
    const char DATA_KEY_NAME[] = "data";
    const char GOST_KEY_NAME[] = "gost";
    const char SHA512_KEY_NAME[] = "sha512";
    const int HASH_SHA512_STR_LEN = SHA512_DIGEST_LENGTH*2;
    const int HASH_GOST_STR_LEN = GOST_DIGEST_LENGTH*2;
    const int JSON_OUT_STR_SIZE = JSON_STRING_SIZE_FOR_TWO_PAIRS(
                                    sizeof(GOST_KEY_NAME), HASH_SHA512_STR_LEN,
                                    sizeof(SHA512_KEY_NAME), HASH_GOST_STR_LEN);
    const int JSON_IN_DATA_STR_SIZE = 1024+1;

    char data[JSON_OUT_STR_SIZE],
         out[JSON_OUT_STR_SIZE],
         gost_hash_str[HASH_GOST_STR_LEN + 1],
         sha512_hash_str[HASH_SHA512_STR_LEN + 1];

    if (FAIL(err)) {
        return err;
    }

    IF_DEBUG(printf("Request to '/' with %s\n", req->data));

    if (req->cont_type != CONTENT_TYPE_APPLICATION_JSON) {
        return E_ROUTE_CONTENT_TYPE;
    }

    TRY(json_decode_single_pair(req->data, DATA_KEY_NAME,
            data, JSON_IN_DATA_STR_SIZE));

    size_t data_len = strlen(data);
    get_sha512_as_str(data, data_len, sha512_hash_str);
    get_gost_as_str(data, data_len, gost_hash_str);

    TRY(json_encode_single_pair(out, sizeof(out),
            SHA512_KEY_NAME, sha512_hash_str));
    /*
     * out+out_len-1 points to '}' (last) symbol in out
     * So there are the following transformations:
     *      {<pair1_json>} ->
     *      {<pair1_json>{<pair2_json>} ->
     *      {<pair1_json>,<pair2_json>}
     */
    size_t out_len = strlen(out);
    TRY(json_encode_single_pair(out+out_len-1, sizeof(out)-out_len,
            GOST_KEY_NAME, gost_hash_str))
    out[out_len-1] = ',';

    Response_set_content_type(res, CONTENT_TYPE_APPLICATION_JSON);
    Response_send(res, out, strlen(out)+1);

    IF_DEBUG(printf("\n"));

    return SUCCESS;
}

error_t not_found_route(error_t err, Request *req, Response *res) {
    if (FAIL(err)) {
        return err;
    }
    printf("404 at %s to %s\n\n", RequestMethod_to_str(req->method), req->path);
    Response_end(res, HTTP_CODE_NOT_FOUND);
    return SUCCESS;
}

error_t internal_error_route(error_t err, Request *req, Response *res) {
    if(SUCC(err)) {
        printf("internal_error_route called but there is no error\n");
    }
    else {
        printf("Internal server\n");
        print_error(err);
    }
    printf("\n");
    Response_end(res, HTTP_CODE_INTERNAL_ERROR);
    return SUCCESS;
}

void server_start() {
    printf("Server listening on %s:%d\n\n", SERVER_HOST, SERVER_PORT);
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

    return (int)err;
}
