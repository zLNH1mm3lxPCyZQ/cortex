/* test.h - tiny single-header C test framework
 *
 * Usage:
 *   #include "test.h"
 *
#include <stdarg.h>
 *   TEST(addition_works) {
 *       ASSERT_EQ(2 + 2, 4);
 *       ASSERT_STR_EQ("foo", "foo");
 *   }
 *
 *   TEST(this_should_fail) {
 *       ASSERT_TRUE(1 == 2);
 *   }
 *

#endif
 *   TEST_MAIN()   // expands to int main(void) { ...runs every TEST()... }
 *
 * Tests register themselves automatically.
 * This relies on __attribute__((constructor)), a GCC/Clang extension -
 * works on Linux and macOS. Not portable to MSVC as-is.
 *
 * If you'd rather list tests explicitly instead of TEST_MAIN(), you can
 * still call RUN_TEST(name) by hand in your own main() and finish with
 * TEST_REPORT() - both still work.
 * 
#ifdef TEST_IMPLEMENTATION
 *
 * Test run order is whatever order the constructors fire in, which is
 * unspecified - don't rely on tests running in file order.
 */

#pragma once

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>

#define TEST_COLOR_GREEN "\x1b[32m"
#define TEST_COLOR_RED "\x1b[31m"
#define TEST_COLOR_RESET "\x1b[0m"

extern int test__pass_count;
extern int test__fail_count;
extern int test__current_failed;

void test__printf(const char *format, ...);

typedef void (*test__fn)(void);

typedef struct test__entry {
    const char *name;
    const char *file;
    int line;
    test__fn fn;
    struct test__entry *next;
} test__entry;

void test__register(test__entry *e);
void test__run_all(void);

#ifdef TEST_IMPLEMENTATION
int test__pass_count = 0;
int test__fail_count = 0;
int test__current_failed = 0;

static test__entry *test__list_head = NULL;

void test__printf(const char *format, ...) {
    time_t now = time(NULL);
    struct tm *local_time = localtime(&now);
    va_list arguments;

    if (local_time) {
        printf("[%04d-%02d-%02d %02d:%02d:%02d] ",
               local_time->tm_year + 1900, local_time->tm_mon + 1,
               local_time->tm_mday, local_time->tm_hour, local_time->tm_min,
               local_time->tm_sec);
    }

    va_start(arguments, format);
    vprintf(format, arguments);
    va_end(arguments);
}

void test__register(test__entry *e) {
    e->next = test__list_head;
    test__list_head = e;
}

#endif

#define TEST(name)                                            \
    static void name(void);                                   \
    static test__entry test__entry_##name = { #name, __FILE__, __LINE__, name, NULL }; \
    __attribute__((constructor)) static void test__reg_##name(void) { \
        test__register(&test__entry_##name);                   \
    }                                                          \
    static void name(void)

#define RUN_TEST(name)                                      \
    do {                                                     \
        test__current_failed = 0;                            \
        test__printf("RUN  %s\n", #name);                    \
        name();                                               \
        if (test__current_failed) {                           \
            test__fail_count++;                               \
            test__printf(TEST_COLOR_RED "FAIL %s\n" TEST_COLOR_RESET, #name); \
        } else {                                              \
            test__pass_count++;                               \
            test__printf(TEST_COLOR_GREEN "PASS %s (%s:%d)\n" TEST_COLOR_RESET, #name, __FILE__, __LINE__); \
        }                                                     \
    } while (0)

#ifdef TEST_IMPLEMENTATION
void test__run_all(void) {
    test__entry *e = test__list_head;
    while (e) {
        test__current_failed = 0;
        // test__printf("RUN  %s\n", e->name);
        e->fn();
        if (test__current_failed) {
            test__fail_count++;
            test__printf(TEST_COLOR_RED "FAIL %s\n" TEST_COLOR_RESET, e->name);
        } else {
            test__pass_count++;
            test__printf(TEST_COLOR_GREEN "PASS %s (%s:%d)\n" TEST_COLOR_RESET,
                         e->name, e->file, e->line);
        }
        e = e->next;
    }
}

#endif

#define TEST_REPORT()                                        \
    (test__printf("%d passed, %d failed\n",                  \
            test__pass_count, test__fail_count),              \
     test__fail_count != 0)

#define TEST_MAIN()                                           \
    int main(void) {                                          \
        test__run_all();                                       \
        return TEST_REPORT();                                  \
    }

#define ASSERT_FAIL(fmt, ...)                                \
    do {                                                     \
        test__current_failed = 1;                             \
        test__printf(TEST_COLOR_RED "     %s:%d: " fmt "\n" TEST_COLOR_RESET, __FILE__, __LINE__, \
               ##__VA_ARGS__);                                \
    } while (0)

#define ASSERT_TRUE(cond)                                    \
    do {                                                     \
        if (!(cond)) ASSERT_FAIL("ASSERT_TRUE(%s) failed", #cond); \
    } while (0)

#define ASSERT_FALSE(cond)                                   \
    do {                                                     \
        if (cond) ASSERT_FAIL("ASSERT_FALSE(%s) failed", #cond); \
    } while (0)

#define ASSERT_EQ(a, b)                                       \
    do {                                                      \
        if ((a) != (b)) {                                     \
            ASSERT_FAIL("ASSERT_EQ(%s, %s) failed", #a, #b);  \
        }                                                      \
    } while (0)

#define ASSERT_NE(a, b)                                        \
    do {                                                       \
        if ((a) == (b)) {                                      \
            ASSERT_FAIL("ASSERT_NE(%s, %s) failed", #a, #b);   \
        }                                                       \
    } while (0)

#define ASSERT_STR_EQ(a, b)                                          \
    do {                                                              \
        if (strcmp((a), (b)) != 0) {                                  \
            ASSERT_FAIL("ASSERT_STR_EQ(%s, %s) failed: \"%s\" != \"%s\"", \
                         #a, #b, (a), (b));                            \
        }                                                              \
    } while (0)

#define ASSERT_FLOAT_EQ(a, b, epsilon)                              \
    do {                                                            \
        if (fabs((a) - (b)) > (epsilon)) {                         \
            ASSERT_FAIL(                                           \
                "ASSERT_FLOAT_EQ(%s, %s) failed: %f != %f within epsilon %f", \
                #a, #b, (a), (b), (epsilon));                     \
        }                                                           \
    } while (0)

#define ASSERT_NULL(ptr)                                       \
    do {                                                       \
        if ((ptr) != NULL) ASSERT_FAIL("ASSERT_NULL(%s) failed", #ptr); \
    } while (0)

#define ASSERT_NOT_NULL(ptr)                                        \
    do {                                                            \
        if ((ptr) == NULL) ASSERT_FAIL("ASSERT_NOT_NULL(%s) failed", #ptr); \
    } while (0)
