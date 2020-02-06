#ifndef JSON_H
#define JSON_H

/*
 * This is ugly solution. 
 * 
 * Currently parser can parse *only* json strings in format:
 * 
 *     {
 *         "key": "data",
 *     }
 * 
 * Or, more precisely, grammar:
 *     <JSON> ::= {<space>"<text>"<space>:<space>"<text>"
 *                      <space><comma_or_empty><space>}
 * where <space> = { ' ', '\t', '\n', '\r' } (from https://www.json.org/)
 */ 

#include <stdlib.h>

#include "hash-server-error.h"

static error_t parse_whitespace(char **str);
static error_t parse_text(char **str, char *val, size_t val_size);
static error_t get_escaped_symbol(char sym, char *esc);
static error_t parse_comma_if_presented(char **str);
static error_t parse_symbol(char **str, char sym);

error_t json_decode_single_pair(/*const*/ char *str, const char *key,
                                char *val, size_t val_size);

error_t json_encode_single_pair(char *str, size_t str_size,
                                const char *key, const char *data);

#endif // !JSON_H
