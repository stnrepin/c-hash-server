#ifndef HASH_SERVER_ERROR_H
#define HASH_SERVER_ERROR_H

#define SUCCESS (error_t)0
#define FAIL(e) (e != SUCCESS)
#define SUCC(e) (e == SUCCESS)

typedef enum {
    E_ALLOC = 1,
} error_t;

/*
 * Note that returned string is static.
 */
const char *get_error_message(error_t err);

void print_error(error_t err);

void panic(error_t err);

/*
 * Prints with format "{prefix}{get_error_message(err)}"
 */
void print_error_internal(const char *prefix, error_t err);

#endif // !HASH_SERVER_ERROR_H
