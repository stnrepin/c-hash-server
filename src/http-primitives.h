#ifndef HTTP_PRIMITIVES_H
#define HTTP_PRIMITIVES_H

#include <stdlib.h>
#include <stdbool.h>

#include "hash-server-error.h"

typedef enum {
    HTTP_CODE_OK = 200,
    HTTP_CODE_NOT_FOUND = 404,
    /* Others are not implemented. */
} HttpCode;

typedef enum {
    CONTENT_TYPE_PLAIN_TEXT,
    CONTENT_TYPE_APPLICATION_JSON,
    /* Others are not implemented. */
} ContentType;

typedef struct {
    const char *data;           ///< Response message, C-string.
    size_t data_size;           ///< Response message string buffer size.
    size_t data_len;            ///< Response message actual string len.
    ContentType cont_type;      ///< Content type of the message.
    bool is_end;                ///< Determine if Response has already
                                ///< been sent to client.
} Response;

Response *Response_new(size_t init_data_sz);
error_t Response_send(Response *res, const char *data, size_t sz);
error_t Response_set_content_type(Response *res, ContentType cont_type);
error_t Response_write(Response *res, const char *data);
error_t Response_end(Response *res, HttpCode code);

typedef enum {
    METHOD_POST,
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
