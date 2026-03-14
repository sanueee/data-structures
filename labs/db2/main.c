#include <stdio.h>
#include <string.h>
#include "include/types.h"
#include "include/cmd.h"

// insert delete update search
// from cmd/file to console/file
// input.txt - insert, select, delete, update, uniq
// +checks for conditions
// if incorret - console: incorrect command: 20 symbols...
// memstat.txt - counters for malloc, calloc, realloc, free.

int main(int argc, char** argv)
{
    FILE *input  = stdin; // по умолчанию консоль
    FILE *output = fopen("output.txt", "w");
    list data_base = {NULL, 0};
    char line[1024];

    if (argc == 2 && (*argv)[1] == 'f')
    {
        input = fopen(argv[1], "r");

        if (!input)
        {
            fprintf(stderr, "file opening error"); return 1;
        }

        while (fgets(line, sizeof(line), input))
        {
            line[strcspn(line, "\n")] = '\0';
            if (strlen(line) == 0) continue;
            execute_command(line, &data_base, output);
        }

        fclose(input);
    }

    while (1)
    {
        printf(":> ");

        line[strcspn(line, "\n")] = '\0';
        if (strlen(line) == 0) continue;

        if (strcmp(line, "exit") == 0) break;

        execute_command(line, &data_base, output);
    }

    return 0;
}