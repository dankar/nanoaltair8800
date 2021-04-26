#include "testing.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#ifdef _WIN32
#include <Windows.h>
#endif

test_session_t g_test_session = {0};

typedef enum
{
    BLACK = 30,
    RED = 31,
    GREEN = 32,
    YELLOW = 33,
    BLUE = 34,
    MAGENTA = 35,
    CYAN = 36,
    WHITE = 37
} color_t;
void strcat_color(char *buff, char *str, color_t color, bool bold)
{
    sprintf(&buff[strlen(buff)], "\033[%s;%dm", bold ? "1" : "", color);
    strcat(buff, str);
    strcat(buff, "\033[0m");
}

uint32_t tester_get_tick()
{
#ifdef _WIN32
    return GetTickCount();
#else
    struct timespec ts;
    unsigned theTick = 0U;
    clock_gettime(CLOCK_REALTIME, &ts);
    theTick = ts.tv_nsec / 1000000;
    theTick += ts.tv_sec * 1000;
    return theTick;
#endif
}

void print_color(const char *str, color_t color, bool bold)
{
    printf("\033[%s;%dm", bold ? "1" : "", color);
    printf("%s", str);
    printf("\033[0m");
}

char g_output_buffer[512];

void tester_pretty_print(bool success, bool skipped, uint8_t tabs, const char *desc, const char *cond, uint32_t time, FILE *fp)
{
    for(int i = 0; i < tabs; i++)
    {
        g_output_buffer[i] = ' ';
    }

    if(cond)
    {
        sprintf(&g_output_buffer[tabs], "%s (%s) ", desc, cond);
    }
    else
    {
        strcpy(&g_output_buffer[tabs], desc);
    }

    int32_t number_of_dots = 74 - snprintf(NULL, 0, "(%fs)", (float)time / 1000.0f);

    number_of_dots -= strlen(&g_output_buffer[tabs]) + tabs;

    if(number_of_dots < 0)
        number_of_dots = 0;

    if(skipped)
    {
        number_of_dots -= 5;
    }
    else if(!success)
    {
        number_of_dots -= 4;
    }

    for(int i = 0; i < number_of_dots; i++)
    {
        strcat(g_output_buffer, ".");
    }

    sprintf(&g_output_buffer[strlen(g_output_buffer)], "(%fs)", (float)time/1000.0f);

    strcat(g_output_buffer, " [");
    if(skipped)
    {
        strcat_color(g_output_buffer, "skipped", YELLOW, false);
    }
    else if(success)
    {
        strcat_color(g_output_buffer, "ok", GREEN, false);
    }
    else
    {
        strcat_color(g_output_buffer, "failed", RED, true);
    }
    strcat(g_output_buffer, "]");
    

    fprintf(fp, "%s\n", g_output_buffer);
}

void tester_add(test_func_t func, const char *name)
{
    test_t *test = (test_t*)calloc(1, sizeof(test_t));
    test->test_name = malloc(strlen(name)+1);
    test->func_ptr = func;
    strcpy(test->test_name, name);
    if(g_test_session.tests_tail == NULL)
    {
        g_test_session.tests_tail = g_test_session.tests_head = test;
    }
    else
    {
        g_test_session.tests_tail->next_test = test;
        g_test_session.tests_tail = test;
    }
}

void tester_run()
{
    test_t *test = g_test_session.tests_head;
    while(test)
    {
        fprintf(stderr, "Running \"%s\"\n", test->test_name);
        
        test->result = (test_results_t *)calloc(1, sizeof(test_results_t));
        test->start_time = tester_get_tick();
        test->completed = test->func_ptr(test->result);
        test->stop_time = tester_get_tick();
        test = test->next_test;
    }
}

void tester_new_result(test_result_t **head, test_result_t **tail, const char *description, const char *test_condition, bool skipped, uint32_t time)
{
    test_result_t *result = (test_result_t*)calloc(1, sizeof(test_result_t));
    result->description = malloc(strlen(description) + 1);
    result->test_condition = malloc(strlen(test_condition) + 1);
    result->result_time = time;
    strcpy(result->description, description);
    strcpy(result->test_condition, test_condition);
    if(skipped)
    {
        result->skipped = true;
    }    
    
    if(*head == NULL)
    {
        *head = *tail = result;
    }
    else
    {
        (*tail)->next_result = result;
        *tail = result;
    }
}
void tester_pass(test_results_t *results, const char *description, const char *test_condition, uint32_t time)
{
    tester_pretty_print(true, false, 2, description, test_condition, time, stderr);
    tester_new_result(&results->passed_head, &results->passed_tail, description, test_condition, false, time);
}
bool tester_print_errors = false;
extern void test_print_error();

