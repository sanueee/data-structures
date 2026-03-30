#include "../include/cmd.h"

static void invite_user()
{
    fprintf(stdout, "> ");
    fflush(stdout);
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