#include "../include/gen.h"
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
    srand(time(NULL));
}

person *generate_student(size_t min_subjects, size_t max_subjects)
{
    size_t count = min_subjects + rand() % (max_subjects - min_subjects + 1);
    person *p = malloc(sizeof(person) + count * sizeof(subject));
    if (!p)
        return NULL;

    snprintf(p->full_name, 100, "%s %s %s",
             last_names[rand() % 15],
             first_names[rand() % 13],
             patronymics[rand() % 15]);

    sprintf(p->birthday, "%02d.%02d.%04d",
            1 + rand() % 28,
            1 + rand() % 12,
            1999 + rand() % 7);

    snprintf(p->group_number, PERSON_GROUP_NUMBER_SIZE, "%04d",
            rand() % 10000); // было переполнение

    p->subjects_count = count;
    for (size_t i = 0; i < count; i++)
    {
        strncpy(p->subjects[i].name, disciplines[rand() % 15], SUBJECT_NAME_SIZE);
        p->subjects[i].name[SUBJECT_NAME_SIZE - 1] = '\0';
        p->subjects[i].lec_room = rand() % 256;
        p->subjects[i].lab_room = rand() % 256;
        p->subjects[i].hours = rand() % 256;
        p->subjects[i].attestation = rand() % 3;
    }

    return p;
}
