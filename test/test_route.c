#include "unity.h"

#include "common-test-util.h"

#include "http-primitives.h"
#include "hash-server-error.h"

SET_UP_STUB();
TEAR_DOWN_STUB();

error_t test_route_callback(error_t err, Request *req, Response *res) {
    return E_NOT_IMPLEMENTED;
}

void should_create_valid_route() {
    Route r;
    const char *path = "/path/a";
    RequestMethod meth = METHOD_POST;
    Route_init(&r, path, meth, test_route_callback);

    TEST_ASSERT_EQUAL_STRING(path, r.path);
    TEST_ASSERT_EQUAL(meth, r.req_meth);
    TEST_ASSERT_EQUAL(&test_route_callback, r.rc);
}

void should_correctly_compare_routes() {
    Route r1, r2, r3;

    Route_init(&r1, "/api1", METHOD_POST, test_route_callback);
    TEST_ASSERT_TRUE(Route_satisfies_path(&r1, "/api1", METHOD_POST));
    TEST_ASSERT_FALSE(Route_satisfies_path(&r1, "not-api", METHOD_POST));

    Route_init(&r2, "/api2", METHOD_ALL, test_route_callback);
    TEST_ASSERT_TRUE(Route_satisfies_path(&r2, "/api2", METHOD_POST));
    TEST_ASSERT_FALSE(Route_satisfies_path(&r2, "not-api", METHOD_POST));

    Route_init(&r3, "*", METHOD_ALL, test_route_callback);
    TEST_ASSERT_TRUE(Route_satisfies_path(&r3, "not-api", METHOD_POST));
}

int main() {
    UNITY_BEGIN();

    RUN_TEST(should_create_valid_route);
    RUN_TEST(should_correctly_compare_routes);

    return UNITY_END();
}