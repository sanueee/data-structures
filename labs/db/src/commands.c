#include "../include/commands.h"
#include "../include/gen.h"
#include "../include/_storage.h"
#include <stdio.h>
#include <stdlib.h>

int gen(size_t n, size_t min_disc, size_t max_disc)
{
    if (min_disc > max_disc)
    {
        fprintf(stderr, "min-disciplines must be less than max-disciplines\n");
        return -1;
    }

    if (!can_allocate(n, max_disc))
    {
        fprintf(stderr, "no memory to generate %zu students\n", n);
        return -1;
    }

    for (size_t i = 0; i < n; i++)
    {
        person *p = generate_student(min_disc, max_disc);

        if (!p)
        {
            fprintf(stderr, "something went wrong while generating student %zu\n", i + 1);
            return -1;
        }

        if (add_student(p) != 0)
        {
            free(p);
            fprintf(stderr, "cannot add student in storage\n");
            return -1;
        }
    }

    printf("generated %zu students\n", n);
    return 0;
}

void get_size(void)
{
    size_t total_size = get_total_size();

    const char *units[] = {"bytes", "kylobytes", "megabytes", "gigabytes"};
    double size = (double)total_size;
    int unit_idx = 0;

    while (size >= 1024.0 && unit_idx < 3)
    {
        size /= 1024.0;
        unit_idx++;
    }

    printf("storage size: %.2f %s (%zu bytes)\n", size, units[unit_idx], total_size);
}

void print_students(int n)
{
    size_t count = (n == -1) ? get_count() : (size_t)n;
    size_t total = get_count();

    if (count > total)
    {
        count = total;
    }

    for (size_t i = 0; i < count; i++)
    {
        person *p = get_student(i);
        if (!p)
            continue;

        printf("\nСтудент %zu\n", i+1);
        printf("ФИО: %s\n", p->full_name);
        printf("Дата рождения: %s\n", p->birthday);
        printf("Группа: %s\n", p->group_number);
        printf("Количество предметов: %zu\n-----------------------\n", p->subjects_count);

        for (size_t j = 0; j < p->subjects_count; j++)
        {
            printf("%s - ", p->subjects[j].name);
            printf("лекции: ауд. %u, Лабы: ауд. %u, ", p->subjects[j].lec_room, p->subjects[j].lab_room);
            printf("часов: %u, аттестация: %s\n\n", p->subjects[j].hours, get_attestation(p->subjects[j].attestation));
        }
    }
}

char *get_attestation(attestation_type attestation)
{
    switch (attestation)
    {
    case EXAM:
        return "Экзамен";
        break;
    case COURSE_WORK:
        return "Курсовая работа";
        break;
    case CREDIT:
        return "Зачёт";
        break;
    default:
        return "error";
        break;
    }
}
