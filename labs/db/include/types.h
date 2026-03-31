#ifndef _TYPES_H_
#define _TYPES_H_

#include <stddef.h>

#define SUBJECT_NAME_SIZE 61
#define PERSON_FULL_NAME_SIZE 101
#define PERSON_BIRTHDAY_SIZE 11
#define PERSON_GROUP_NUMBER_SIZE 21

typedef enum
{
    EXAM,
    CREDIT,
    COURSE_WORK
} attestation_type;

typedef struct
{
    char name[SUBJECT_NAME_SIZE];
    unsigned char lec_room;
    unsigned char lab_room;
    unsigned char hours;
    attestation_type attestation;
} subject;

typedef struct
{
    char full_name[PERSON_FULL_NAME_SIZE];
    char birthday[PERSON_BIRTHDAY_SIZE];
    char group_number[PERSON_GROUP_NUMBER_SIZE];
    size_t subjects_count;
    subject subjects[];
} person;

#endif
