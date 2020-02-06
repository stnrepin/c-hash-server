#ifndef CONFIG_H
#define CONFIG_H

#define SERVER_PORT 3000
#define SERVER_HOST "127.0.0.1"

// The size of content must be specified by Content-Length
//     https://www.w3.org/Protocols/HTTP/1.0/spec.html#POST
#define MAX_HTTP_REQ_RES_BUFFER_SIZE 2000
#define MAX_HTTP_HEADER_SIZE 8190 // Got from Apache.
#define MAX_PARALLEL_CONN_COUNT 1000

#endif // !CONFIG_H
