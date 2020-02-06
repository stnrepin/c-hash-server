#include <stdlib.h>

#include "unity.h"

#include "json.h"
#include "hash-server-error.h"

#include "common-test-util.h"

#define MAX_VALUE_SIZE 100
#define MAX_STR_SIZE 100

SET_UP_STUB();
TEAR_DOWN_STUB();

/*
 * Method: json_decode_single_pair
 */
void should_decode_basic() {
    error_t err;
    /*const*/ char *str;
    char value[MAX_VALUE_SIZE];

    // The simplest case
    str = "{\"key\": \"1234\"}";
    err = json_decode_single_pair(str, "key", value, MAX_VALUE_SIZE);
    TEST_ASSERT_EQUAL(SUCCESS, err);
    TEST_ASSERT_EQUAL_STRING("1234", value);

    // String with a lot of spaces
    str = "   {\n\t\"key\": \"1234\",\n}";
    err = json_decode_single_pair(str, "key", value, MAX_VALUE_SIZE);
    TEST_ASSERT_EQUAL(SUCCESS, err);
    TEST_ASSERT_EQUAL_STRING("1234", value);

    // Empty value
    str = "{\"key\": \"\"}";
    err = json_decode_single_pair(str, "key", value, MAX_VALUE_SIZE);
    TEST_ASSERT_EQUAL(SUCCESS, err);
    TEST_ASSERT_EQUAL_STRING("", value);

    // Escaped "
    str = "{\"k\\\"ey\": \"12\"34\"}";
    err = json_decode_single_pair(str, "k\"ey", value, MAX_VALUE_SIZE);
    TEST_ASSERT_EQUAL(SUCCESS, err);
    TEST_ASSERT_EQUAL_STRING("12\"34", value);

    // Not escaped
    str = "{\"key\": \"12\\\\n34\"}";
    err = json_decode_single_pair(str, "key", value, MAX_VALUE_SIZE);
    TEST_ASSERT_EQUAL(SUCCESS, err);
    TEST_ASSERT_EQUAL_STRING("12\\n34", value);

    // Escaped symbol
    str = "{\"key\": \"12\\r34\"}";
    err = json_decode_single_pair(str, "key", value, MAX_VALUE_SIZE);
    TEST_ASSERT_EQUAL(SUCCESS, err);
    TEST_ASSERT_EQUAL_STRING("12\r34", value);
}

/*
 * Method: json_decode_single_pair
 */
void should_return_error_if_decode_invalid_json() {
    error_t err;
    /*const*/ char *str;
    char value[MAX_VALUE_SIZE];

    // Missed ""
    str = "{key: \"1234\"}";
    err = json_decode_single_pair(str, "key", value, MAX_VALUE_SIZE);
    TEST_ASSERT_EQUAL(E_JSON_BAD_FORMAT, err);

    // Extra }
    str = "{\"key\": \"1234\"}}";
    err = json_decode_single_pair(str, "key", value, MAX_VALUE_SIZE);
    TEST_ASSERT_EQUAL(E_JSON_BAD_FORMAT, err);

    // Extra {
    str = "{{\"key\": \"1234\"}";
    err = json_decode_single_pair(str, "key", value, MAX_VALUE_SIZE);
    TEST_ASSERT_EQUAL(E_JSON_BAD_FORMAT, err);

    // Messed {}
    str = "\"key\": \"1234\"";
    err = json_decode_single_pair(str, "key", value, MAX_VALUE_SIZE);
    TEST_ASSERT_EQUAL(E_JSON_BAD_FORMAT, err);

    // Missed :
    str = "{\"key\" \"1234\"}";
    err = json_decode_single_pair(str, "key", value, MAX_VALUE_SIZE);
    TEST_ASSERT_EQUAL(E_JSON_BAD_FORMAT, err);

    // Unescaped " in key (or value)
    str = "{\"ke\"y\" \"1234\"}";
    err = json_decode_single_pair(str, "key", value, MAX_VALUE_SIZE);
    TEST_ASSERT_EQUAL(E_JSON_BAD_FORMAT, err);
}

