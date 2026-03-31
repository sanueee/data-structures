#include "include/gen.h"
#include "include/_storage.h"
#include "include/commands.h"
#include <stdio.h>
#include <string.h>
// #include <windows.h>

//void __set_locale__(void);

int main(void)
{
    //__set_locale__();

    init_gen();

    char buf[256];
    while (1)
    {
        printf("> ");
        if (!fgets(buf, 256, stdin))
            break;

        char cmd[20];
        int n = 0;

        if (sscanf(buf, "%19s %d", cmd, &n) >= 1)
        {
            if (strcmp(cmd, "gen") == 0)
            {
                size_t min = 10, max = 20;
                char *s;
                if ((s = strstr(buf, "--min-disciplines")))
                    sscanf(s, "--min-disciplines %zu", &min);
                if ((s = strstr(buf, "--max-disciplines")))
                    sscanf(s, "--max-disciplines %zu", &max);
                if (n > 0)
                    gen(n, min, max);
                else
                    fprintf(stderr, "error: invalid number\n");
            }
            else if (strcmp(cmd, "print_students") == 0)
                print_students(n);
            else if (strcmp(cmd, "get_size") == 0)
                get_size();
            else if (strcmp(cmd, "clean") == 0)
            {
                clean();
                printf("cleaned\n");
            }
            else if (strcmp(cmd, "exit") == 0)
                break;
            else
                fprintf(stderr, "error: unknown command\n");
        }
    }

    clean();
    return 0;
}
/*
void __set_locale__(void)
{
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
}
*/