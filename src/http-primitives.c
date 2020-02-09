#include "http-primitives.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include <hash-server-error.h>

void Response_init(Response *res) {
    res->data = NULL;
    res->code = HTTP_CODE_NO_CONTENT;
    res->data_size = 0;
    res->cont_type = CONTENT_TYPE_PLAIN_TEXT;
    res->is_end = false;
}

void Response_deinit(Response *res) {
    free(res->data);
}

error_t Response_to_str(Response *res, char *str, size_t max_str_size,
                        size_t *actual_str_size)
{
    int written;
    written = snprintf(str, max_str_size-1, "HTTP/1.0 %d %s\r\n\r\n", res->code, get_phrase_by_code(res->code));
    if (written + res->data_size > actual_str_size) {
        return E_RESPONSE_RANGE;
    }
    memcpy(str+written, res->data, actual_str_size-written-1);
    return SUCCESS;
}

void Response_send(Response *res, const char *data, size_t sz) {
    res->data_size = sz;
    res->data = malloc(sz*sizeof(char));
    if (res->data == NULL) {
        panic(E_ALLOC);
    }
    memcpy(res->data, data, sz);

    Response_end(res, HTTP_CODE_OK);
}

void Response_set_content_type(Response *res, ContentType ct) {
    res->cont_type = ct;
}

void Response_end(Response *res, HttpCode code) {
    res->code = code;
    res->is_end = true;
}

void Request_init(Request *req, const char *path, RequestMethod m, ContentType ct, const char *data,
                  size_t data_size)
{
    req->path = path; // Maybe malloc or static array?
    req->method = m;
    req->cont_type = ct;
    req->data_size = data_size;
    req->data = malloc(data_size*sizeof(char));
    if (req->data == NULL) {
        panic(E_ALLOC);
    }

    memcpy(req->data, data, data_size);
}

error_t Request_init_from_str(Request *req, const char *str) {

}

void Request_deinit(Request *req) {
    free(req->data);
}

void *Route_init(Route *r, const char *path, RequestMethod rm, RouteCallback rc) {
    r->path = malloc((strlen(path) + 1) * sizeof(char));
    r->req_meth = rm;
    r->rc = rc;

    strcpy(r->path, path);
}

void Route_deinit(Route *r) {
    if (r == NULL) {
        return;
    }
    free(r->path);
    free(r);
}

bool Route_satisfies_path(Route *r, const char *path, RequestMethod rm) {
    if (strcmp(r->path, "*") != 0 && strcmp(r->path, path) != 0) {
        return false;
    }

    if (r->req_meth != METHOD_ALL && r->req_meth != rm) {
        return false;
    }

    return true;
}