/*
 * Method: json_decode_single_pair
 * 
 * See the comment in json.h
 */
void should_not_decode_complex_json() {
    error_t err;
    /*const*/ char *str;
    char value[MAX_VALUE_SIZE];

    str = "{\"key1\": \"1234\", \"key2\": \"value\"}";
    err = json_decode_single_pair(str, "key2", value, MAX_VALUE_SIZE);
    TEST_ASSERT_EQUAL(E_JSON_BAD_FORMAT, err);

    str = "{\"key1\": \"1234\", []}";
    err = json_decode_single_pair(str, "key2", value, MAX_VALUE_SIZE);
    TEST_ASSERT_EQUAL(E_JSON_BAD_FORMAT, err);
}

/*
 * Method: json_decode_single_pair
 */
void should_return_error_if_key_not_found_on_decode() {
    error_t err;
    /*const*/ char *str;
    char value[MAX_VALUE_SIZE];
    str = "{\"key\": \"1234\"}";
    err = json_decode_single_pair(str, "not-exist-key", value, MAX_VALUE_SIZE);
    TEST_ASSERT_EQUAL(E_JSON_BAD_FORMAT, err);

    str = "";
    err = json_decode_single_pair(str, "key", value, MAX_VALUE_SIZE);
    TEST_ASSERT_EQUAL(E_JSON_BAD_FORMAT, err);
}

/*
 * Method: json_decode_single_pair
 */
void should_handle_out_of_range_on_decode() {
    error_t err;
    const int VALUE_SIZE = 3;
    /*const*/ char *str;
    char value[VALUE_SIZE];

    // The simplest case
    str = "{\"key\": \"1234\"}";
    err = json_decode_single_pair(str, "key", value, VALUE_SIZE);
    TEST_ASSERT_EQUAL(E_JSON_DECODE_RANGE, err);
}

/*
 * Method: json_encode_single_pair
 */
void should_encode_basic() {
    error_t err;
    char str[MAX_STR_SIZE],
         *key,
         *val;

    // The simplest case
    key = "key";
    val = "val";
    err = json_encode_single_pair(str, MAX_STR_SIZE, key, val);
    TEST_ASSERT_EQUAL(SUCCESS, err);
    TEST_ASSERT_EQUAL_STRING("{\"key\":\"val\"}", str);

    // Empty value
    key = "key";
    val = "";
    err = json_encode_single_pair(str, MAX_STR_SIZE, key, val);
    TEST_ASSERT_EQUAL(SUCCESS, err);
    TEST_ASSERT_EQUAL_STRING("{\"key\":\"\"}", str);

    // Escaped
    key = "ke\"y";
    val = "va\nl";
    err = json_encode_single_pair(str, MAX_STR_SIZE, key, val);
    TEST_ASSERT_EQUAL(SUCCESS, err);
    TEST_ASSERT_EQUAL_STRING("{\"ke\\\"y\":\"va\\nl\"}", str);
}

/*
 * Method: json_encode_single_pair
 */
void should_handle_out_of_range_on_encode() {
    error_t err;
    const int STR_SIZE = 5;
    char str[STR_SIZE],
         *key,
         *val;
    
    key = "key";
    val = "val";
    err = json_encode_single_pair(str, STR_SIZE, key, val);
    TEST_ASSERT_EQUAL(E_JSON_ENCODE_RANGE, err);
}

int main() {
    UNITY_BEGIN();

    RUN_TEST(should_decode_basic);
    RUN_TEST(should_return_error_if_decode_invalid_json);
    RUN_TEST(should_not_decode_complex_json);
    RUN_TEST(should_return_error_if_key_not_found_on_decode);
    RUN_TEST(should_handle_out_of_range_on_decode);

    RUN_TEST(should_encode_basic);
    RUN_TEST(should_handle_out_of_range_on_encode);

    return UNITY_END();
}
