#ifndef MAIN_H
#define MAIN_H

#include <stddef.h>

#include "http-primitives.h"

#ifdef DEBUG
#define IF_DEBUG(x) x
#else
#define IF_DEBUG(x)
#endif

#define GOST_DIGEST_LENGTH 32

void to_hex_str(unsigned char num, char *str);
void get_sha512_as_str(const char *d, size_t ds, char *hash_str);
void get_gost_as_str(const char *d, size_t ds, char *hash_str);

error_t post_root_route(error_t err, Request *req, Response *res);
error_t not_found_route(error_t err, Request *req, Response *res);
error_t internal_error_route(error_t err, Request *req, Response *res);
void server_start();

int main(int argc, char **argv);

#endif // !MAIN_H
