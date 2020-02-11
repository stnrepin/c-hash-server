#include "http-primitives.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "hash-server-error.h"
#include "config.h"

const char *HttpCode_to_str(HttpCode c) {
    switch (c) {
        case HTTP_CODE_OK:
            return "OK";
        case HTTP_CODE_NOT_FOUND:
            return "Not Found";
        case HTTP_CODE_NO_CONTENT:
            return "No Content";
        case HTTP_CODE_INTERNAL_ERROR:
            return "Internal Server Error";
    }
    // Should never happen.
    return "<NOT-IMPLEMENTED>";
}

ContentType ContentType_from_str(const char *s) {
    if (strcmp(s, "application/json") == 0) {
        return CONTENT_TYPE_APPLICATION_JSON;
    }
    return CONTENT_TYPE_TEXT_PLAIN;
}

const char *ContentType_to_str(ContentType ct) {
    switch (ct) {
        case CONTENT_TYPE_TEXT_PLAIN:
            return "text/plain";
        case CONTENT_TYPE_APPLICATION_JSON:
            return "application/json";
    }
    // Should never happen.
    return "<NOT-IMPLEMENTED>";
}

void Response_init(Response *res) {
    res->data = NULL;
    res->code = HTTP_CODE_NO_CONTENT;
    res->data_size = 0;
    res->cont_type = CONTENT_TYPE_TEXT_PLAIN;
    res->is_end = false;
}

void Response_deinit(Response *res) {
    free(res->data);
}

error_t Response_to_str(Response *res, char *str, size_t max_str_size,
                        size_t *actual_str_size)
{
    int written;
    written = snprintf(str, max_str_size-1, "HTTP/1.0 %d %s\r\n",
                            res->code, HttpCode_to_str(res->code));
    if (res->data_size > 0) {
        written += snprintf(str+written, max_str_size-1-written,
                                "Content-Type: %s\r\n\r\n",
                                ContentType_to_str(res->cont_type));
    }
    if (written + res->data_size > max_str_size) {
        return E_RESPONSE_RANGE;
    }

    if (res->data_size > 0) {
        memcpy(str+written, res->data, max_str_size-written-1);
    }
    *actual_str_size = written + (res->data_size > 0 ? res->data_size : 1);
    return SUCCESS;
}

void Response_send(Response *res, const char *data, size_t sz) {
    res->data_size = sz;

    // https://stackoverflow.com/a/1073175
    //
    if (sz > 0) {
        res->data = malloc(sz*sizeof(char));
        if (res->data == NULL) {
            panic(E_ALLOC);
        }
        memcpy(res->data, data, sz);
    }

    Response_end(res, HTTP_CODE_OK);
}

void Response_set_content_type(Response *res, ContentType ct) {
    res->cont_type = ct;
}

void Response_end(Response *res, HttpCode code) {
    res->code = code;
    res->is_end = true;
}

RequestMethod RequestMethod_from_str(const char *s) {
    if (strcmp(s, "POST") == 0) {
        return METHOD_POST;
    }
    // If method is not supported.
    return METHOD_NO;
}

const char *RequestMethod_to_str(RequestMethod rm) {
    switch (rm)
    {
        case METHOD_POST:
            return "POST";
    }
    // Should never happen.
    return "<NOT-IMPLEMENTED>";
}

void Request_init(Request *req, const char *path, RequestMethod m, ContentType ct, const char *data,
                  size_t data_size)
{
    strncpy(req->path, path, MAX_URL_SIZE);
    req->method = m;
    req->cont_type = ct;
    req->data_size = data_size;

    memcpy(req->data, data, MAX_HTTP_REQUEST_SIZE);
}

error_t Request_init_from_str(Request *req, const char *str) {
    // TODO: Can I extract while (1) {...} to a method?
    char buffer[255];
    RequestMethod meth;

    int i;

    // Read method.
    //
    i = 0;
    while (1) {
        if (*str == '\0') {
            return E_REQUEST_BAD_FORMAT;
        }
        else if (*str == ' ') {
            buffer[i++] = '\0';
            req->method = RequestMethod_from_str(buffer);
            str++;
            break;
        }
        buffer[i++] = *(str++);
    }
    // Read path.
    //
    i = 0;
    while (1)
    {
        if (*str == '\0') {
            return E_REQUEST_BAD_FORMAT;
        }
        else if (*str == ' ') {
            req->path[i++] = '\0';
            str++;
            break;
        }
        req->path[i++] = *(str++);
    }
    // Read protocol.
    //
    i = 0;
    while (1)
    {
        if (*str == '\0') {
            return E_REQUEST_BAD_FORMAT;
        }
        else if (*str == '\r' && *(str+1) == '\n') {
            buffer[i++] = '\0';
            if (strcmp(buffer, "HTTP/1.0") != 0) {
                return E_REQUEST_BAD_FORMAT;
            }
            str++; // Do not skip '\n' at the end of the string.
            break;
        }
        buffer[i++] = *(str++);
    }
    // Read Content-type.
    //
    req->cont_type = CONTENT_TYPE_TEXT_PLAIN;
    char *cont_type_key_str = strcasestr(str, "Content-Type: ");
    if (cont_type_key_str != NULL) {
        str = cont_type_key_str+14;
        i = 0;
        while (1)
        {
            if (*str == '\0') {
                return E_REQUEST_BAD_FORMAT;
            }
            else if (*str == '\r' && *(str+1) == '\n') {
                buffer[i++] = '\0';
                req->cont_type = ContentType_from_str(buffer);
                str++; // Do not skip '\n' at the end of the string.
                break;
            }
            buffer[i++] = *(str++);
        }
    }
    // Read data.
    //
    char *del_empty_line = strstr(str, "\n\r\n");
    if (del_empty_line == NULL) {
        req->data[0] = '\0';
        req->data_size = 0;
        return SUCCESS;
    }
    str = del_empty_line + 3;
    i = 0;
    while (1)
    {
        if (*str == '\0') {
            req->data[i++] = '\0';
            req->data_size = i;
            break;
        }
        req->data[i++] = *(str++);
    }

    return SUCCESS;
}

void Request_deinit(Request *req) {

}

Route *Route_new(const char *path, RequestMethod rm, RouteCallback rc) {
    Route *r;
    r = malloc(sizeof(Route));
    if (r == NULL) {
        panic(E_ALLOC);
    }
    r->path = malloc((strlen(path) + 1) * sizeof(char));
    if (r->path == NULL) {
        panic(E_ALLOC);
    }
    r->req_meth = rm;
    r->rc = rc;

    strcpy(r->path, path);

    return r;
}

void Route_free(Route *r) {
    if (r == NULL) {
        return;
    }
    free(r->path);
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
