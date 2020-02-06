#include "hash-server-error.h"

#include <stdio.h>
#include <stdlib.h>

void print_error_internal(const char *format, error_t err) {
    fprintf(stderr, format, get_error_message(err), (int)err);
}

void print_error(error_t err) {
    print_error_internal(ERROR_MESSAGE_STRING_FORMAT, err);
}

void panic(error_t err) {
    print_error_internal(ERROR_PANIC_MESSAGE_STRING_FORMAT, err);
    exit((int)err);
}

const char *get_error_message(error_t err) {
    return "<no message>";
}
