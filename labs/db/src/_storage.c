#include "../include/_storage.h"
#include <stdlib.h>

static person **students = NULL;
static size_t count = 0;
static size_t capacity = 0;

int add_student(person *p)
{
    if (count >= capacity)
    {
        size_t new_cap = capacity == 0 ? 1000 : capacity * 2;
        person **new_arr = realloc(students, new_cap * sizeof(person *));
        if (!new_arr)
            return -1;
        students = new_arr;
        capacity = new_cap;
    }
    students[count++] = p;
    return 0;
}

size_t get_count(void)
{
    return count;
}

person *get_student(size_t index)
{
    return index < count ? students[index] : NULL;
}

size_t get_total_size(void)
{
    size_t size = 0;
    for (size_t i = 0; i < count; i++)
        size += sizeof(person) + students[i]->subjects_count * sizeof(subject);
    return size;
}

bool can_allocate(size_t n, size_t max_subjects)
{
    size_t estimated_size = n * (sizeof(person) + max_subjects * sizeof(subject));
    void *test = malloc(estimated_size);
    if (!test)
        return 0;
    free(test);

    return 1;
}

void clean(void)
{
    for (size_t i = 0; i < count; i++)
        free(students[i]);
    free(students);
    students = NULL;
    count = 0;
    capacity = 0;
}
