#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "unity.h"

#include "common-test-util.h"

#include "http-primitives.h"
#include "hash-server-error.h"

SET_UP_STUB();
TEAR_DOWN_STUB();

/*
 * Method: Response_init()
 */
void should_create_valid_response() {
    // Basic case
    Response r;

    Response_init(&r);

    TEST_ASSERT_EQUAL(CONTENT_TYPE_PLAIN_TEXT, r.cont_type);
    TEST_ASSERT_EQUAL(HTTP_CODE_NO_CONTENT, r.code);
    TEST_ASSERT_EQUAL(0, r.data_size);
    TEST_ASSERT_NULL(r.data);
    TEST_ASSERT_EQUAL(false, r.is_end);

    Response_deinit(&r);
}

/*
 * Method: Response_set_content_type()
 */
void should_set_content_type() {
    Response r;
    Response_init(&r);

    Response_set_content_type(&r, CONTENT_TYPE_APPLICATION_JSON);
    TEST_ASSERT_EQUAL(CONTENT_TYPE_APPLICATION_JSON, r.cont_type);
    Response_set_content_type(&r, CONTENT_TYPE_PLAIN_TEXT);
    TEST_ASSERT_EQUAL(CONTENT_TYPE_PLAIN_TEXT, r.cont_type);

    Response_deinit(&r);
}

void should_end() {
    Response r;
    Response_init(&r);

    Response_end(&r, HTTP_CODE_OK);

    TEST_ASSERT_EQUAL(true, r.is_end);
    TEST_ASSERT_EQUAL(HTTP_CODE_OK, r.code);

    Response_deinit(&r);
}

void should_send() {
    Response r1, r2;
    Response_init(&r1);
    Response_init(&r2);

    const char *VALUE = "test test";
    size_t value_size = strlen(VALUE) + 1;
    Response_send(&r1, VALUE, value_size);
    TEST_ASSERT_EQUAL_STRING(VALUE, r1.data);
    TEST_ASSERT_EQUAL(value_size, r1.data_size);

    Response_send(&r2, "", 1);
    TEST_ASSERT_EQUAL_STRING("", r2.data);
    TEST_ASSERT_EQUAL(1, r2.data_size);

    Response_deinit(&r1);
    Response_deinit(&r2);
}

int main() {
    UNITY_BEGIN();

    RUN_TEST(should_create_valid_response);
    RUN_TEST(should_set_content_type);
    RUN_TEST(should_end);
    RUN_TEST(should_send);

    return UNITY_END();
}
