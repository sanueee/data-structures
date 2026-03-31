#include "../include/cmd.h"

static void invite_user()
{
    fprintf(stdout, "> ");
    fflush(stdout);
}

static void strip_newline(char *s)
{
    size_t len = strlen(s);
    if (len > 0 && s[len - 1] == '\n')
        s[len - 1] = '\0';
    if (len > 1 && s[len - 2] == '\r')
        s[len - 2] = '\0';
}

int main(int argc, char** argv)
{
    FILE* input = NULL;
    FILE* output = fopen("output.txt", "w");
    if (output == NULL)
    {
        fprintf(stderr, "output.txt cannot open");
        return 1;
    }

    // с файла
    if (argc > 1)
    {
        input = fopen("input.txt", "r");
        if (input == NULL)
        {
            fprintf(stderr, "input.txt cannot open");
            return 1;
        }

        char buffer[4096] = {0};
        int res;
        while (fgets(buffer, sizeof(buffer), input))
        {
            strip_newline(buffer);
            res = execute_command(buffer, output);
        }

        fclose(input);
    }
    // с консоли
    char buffer[4096] = {0};
    int res;
    invite_user();

    while (fgets(buffer, sizeof(buffer), stdin))
    {
        strip_newline(buffer);
        if (buffer[0] != '\0')
        {
            res = execute_command(buffer, output);
            if (res == -1) { break; }
        }
        invite_user();
    }

    cmd_clean();
    fclose(output);
    return 0;
}