void tester_fail(test_results_t *results, const char *description, const char *test_condition, uint32_t time)
{
    tester_pretty_print(false, false, 2, description, test_condition, time, stderr);
    if (tester_print_errors)
    {
        test_print_error();
    }
    tester_new_result(&results->failed_head, &results->failed_tail, description, test_condition, false, time);
    TESTING_LEVEL = TEST_LEVEL_NONE;
}

void print_line()
{
    printf("-------------------------------------------------------------------------------\n");
}

bool tester_print_results()
{
    print_line();
    print_color("\n          Test results for build \"" BUILD_VERSION "\":\n\n", WHITE, true);
    
    test_t *test = g_test_session.tests_head;
    
    uint32_t failures = 0;
    uint32_t passes = 0;
    uint32_t warnings = 0;
    uint32_t total_time = 0;
    uint32_t skipped_tests = 0;
    while(test)
    {
        print_line();
        sprintf(g_output_buffer, "  %s (%fs) (%s)\n", test->test_name, (float)(test->stop_time - test->start_time) / 1000.0f, test->completed ? "test ran to completion" : "test aborted early");
        total_time += test->stop_time - test->start_time;
        if(!test->completed)
        {
            warnings++;
        }
        test_results_t *res = test->result;
        if(!res || (!res->failed_head && !res->passed_head))
        {
            strcat(g_output_buffer, "    No results for test\n");
            print_color(g_output_buffer, YELLOW, false);
            print_line();
            warnings++;
        }
        else
        {
            if(res->failed_head)
            {
                print_color(g_output_buffer, RED, false);
                
            }
            else
            {
                print_color(g_output_buffer, GREEN, false);
            }
            print_line();
            test_result_t *pass = res->passed_head;
            while(pass)
            {
                if(pass->skipped)
                {
                    tester_pretty_print(false, true, 4, pass->description, pass->test_condition, pass->result_time, stdout);
                    skipped_tests++;
                }
                else
                {
                    tester_pretty_print(true, false, 4, pass->description, pass->test_condition, pass->result_time, stdout);
                    passes++;
                }
                pass = pass->next_result;
                
            }
            test_result_t *fail = res->failed_head;
            while(fail)
            {
                tester_pretty_print(false, false, 4, fail->description, fail->test_condition, fail->result_time, stdout);
                fail = fail->next_result;
                failures++;
            }
        }
        test = test->next_test;
    }

    printf("\n                  Succeeded: ");
    sprintf(g_output_buffer, "%d", passes);
    print_color(g_output_buffer, GREEN, false);
    printf(" Failed: ");
    sprintf(g_output_buffer, "%d", failures);
    print_color(g_output_buffer, failures ? RED : GREEN, false);
    printf(" Warnings: ");
    sprintf(g_output_buffer, "%d", warnings);
    print_color(g_output_buffer, warnings ? YELLOW : GREEN, false);
    
    printf(" Skipped: ");
    sprintf(g_output_buffer, "%d", skipped_tests);
    print_color(g_output_buffer, skipped_tests ? YELLOW : GREEN, false);
    printf("\n");

    printf("                          %d tests done in %fs\n", failures + passes, (float)total_time/1000.0f);

    if(passes && !failures && !warnings)
    {
        print_color("\n\n                         ******* Verdict: pass *******\n\n\n", GREEN, true);
    }
    else
    {
        print_color("\n\n                         ******* Verdict: fail *******\n\n", RED, true);
    }

    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
        printf("                     Run completed on: %d-%02d-%02d %02d:%02d:%02d\n\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
    
    return !warnings && !failures;
}

void tester_skipped(test_results_t *results, const char *description, const char *condition, uint32_t time)
{
    tester_new_result(&results->passed_head, &results->passed_tail, description, condition, true, time);
}

uint32_t g_timeout;

test_level_t TESTING_LEVEL;
void tester_set_run_level(test_level_t level)
{
    TESTING_LEVEL = level;
}

int main(int argc, char *argv[])
{
    srand((int)time(NULL));
    tester_set_run_level(TEST_LEVEL_ONE);
    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "level_one") == 0)
            tester_set_run_level(TEST_LEVEL_ONE);
        if (strcmp(argv[i], "level_two") == 0)
            tester_set_run_level(TEST_LEVEL_TWO);
        if (strcmp(argv[i], "level_three") == 0)
            tester_set_run_level(TEST_LEVEL_THREE);
        if (strcmp(argv[i], "print_errors") == 0)
            tester_print_errors = true;
    }
    tester_run();
    return tester_print_results() ? 0 : 1;
}
