#ifndef HTTP_PRIMITIVES_H
#define HTTP_PRIMITIVES_H

#include <stdlib.h>
#include <stdbool.h>

#include "hash-server-error.h"

typedef enum {
    OK = 200,
    NOT_FOUND = 404,
    /* Others are not implemented. */
} HttpCode;

typedef enum {
    PLAIN_TEXT,
    APPLICATION_JSON,
    /* Others are not implemented. */
} ContentType;

typedef struct {
    const char *data;
    size_t data_size;
    ContentType cont_type;
    bool is_end;
} Response;

Response *Response_new(size_t init_data_sz);
error_t Response_send(const char *data, size_t sz);
error_t Response_set_content_type(ContentType cont_type);
error_t Response_write(const char *data);
error_t Response_end(HttpCode code);

typedef enum {
    POST,
    /* Others are not implemented. */
} RequestMethod;

typedef struct {
    RequestMethod method;
    ContentType cont_type;
    void *data;
    size_t data_size;
} Request;

Request *Request_new(RequestMethod meth, ContentType cont_type,
                     void *data, size_t data_size);
void Request_free(Request *req);

// Yeah, there also should be Next, but I'm too lazy. 
typedef error_t (*RouterCallback)(error_t, Request, Response);

typedef struct {
    char *path;
    RequestMethod req_meth;
    RouterCallback rc;
} Router;

Router *Route_new(const char *path, RequestMethod rm, RouterCallback rc);
void Route_free(Router *r);
error_t Route_satisfies_path(Router* r, const char *path, RequestMethod rm);

#endif // !HTTP_PRIMITIVES_H
