#include <stdlib.h>

#include "unity.h"

#include "http-primitives.h"
#include "hash-server-error.h"

const int BASIC_RESPONSE_DATA_SIZE = 1000;

/*
 * Method: Response_new()
 */
void should_create_valid_response() {
    // Basic case
    Response *r = Response_new(BASIC_RESPONSE_DATA_SIZE);
    TEST_ASSERT_EQUAL(CONTENT_TYPE_PLAIN_TEXT, r->cont_type);
    TEST_ASSERT_EQUAL(BASIC_RESPONSE_DATA_SIZE, r->data_size);
    TEST_ASSERT_EQUAL(0, r->data_len);
    TEST_ASSERT_NOT_NULL(r->data);

    Response_free(r);
}

/*
 * Method: Response_set_content_type()
 */
void should_set_content_type() {
    Response *r = Response_new(BASIC_RESPONSE_DATA_SIZE);

    Response_set_content_type(r, CONTENT_TYPE_APPLICATION_JSON);
    TEST_ASSERT_EQUAL(CONTENT_TYPE_APPLICATION_JSON, r->cont_type);
    Response_set_content_type(r, CONTENT_TYPE_PLAIN_TEXT);
    TEST_ASSERT_EQUAL(CONTENT_TYPE_PLAIN_TEXT, r->cont_type);

    Response_free(r);
}

/*
 * Method: Response_write()
 */
void should_write_valid_text() {
#define WRITE_TEST_STR "Oh, shit"
#define WRITE_TEST_CONCATE(a, b) a#b
#define WRITE_TEST_DOUBLED_STR WRITE_TEST_CONCATE(WRITE_TEST_STR, WRITE_TEST_STR)

    const char *str = WRITE_TEST_STR,
               *doubled_str = WRITE_TEST_DOUBLED_STR;
    error_t err;
    Response *r;

    r = Response_new(BASIC_RESPONSE_DATA_SIZE);
    err = Response_write(r, str);
    
    TEST_ASSERT_EQUAL(SUCCESS, err);
    TEST_ASSERT_EQUAL(str, r->data_size);
    TEST_ASSERT_EQUAL(strlen(str) + 1, r->data_len);
    TEST_ASSERT_EQUAL_STRING(str, r->data);

    err = Response_write(r, str);
    TEST_ASSERT_EQUAL_STRING(doubled_str, r->data);

    Response_free(r);
}

/*
 * Method: Response_write()
 */
void should_return_error_if_write_too_much() {
    error_t err;
    Response *r;

    r = Response_new(5);
    err = Response_write(r, "123456");

    TEST_ASSERT_EQUAL(E_RESPONSE_RANGE, err);

    Response_free(r);
}

// TODO: Can we test Response_end() and Response_end()?

int main() {
    UNITY_BEGIN();

    RUN_TEST(should_create_valid_response);
    RUN_TEST(should_set_content_type);
    RUN_TEST(should_write_valid_text);
    RUN_TEST(should_return_error_if_write_too_much);

    return UNITY_END();
}
