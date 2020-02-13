#ifndef HASH_SERVER_ERROR_H
#define HASH_SERVER_ERROR_H

#define SUCCESS (error_t)0
#define FAIL(e) (e != SUCCESS)
#define SUCC(e) (e == SUCCESS)
#define SUCC_OR_ERR(cond, err) ((cond) ? SUCCESS : err)

#define TRY(func_call) { \
    err = func_call;     \
    if (FAIL(err)) {     \
        return err;      \
    }                    \
}

#define ERROR_MESSAGE_STRING_FORMAT "Error: %s. Code: %d\n"
#define ERROR_PANIC_MESSAGE_STRING_FORMAT "Fatal error: %s. Code: %d\n"

typedef enum {
    E_NOT_IMPLEMENTED = 1,
    E_ALLOC = 2,

    E_SERVER_HANDLER_NOT_FOUND = 101,

    E_SOCKET_OPEN = 201,
    E_SOCKET_CLOSE = 202,
    E_SOCKET_BIND = 203,
    E_SOCKET_LISTEN = 204,
    E_SOCKET_ACCEPT = 205,
    E_SOCKET_RECEIVE = 206,
    E_SOCKET_SEND = 207,
    E_SOCKET_NOT_AVAILABLE = 208,

    E_JSON_BAD_FORMAT = 301,
    E_JSON_DECODE_RANGE = 302,
    E_JSON_ENCODE_RANGE = 303,

    E_RESPONSE_RANGE = 501,

    E_REQUEST_BAD_FORMAT = 601,

    E_ROUTE_CONTENT_TYPE = 701,
} error_t;

/*
 * Note that returned string is static.
 */
const char *get_error_message(error_t err);

void print_error(error_t err);

void panic(error_t err);

void print_error_internal(const char *format, error_t err);

#endif // !HASH_SERVER_ERROR_H
