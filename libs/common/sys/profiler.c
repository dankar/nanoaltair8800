#include "profiler.h"
#include "sys/log.h"
#include "sys/atomic_lock.h"
#include "main.h"
#include <string.h>

#define PAD_EMPTY           "   "                            // "   "
#define PAD_PIPE            "\xe2\x94\x82  "                 // "│  "
#define PAD_CHILD           "\xe2\x94\x9c\xe2\x94\x80 "      // "├─ "
#define PAD_LAST_CHILD      "\xe2\x94\x94\xe2\x94\x80 "      // "└─ "
#define INFO_COLUMN         80

#if PROFILER_ENABLED == 1
#define PROF_MEM (65*1024)

typedef struct function_t
{
    void *func;
    struct function_t *parent;
    struct function_t *next_sibling;
    struct function_t *children_head;
    uint32_t entry_time;
    uint32_t total_time;
    uint32_t num_calls;
} function_t;

char profiler_memory[PROF_MEM];
char profiler_memory_copy[PROF_MEM];

char *current_memory_area = profiler_memory;
uint32_t next_free = 0;
function_t *current_function = NULL;

lock_t profiler_semaphore = 0;

LOG_UNIT("profiler");

bool NO_PROFILE profiler_flatten();

void NO_PROFILE profiler_swap(uint32_t *a, uint32_t *b)
{
    uint32_t tmp = *a;
    *a = *b;
    *b = tmp;
}

void NO_PROFILE profiler_sort_children(function_t *func)
{
    bool done = false;
    uint32_t max_tries = 0;
    do
    {
        done = true;
        function_t *child = func->children_head;
        function_t *prev = NULL;
        
        while(child && child->next_sibling)
        {
            function_t *next = child->next_sibling;
            if(child->total_time < next->total_time)
            {
                child->next_sibling = next->next_sibling;
                next->next_sibling = child;
                if(prev == NULL)
                {
                    func->children_head = next;
                }
                else
                {
                    prev->next_sibling = next;
                }
                
                done = false;
            }
            prev = child;
            child = child->next_sibling;
        }
    }while(!done && ++max_tries < 200);
}

uint32_t NO_PROFILE profiler_get_child_sum(function_t *func)
{
    uint32_t total_time = 0;
    function_t *child_func = func->children_head;
    while(child_func)
    {
        total_time += child_func->total_time;
        child_func = child_func->next_sibling;
    }
    return total_time;
}

const char NO_PROFILE *profiler_get_func_name(function_t *func)
{
    if(func->func)
    {
        uint32_t *p = func->func-5;
        if((*p & 0xff000000) == 0xff000000)
        {
            uint32_t offset = *p & 0x00ffffff;
            return func->func-(5+offset);
        }
    }
    return NULL;
}
void NO_PROFILE profiler_pad(uint16_t n)
{
    for(uint32_t i = 0; i < n; i++)
    {
        printf(" ");
    }
}

// Return how many chars on screen the string represents
// NOTE(dankar): Stupid hack for checking length of
// utf8 string
uint32_t NO_PROFILE profiler_printed_length(char *string)
{
    uint32_t c = 0;
    uint32_t i = 0;
    
    while(string[i])
    {
        if((string[i] & 0x80) == 0)
        {
            i++;
        }
        else if((string[i] & 0xE0) == 0xC0)
        {
            i+=2;
        }
        else if((string[i] & 0xF0) == 0xE0)
        {
            i+=3;
        }
        else if((string[i] & 0xF8) == 0xF0)
        {
            i+=4;
        }
        
        c++;
    }
    
    return c;
}

uint32_t NO_PROFILE profiler_pad_string(char *string, const char *pad_string)
{
    strcat(string, pad_string);
    return strlen(pad_string);
}

void NO_PROFILE profiler_unpad_string(char *string, uint32_t num_chars)
{
    string[strlen(string)-(num_chars)] = '\0';
}

void NO_PROFILE profiler_print_func(function_t *func, char *pad_string, bool all_exclusive, bool last_child)
{
    uint32_t child_time_sum = 0;
    uint32_t pad_chars_added = 0;
    
    if(last_child)
    {
        pad_chars_added = profiler_pad_string(pad_string, PAD_LAST_CHILD);
    }
    else
    {
        pad_chars_added = profiler_pad_string(pad_string, PAD_CHILD);
    }
    
    printf(pad_string);
    
    profiler_unpad_string(pad_string, pad_chars_added);
    pad_chars_added = profiler_pad_string(pad_string, PAD_PIPE);
    
    child_time_sum = profiler_get_child_sum(func);
    
    const char *func_name = profiler_get_func_name(func);
    
    uint32_t inclusive_time = func->total_time;
    uint32_t exclusive_time = func->total_time-child_time_sum;
    float percent = 0;
    if(func->parent && inclusive_time < func->parent->total_time)
    {
        percent = ((float)func->total_time)/func->parent->total_time*100.0f;
    }
    else if(func->parent && inclusive_time == func->parent->total_time)
    {
        percent = 100.0f;
    }
    
    uint32_t len = snprintf(NULL, 0, "%s (%p)", func_name != NULL ? func_name : "none", func->func);
    printf("%s (%p)", func_name != NULL ? func_name : "none", func->func);
    
    uint32_t total_printed = len + profiler_printed_length(pad_string);
    
    if(total_printed < INFO_COLUMN)
        profiler_pad(INFO_COLUMN-total_printed);
    if(all_exclusive)
        printf(" %05ld (%06.2f%%, %07ld c)\r\n", inclusive_time, percent, func->num_calls);
    else
        printf(" i %05ld, e %05ld (%06.2f%%, %07ld c)\r\n", inclusive_time, exclusive_time, percent, func->num_calls);
    
    profiler_sort_children(func);
    
    if(last_child)
    {
        profiler_unpad_string(pad_string, pad_chars_added);
        pad_chars_added = profiler_pad_string(pad_string, PAD_EMPTY);
    }
    
    function_t *child_func = func->children_head;
    while(child_func)
    {
        profiler_print_func(child_func, pad_string, all_exclusive, child_func->next_sibling == NULL);
        child_func = child_func->next_sibling;
    }
    
    profiler_unpad_string(pad_string, pad_chars_added);
}

