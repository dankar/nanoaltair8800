#ifndef _TESTING_H_
#define _TESTING_H_

#include <stdint.h>
#include <stdbool.h>

#include <time.h>

uint32_t tester_get_tick();

typedef enum
{
    TEST_LEVEL_ONE,
    TEST_LEVEL_TWO,
    TEST_LEVEL_THREE
} test_level_t;

typedef struct test_result_t
{
    struct test_result_t *next_result;
    char *description;
    char *test_condition;
    bool skipped;
    uint32_t result_time;
} test_result_t;

typedef struct test_results_t
{
    test_result_t *passed_head;
    test_result_t *failed_head;
    test_result_t *passed_tail;
    test_result_t *failed_tail;
} test_results_t;

typedef bool (*test_func_t)(test_results_t *test_results);

typedef struct test_t
{
    struct test_t  *next_test;
    char           *test_name;
    test_func_t    func_ptr;
    uint32_t       start_time;
    uint32_t       stop_time;
    bool           completed;
    test_results_t *result;
} test_t;

typedef struct test_session_t
{
    test_t *tests_head;
    test_t *tests_tail;
} test_session_t;

extern test_level_t TESTING_LEVEL;

#define DECLARE_TEST(x) \
bool test_ ##x(test_results_t *test_results); \
__attribute__((constructor)) \
void constr_ ## x() \
{ \
tester_add(test_ ##x, #x); \
} \
bool test_ ##x(test_results_t *test_results)

#define RESET_TEST_TIMER() test_timer = tester_get_tick()
#define GET_TEST_TIME() (tester_get_tick()-test_timer)

#define DELETE_LINE(fp) do { fprintf(fp, "\33[2K\r"); } while(0)

#define TEST(x) if(current_test_level <= TESTING_LEVEL) { x; }

#define TEST_CONDITION(cond, description) \
do { \
fprintf(stderr, "  %s (" description ") ......... running", test_description); fflush(stderr); \
if(current_test_level <= TESTING_LEVEL) \
{ \
if(cond) \
{ \
DELETE_LINE(stderr); \
tester_pass(test_results, test_description, description, GET_TEST_TIME()); \
} \
else \
{ \
DELETE_LINE(stderr); \
tester_fail(test_results, test_description, description, GET_TEST_TIME()); \
} \
RESET_TEST_TIMER(); \
} \
else \
{ \
DELETE_LINE(stderr); \
tester_skipped(test_results, test_description, description, GET_TEST_TIME()); \
RESET_TEST_TIMER(); \
} \
} while(0)

extern uint32_t g_timeout;

#define START_TIMEOUT(ms) do { g_timeout = tester_get_tick() + ms; } while(0);
#define TIMEOUT() (tester_get_tick() > g_timeout)

#define START_TEST(x, level) { uint32_t RESET_TEST_TIMER(); const char *test_description = x; test_level_t current_test_level = level; 
#define END_TEST() }

void tester_add(test_func_t func, const char *name);
void tester_run();
bool tester_print_results();
void tester_pass(test_results_t *results, const char *description, const char *test_condition, uint32_t time);
void tester_fail(test_results_t *results, const char *description, const char *test_condition, uint32_t time);
void tester_skipped(test_results_t *results, const char *description, const char *condition, uint32_t time);
void tester_set_run_level(test_level_t level);

#endif