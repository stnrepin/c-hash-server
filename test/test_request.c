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

    RequestMethod meth = METHOD_POST;
    ContentType cont_type = CONTENT_TYPE_APPLICATION_JSON;
    const char *data = "data data";
    int data_size = strlen(data) + 1;
    Request_init(&r, meth, cont_type, data, data_size);

    TEST_ASSERT_EQUAL(meth, r.method);
    TEST_ASSERT_EQUAL(cont_type, r.cont_type);
    TEST_ASSERT_EQUAL_STRING(data, r.data);
    TEST_ASSERT_EQUAL(data_size, r.data_size);

    Request_deinit(&r);
}

int main() {
    UNITY_BEGIN();

    RUN_TEST(should_create_valid_request);

    return UNITY_END();
}