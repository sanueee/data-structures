#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/memstat.h"
#include "../include/cmd.h"

static void strip_newline(char *s)
{
    size_t len = strlen(s);
    if (len > 0 && s[len - 1] == '\n')
        s[len - 1] = '\0';
    if (len > 1 && s[len - 2] == '\r')
        s[len - 2] = '\0';
}

int main(int argc, char *argv[])
{
    FILE *output = fopen("output.txt", "w");
    if (!output)
    {
        fprintf(stderr, "error: cannot open output.txt\n");
        return 1;
    }

    if (argc > 1)
    {
        FILE *input = fopen(argv[1], "r");
        if (!input)
        {
            fprintf(stderr, "error: cannot open %s\n", argv[1]);
            fclose(output);
            return 1;
        }

        char line[4096];
        while (fgets(line, sizeof(line), input))
        {
            strip_newline(line);
            if (line[0] == '\0')
                continue;
            execute_command(line, output);
        }

        fclose(input);
    }
    char line[4096];

    printf("> ");
    fflush(stdout);

    while (fgets(line, sizeof(line), stdin))
    {
        strip_newline(line);
        if (line[0] != '\0')
            execute_command(line, output);

        printf("> ");
        fflush(stdout);
    }

    cmd_cleanup();
    fclose(output);
    return 0;
}
