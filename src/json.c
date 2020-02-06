#include "json.h"

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "hash-server-error.h"

error_t json_decode_single_pair(char *str, const char *key,
                                char *val, size_t val_size) { 
    error_t err;
    TRY(parse_whitespace(&str));
    TRY(parse_symbol(&str, '{'));

    TRY(parse_whitespace(&str));
    TRY(parse_text(&str, val, val_size)); // Use val as a buffer? Is it good?
    if (strcmp(val, key) != 0) {
        return E_JSON_BAD_FORMAT;
    }
    TRY(parse_whitespace(&str));
    TRY(parse_symbol(&str, ':'));

    TRY(parse_whitespace(&str));
    TRY(parse_text(&str, val, val_size));
    TRY(parse_whitespace(&str));
    TRY(parse_comma_if_presented(&str));

    TRY(parse_whitespace(&str));
    TRY(parse_symbol(&str, '}'));

    return err;
}

error_t parse_whitespace(char **str) {
    char c;
    while (**str == ' ' || **str == '\t' || **str == '\r' || **str == '\n') {
        *str += 1;
    }
    return SUCCESS;
}

error_t parse_symbol(char **str, char c) {
    if (**str != c) {
        return E_JSON_BAD_FORMAT;
    }
    return SUCCESS;
}

error_t parse_text(char **str, char *val, size_t val_size) {
    error_t err;
    size_t i;
    bool is_escaped;
    char c;

    parse_symbol(str, '"');
    
    i = 0;
    is_escaped = false;
    while (i < val_size) {
        if (**str == '\0') {
            return E_JSON_BAD_FORMAT;
        }

        if (!is_escaped && **str == '"') {
            return SUCCESS;
        }

        if (!is_escaped && **str == '\\') {
            // Don't add escape symbol.
            is_escaped = true;
        }
        else {
            c = **str;
            if (is_escaped) {
                err = get_escaped_symbol(**str, &c);
                if (FAIL(err)) {
                    return err;
                }
            }
            val[i] = c;
            is_escaped = false;
        }
        *str += 1;
        i++;
    }
    return E_JSON_DECODE_RANGE;
}

// See: https://www.json.org
//
error_t get_escaped_symbol(char sym, char *esc) {
    switch (sym)
    {
        case '"':
        case '\\':
            *esc = '\\';
            break;
        case '/':
            *esc = '/';
            break;
        case 'b':
            *esc = '\b';
            break;
        case 'f':
            *esc = '\f';
            break;
        case 'n':
            *esc = '\n';
            break;
        case 'r':
            *esc = '\r';
            break;
        case 't':
            *esc = '\t';
            break;
        case 'u':
            // We dont support Unicode at all.
            return E_JSON_BAD_FORMAT;
        default:
            // *esc = sym;
            // But by Specification it must occur an error.
            return E_JSON_BAD_FORMAT;
    }
    return SUCCESS;
}

error_t parse_comma_if_presented(char **str) {
    if (**str == ',') {
        *str += 1;
    }
    return SUCCESS;
}

error_t json_encode_single_pair(char *str, size_t str_size,
                                const char *key, const char *data) {
    return E_NOT_IMPLEMENTED;
}
