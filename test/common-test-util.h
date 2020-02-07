#ifndef COMMON_TEST_UTIL_H
#define COMMON_TEST_UTIL_H

// HACK: https://github.com/ThrowTheSwitch/Unity/issues/438
//
#define SET_UP_STUB() \
    void setUp() { }
#define TEAR_DOWN_STUB() \
    void tearDown() { }

#endif // !COMMON_TEST_UTIL_H