bool NO_PROFILE profiler_is_empty()
{
    return next_free == 0;
}

void NO_PROFILE profiler_print()
{
    char pad_string[128] = "";
    if(!lock(&profiler_semaphore))
    {
        printf("Failed to get profiler semaphore\r\n");
        return;
    }
    if(!profiler_is_empty())
    {
        function_t *start_func = (function_t*)current_memory_area;
        start_func->total_time = profiler_get_child_sum(start_func);
        profiler_print_func(start_func, pad_string, false, true);
        profiler_flatten();
        printf("Flattened:\r\n");
        start_func = (function_t*)profiler_memory_copy;
        profiler_print_func(start_func, pad_string, true, true);
    }
    else
    {
        printf("No profiler data\r\n");
    }
    unlock(&profiler_semaphore);
}

bool NO_PROFILE profiler_alloc(function_t **new)
{
    if((next_free + sizeof(function_t)) > PROF_MEM)
    {
        ASSERT(false, "OOM\r\n");
        return false;
    }
    *new = (function_t*)&current_memory_area[next_free];
    memset(*new, 0, sizeof(function_t));
    next_free += sizeof(function_t);
    return true;
}

void NO_PROFILE profiler_reset()
{
    function_t *out;
    memset(current_memory_area, 0, PROF_MEM);
    next_free = 0;
    if(!profiler_alloc(&out))
    {
        return;
    }
    out->func = NULL;
    out->parent = NULL;
    out->entry_time = 0;
    current_function = out;
}

function_t NO_PROFILE *profiler_find_child(function_t *parent, void *fn)
{
    function_t *func = parent->children_head;
    while(func)
    {
        if(func->func == fn)
        {
            return func;
        }
        func = func->next_sibling;
    }
    return NULL;
}

bool NO_PROFILE profiler_add_to_list(void *this_fn, function_t *target, function_t **out)
{
    *out = profiler_find_child(target, this_fn);
    
    if(*out)
    {
        return true;
    }
    
    if(!profiler_alloc(out))
    {
        return false;
    }
    
    (*out)->func = this_fn;
    (*out)->parent = target;
    (*out)->next_sibling = NULL;
    
    if(target->children_head == 0)
    {
        // Only one entry
        target->children_head = *out;
    }
    else
    {
        // Add to tail
        (*out)->next_sibling = target->children_head;
        target->children_head = *out;
    }
    
    return true;
}

bool NO_PROFILE profiler_get(void *this_fn, function_t **out, void *call_site)
{
    if(profiler_is_empty())
    {
        profiler_reset();
    }
    
    return profiler_add_to_list(this_fn, current_function, out);
}

bool NO_PROFILE profiler_flatten_add(function_t *func)
{
    function_t *new_func;
    if(!profiler_get(func->func, &new_func, NULL))
    {
        return false;
    }
    new_func->num_calls += func->num_calls;
    new_func->total_time += func->total_time - profiler_get_child_sum(func);
    
    function_t *child = func->children_head;
    while(child)
    {
        profiler_flatten_add(child);
        child = child->next_sibling;
    }
    
    return true;
}

bool NO_PROFILE profiler_flatten()
{
    current_memory_area = profiler_memory_copy;
    profiler_reset();
    function_t *copy_func = (function_t*)profiler_memory;
    
    function_t *child = copy_func->children_head;
    while(child)
    {
        profiler_flatten_add(child);
        child = child->next_sibling;
    }
    
    uint32_t sum = profiler_get_child_sum(copy_func);
    copy_func = (function_t*)profiler_memory_copy;
    copy_func->total_time = sum;
    current_memory_area = profiler_memory;
    return true;
}

void NO_PROFILE __cyg_profile_func_enter (void *this_fn, void *call_site) 
{
    if(!lock(&profiler_semaphore))
    {
        return;
    }
    uint32_t entry_time = HAL_GetTick();
    function_t *func;
    if(!profiler_get(this_fn, &func, call_site))
    {
        ASSERT(false, "what?");
        current_function = 0;
        next_free = 0;
        unlock(&profiler_semaphore);
        return;
    }
    func->num_calls++;
    func->entry_time = entry_time;
    current_function = func;
    unlock(&profiler_semaphore);
}

void NO_PROFILE __cyg_profile_func_exit  (void *this_fn, void *call_site) 
{
    if(!lock(&profiler_semaphore))
    {
        return;
    }
    uint32_t exit_time = HAL_GetTick();
    if(!current_function || !current_function->parent)
    {
        profiler_reset();
        unlock(&profiler_semaphore);
        return;
    }
    current_function->total_time += exit_time - current_function->entry_time;
    current_function->entry_time = 0;
    current_function = current_function->parent;
    unlock(&profiler_semaphore);
}
#endif