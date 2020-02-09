#include "json.h"

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "hash-server-error.h"

error_t json_decode_single_pair(const char *str, const char *key,
                                char *val, size_t val_size)
{ 
    error_t err;
    TRY(parse_whitespace(&str));
    TRY(parse_symbol(&str, '{'));

    TRY(parse_whitespace(&str));
    TRY(parse_text(&str, val, val_size));
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
    TRY(parse_whitespace(&str));
    TRY(parse_symbol(&str, '\0'));

    return err;
}

error_t parse_whitespace(const char **str) {
    while (**str == ' ' || **str == '\t' || **str == '\r' || **str == '\n') {
        *str += 1;
    }
    return SUCCESS;
}

error_t parse_symbol(const char **str, char c) {
    if (**str != c) {
        return E_JSON_BAD_FORMAT;
    }
    *str += 1;
    return SUCCESS;
}

error_t parse_text(const char **str, char *val, size_t val_size) {
    error_t err;
    size_t i;
    bool is_escaped;
    char c;

    parse_symbol(str, '"');
    
    i = 0;
    is_escaped = false;
    // Last symbol for '\0'.
    while (i < val_size - 1) {
        if (**str == '\0') {
            return E_JSON_BAD_FORMAT;
        }

        if (!is_escaped && **str == '"') {
            *str += 1;
            val[i] = '\0';
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
            i++;
            is_escaped = false;
        }
        *str += 1;
    }
    return E_JSON_DECODE_RANGE;
}

error_t get_escaped_symbol(char sym, char *esc) {
    int i = 0;
    for (i = 0; i < ESCAPE_SYMBOLS_COUNT; i++) {
        if (ESCAPE_SYMBOLS_MAP[i][1] == sym) {
            // We dont work with Unicode.
            if (sym == 'u') {
                return E_JSON_BAD_FORMAT;
            }

            *esc = ESCAPE_SYMBOLS_MAP[i][0];
            return SUCCESS;
        }
    }
    // *esc = sym; return SUCCESS;
    // But by Specification it must occur an error.
    return E_JSON_BAD_FORMAT;
}

error_t parse_comma_if_presented(const char **str) {
    if (**str == ',') {
        *str += 1;
    }
    return SUCCESS;
}

error_t json_encode_single_pair(char *str, size_t str_size,
                                const char *key, const char *data)
{
    error_t err;
    size_t str_len;

    // Minimal condition.
    //
    const int EMPTY_JSON_PAIR_STRING_SIZE = 7 + 1;
    if (str_size < EMPTY_JSON_PAIR_STRING_SIZE + 
                   strlen(key) +
                   strlen(data))
    {
        return E_JSON_ENCODE_RANGE;
    }

    str_len = 0;
    TRY(write_str(str, &str_len, str_size, "{\""));
    TRY(write_str_escaped(str, &str_len,str_size, key));
    TRY(write_str(str, &str_len, str_size, "\":\""));
    TRY(write_str_escaped(str, &str_len, str_size, data));
    TRY(write_str(str, &str_len, str_size, "\"}"));

    return err;
}

error_t write_str(char *dest, size_t *dest_len, size_t dest_size,
                  const char *str)
{
    size_t str_len = strlen(str);
    if (dest_size - *dest_len <= str_len) {
        return E_JSON_ENCODE_RANGE;
    }

    strcpy(dest + *dest_len, str);
    *dest_len += str_len;
    return SUCCESS;
}

error_t write_str_escaped(char *dest, size_t *dest_len, size_t dest_size,
                  const char *str)
{
    size_t i;
    for (i = 0; i < strlen(str); i++) {
        if (*dest_len == dest_size) {
            return E_JSON_ENCODE_RANGE;
        }
        if (is_escape(str[i])) {
            dest[(*dest_len)++] = '\\';
            if (*dest_len == dest_size) {
                return E_JSON_ENCODE_RANGE;
            }
            dest[(*dest_len)++] = get_escape_part(str[i]);
        }
        else {
            dest[(*dest_len)++] = str[i];
        }
    }
    if (*dest_len >= dest_size) {
        return E_JSON_ENCODE_RANGE;
    }
    dest[*dest_len] = '\0';
    return SUCCESS;
}

bool is_escape(char c) {
    int i = 0;
    for (i = 0; i < ESCAPE_SYMBOLS_COUNT; i++) {
        if (ESCAPE_SYMBOLS_MAP[i][0] == c) {
            return true;
        }
    }
    return false;
}

char get_escape_part(char c) {
    int i = 0;
    for (i = 0; i < ESCAPE_SYMBOLS_COUNT; i++) {
        if (ESCAPE_SYMBOLS_MAP[i][0] == c) {
            return ESCAPE_SYMBOLS_MAP[i][1];
        }
    }
    // Should never happen.
    return '\0';
}
