#ifndef _TYPES_H_
#define _TYPES_H_

#include <time.h>

#define MAX_NAME 256
#define MAX_NAMES 16
#define MAX_CONDS 16
#define MAX_FV 16

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

typedef struct 
{
    char f[64]; 
    char v[256]; 
} fv_pair;

typedef enum 
{ 
    OP_LT, 
    OP_LE, 
    OP_GT, 
    OP_GE, 
    OP_EQ, 
    OP_NE, 
    OP_IN 
} op_t;

typedef struct 
{ 
    char f[64];
    op_t op; 
    char v[256]; 
} cond;

#endif
