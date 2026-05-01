#include "../include/gen.h"
#include "../../lcg/lcg.h" // было: <stdlib.h> rand/srand; стало: ЛКГ из лабы 1
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

static const char *first_names[] = {
    "Алекс", "Рома", "Влад", "Данияр",
    "Артём", "Пашка", "Валера", "Машка",
    "Некит", "Полина", "Анна", "Аня", "Вика"};
static const char *last_names[] = {
    "Иванов", "Петров", "Сидоров", "Смирнов", "Кузнецов",
    "Попов", "Васильев", "Соколов", "Михайлов", "Новиков",
    "Фёдоров", "Морозов", "Волков", "Алексеев", "Лебедев"};
static const char *patronymics[] = {
    "Александрович", "Дмитриевич", "Максимович", "Иванович", "Артёмович",
    "Михайлович", "Даниилович", "Егорович", "Никитич", "Андреевич",
    "Александровна", "Дмитриевна", "Максимовна", "Ивановна", "Артёмовна"};
static const char *disciplines[] = {
    "Математический анализ", "Линейная алгебра", "Дискретная математика",
    "Математическое моделирование", "Архитектура аппаратных средств", "Операционные системы",
    "Администрирование баз данных", "Алгоритмизация и программирование", "Компьютерные сети",
    "Психология", "Физика", "История России",
    "Иностранный язык", "Философия", "Экономика"};

void init_gen(void)
{
    // было: srand(time(NULL));
    // стало: засеваем ЛКГ параметрами, прошедшими freq/gap-тесты в лабе 1
    lcg_seed(1664525ULL, 1013904223ULL, 4294967296ULL, (ull)time(NULL));
}

person *generate_student(size_t min_subjects, size_t max_subjects)
{
    // было: size_t count = min_subjects + rand() % (max_subjects - min_subjects + 1);
    size_t count = (size_t)lcg_range(min_subjects, max_subjects);
    person *p = malloc(sizeof(person) + count * sizeof(subject));
    if (!p)
        return NULL;

    // было: last_names[rand() % 15], first_names[rand() % 13], patronymics[rand() % 15]
    snprintf(p->full_name, 100, "%s %s %s",
             last_names[lcg_range(0, 14)],
             first_names[lcg_range(0, 12)],
             patronymics[lcg_range(0, 14)]);

    // было: 1 + rand() % 28, 1 + rand() % 12, 1999 + rand() % 7
    sprintf(p->birthday, "%02d.%02d.%04d",
            (int)lcg_range(1, 28),
            (int)lcg_range(1, 12),
            (int)lcg_range(1999, 2005));

    // было: rand() % 10000 (было переполнение)
    snprintf(p->group_number, PERSON_GROUP_NUMBER_SIZE, "%04d",
            (int)lcg_range(0, 9999));

    p->subjects_count = count;
    for (size_t i = 0; i < count; i++)
    {
        // было: disciplines[rand() % 15]
        strncpy(p->subjects[i].name, disciplines[lcg_range(0, 14)], SUBJECT_NAME_SIZE);
        p->subjects[i].name[SUBJECT_NAME_SIZE - 1] = '\0';
        // было: rand() % 256, rand() % 256, rand() % 256, rand() % 3
        p->subjects[i].lec_room    = (int)lcg_range(0, 255);
        p->subjects[i].lab_room    = (int)lcg_range(0, 255);
        p->subjects[i].hours       = (int)lcg_range(0, 255);
        p->subjects[i].attestation = (int)lcg_range(0, 2);
    }

    return p;
}
