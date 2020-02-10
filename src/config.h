#ifndef CONFIG_H
#define CONFIG_H

#define SERVER_PORT 3000
#define SERVER_HOST "127.0.0.1"

// Yes, we nicely must to read Content-Length and alloc
// memory for data. But... but it is hard and error-prone enough.
#define MAX_HTTP_REQUEST_SIZE (8*1024)

#define MAX_PARALLEL_CONN_COUNT 1000

// https://stackoverflow.com/a/417184
#define MAX_URL_SIZE 2048

#define MAX_HASHING_STRING_SIZE 1024

#endif // !CONFIG_H
