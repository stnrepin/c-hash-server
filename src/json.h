#ifndef JSON_H
#define JSON_H

/*
 * This is ugly solution. 
 * 
 * Currently parser can *only* work with json strings in the format:
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
#include <stdbool.h>

#include "hash-server-error.h"

// See: https://www.json.org
//
#define ESCAPE_SYMBOLS_COUNT 9
static const char ESCAPE_SYMBOLS_MAP[ESCAPE_SYMBOLS_COUNT][2] = {
    // { Symbol, Escaped symbol }
    { '"', '"' },
    { '\\', '\\'},
    { '/', '/'  },
    { '\b', 'b' },
    { '\f', 'f' },
    { '\n', 'n' },
    { '\r', 'r' },
    { '\t', 't' },
    { 'u', 'u'},    // Should generate error!
};

static error_t parse_whitespace(const char **str);
static error_t parse_text(const char **str, char *val, size_t val_size);
static error_t get_escaped_symbol(char sym, char *esc);
static error_t parse_comma_if_presented(const char **str);
static error_t parse_symbol(const char **str, char sym);

error_t json_decode_single_pair(const char *str, const char *key,
                                char *val, size_t val_size);

static error_t write_str_escaped(char *dest, size_t *dest_len, size_t dest_size,
                  const char *str);
static error_t write_str(char *dest, size_t *dest_len, size_t dest_size,
                  const char *str);
static bool is_escape(char c);
static char get_escape_part(char c);

error_t json_encode_single_pair(char *str, size_t str_size,
                                const char *key, const char *data);

#endif // !JSON_H
