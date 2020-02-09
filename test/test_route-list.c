#include "unity.h"

#include "route-list.h"
#include "http-primitives.h"

#include "common-test-util.h"

SET_UP_STUB();
TEAR_DOWN_STUB();

void should_create_list() {
    RouteList *l;

    l = RouteList_new();
    TEST_ASSERT_NULL(l->head);
    TEST_ASSERT_NULL(l->last);
    TEST_ASSERT_EQUAL(0, l->size);

    RouteList_free(l);
}

void  should_add_to_list() {
    RouteList *l;
    Route r1, r2, r3;

    l = RouteList_new();
    Route_init(&r1, "r1", METHOD_ALL, NULL);
    Route_init(&r2, "r2", METHOD_ALL, NULL);
    Route_init(&r3, "r3", METHOD_ALL, NULL);

    RouteList_add(l, &r1);
    TEST_ASSERT_EQUAL(&r1, l->head->route);
    TEST_ASSERT_EQUAL(&r1, l->last->route);
    TEST_ASSERT_EQUAL(1, l->size);

    RouteList_add(l, &r2);
    TEST_ASSERT_EQUAL(&r1, l->head->route);
    TEST_ASSERT_EQUAL(&r2, l->last->route);
    TEST_ASSERT_EQUAL(2, l->size);

    RouteList_add(l, &r3);
    TEST_ASSERT_EQUAL(&r1, l->head->route);
    TEST_ASSERT_EQUAL(&r2, l->head->next->route);
    TEST_ASSERT_EQUAL(&r3, l->last->route);
    TEST_ASSERT_EQUAL(3, l->size);

    RouteList_free(l);
}

int main() {
    UNITY_BEGIN();

    RUN_TEST(should_create_list);
    RUN_TEST(should_add_to_list);

    return UNITY_END();
}