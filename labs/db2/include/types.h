#ifndef _TYPES_H_
#define _TYPES_H_

#include <time.h>

#define MAX_NAME 256

typedef enum
{
    cmd,
    file
} input_type;

typedef enum
{
    CMD_INSERT,
    CMD_SELECT,
    CMD_DELETE,
    CMD_UPDATE,
    CMD_UNIQ,
    CMD_UNKNOWN
} command;

typedef enum
{
    running,
    ready,
    paused,
    blocked,
    dying,
    sleeping        
} process_status;

typedef struct
{
    int pid;
    char name[MAX_NAME];
    int priority;
    time_t kern_tm;
    time_t file_tm;
    float cpu_usage;
    process_status status;
} process;

typedef struct node
{
    process data;
    struct node* next;
} node;

typedef struct
{
    node* head;
    size_t size;
} list;

#endif
