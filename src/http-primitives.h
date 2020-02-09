#ifndef HTTP_PRIMITIVES_H
#define HTTP_PRIMITIVES_H

#include <stdlib.h>
#include <stdbool.h>

#include "hash-server-error.h"
#include "config.h"

typedef enum {
    HTTP_CODE_OK = 200,
    HTTP_CODE_NO_CONTENT = 204,
    HTTP_CODE_NOT_FOUND = 404,
    HTTP_CODE_INTERNAL_ERROR = 500,
    /* Others are not implemented. */
} HttpCode;

const char *HttpCode_to_str(HttpCode c);

typedef enum {
    CONTENT_TYPE_PLAIN_TEXT,
    CONTENT_TYPE_APPLICATION_JSON,
    /* Others are not implemented. */
} ContentType;

ContentType ContentType_from_str(const char *s);
const char *ContentType_to_str(ContentType ct);

typedef struct {
    char *data;                 ///< Response message, C-string.
    size_t data_size;           ///< Response message string buffer size.
    ContentType cont_type;      ///< Content type of the message.
    HttpCode code;              ///< Http code of the result.
    bool is_end;                ///< Determine if the Response has already
                                ///< been sent to a client.
} Response;

void Response_init(Response *res);
void Response_deinit(Response *res);
error_t Response_to_str(Response *res, char *str, size_t max_str_size,
                        size_t *actual_str_size);
void Response_send(Response *res, const char *data, size_t sz);
void Response_set_content_type(Response *res, ContentType ct);
void Response_end(Response *res, HttpCode code);

typedef enum {
    METHOD_ALL,
    METHOD_POST,
    METHOD_NO,
    /* Others are not implemented. */
} RequestMethod;

RequestMethod RequestMethod_from_str(const char *s);
const char *RequestMethod_to_str(RequestMethod rm);

typedef struct {
    char path[MAX_URL_SIZE];
    RequestMethod method;
    ContentType cont_type;
    char data[MAX_HTTP_REQUEST_SIZE];
    size_t data_size;
} Request;

void Request_init(Request *req, const char *path, RequestMethod m,
                  ContentType ct, const char *data, size_t data_size);
error_t Request_init_from_str(Request *req, const char *str);
void Request_deinit(Request *req);

// Yeah, there also should be Next, but I'm too lazy. 
typedef error_t (*RouteCallback)(error_t, Request*, Response*);

typedef struct {
    char *path;
    RequestMethod req_meth;
    RouteCallback rc;
} Route;

void *Route_init(Route *r, const char *path, RequestMethod rm, RouteCallback rc);
void Route_deinit(Route *r);
bool Route_satisfies_path(Route* r, const char *path, RequestMethod rm);

#endif // !HTTP_PRIMITIVES_H
