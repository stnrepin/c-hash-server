#include <stdlib.h>
#include <string.h>

#include "unity.h"

#include "common-test-util.h"

#include "http-primitives.h"
#include "hash-server-error.h"

SET_UP_STUB();
TEAR_DOWN_STUB();

void should_create_valid_request() {
    Request r;

    const char *path = "/api";
    RequestMethod meth = METHOD_POST;
    ContentType cont_type = CONTENT_TYPE_APPLICATION_JSON;
    const char *data = "data data";
    int data_size = strlen(data) + 1;
    Request_init(&r, path, meth, cont_type, data, data_size);

    TEST_ASSERT_EQUAL_STRING(path, r.path);
    TEST_ASSERT_EQUAL(meth, r.method);
    TEST_ASSERT_EQUAL(cont_type, r.cont_type);
    TEST_ASSERT_EQUAL_STRING(data, r.data);
    TEST_ASSERT_EQUAL(data_size, r.data_size);

    Request_deinit(&r);
}

void should_create_valid_request_from_str() {
    error_t err;
    Request r;
    
    err = Request_init_from_str(&r, "POST / HTTP/1.0\r\n");
    TEST_ASSERT_TRUE(SUCC(err));
    TEST_ASSERT_EQUAL_STRING("/", r.path);
    TEST_ASSERT_EQUAL(METHOD_POST, r.method);
    TEST_ASSERT_EQUAL(CONTENT_TYPE_TEXT_PLAIN, r.cont_type);
    TEST_ASSERT_EQUAL(0, r.data_size);
    Request_deinit(&r);

#define DATA_BODY "{\"key\":\"data\"}\r\n"
    err = Request_init_from_str(&r, "POST /api HTTP/1.0\r\n" \
                                    "Content-Type: application/json\r\n" \
                                    "\r\n" \
                                    DATA_BODY);
    TEST_ASSERT_TRUE(SUCC(err));
    TEST_ASSERT_EQUAL_STRING("/api", r.path);
    TEST_ASSERT_EQUAL(METHOD_POST, r.method);
    TEST_ASSERT_EQUAL(CONTENT_TYPE_APPLICATION_JSON, r.cont_type);
    TEST_ASSERT_EQUAL_STRING(DATA_BODY, r.data);
    TEST_ASSERT_EQUAL(strlen(DATA_BODY)+1, r.data_size);
}

int main() {
    UNITY_BEGIN();

    RUN_TEST(should_create_valid_request);
    RUN_TEST(should_create_valid_request_from_str);

    return UNITY_END();
}