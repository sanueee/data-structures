#include "../include/cmd.h"
#include "../include/parser.h"
#include "../include/ast.h"
#include "../include/eval.h"

#include <stdio.h>

static ASTNode *g_tree = NULL;

static const char *skip_spaces(const char *s)
{
    while (*s == ' ' || *s == '\t')
        s++;
    return s;
}

static void set_tree(ASTNode *node)
{
    if (g_tree)
        ast_free(g_tree);
    g_tree = node;
}

int execute_command(const char* buffer, FILE *output)
{
    const char* p = buffer;
    while (*p && !isspace((unsigned char)*p))
    {
        p++;
    }

    char cmd[32] = {0};
    int cmd_len = p - buffer;
    if (cmd_len >= 32)
    {
        cmd_len = 31;
    }
    strncpy(cmd, buffer, cmd_len);
    const char *args = skip_spaces(p);

    if (strcmp(cmd, "exit") == 0)
    {
        return -1;
    }
    else if (strcmp(cmd, "parse") == 0)
    {
        ASTNode *node = parse_expr(args);
        set_tree(node);
        fprintf(output, node ? "success\n" : "incorrect\n");
        return 1;
    }
    else if (strcmp(cmd, "load_prf") == 0) 
    {
        ASTNode *node = parse_prefix(args);
        set_tree(node);
        fprintf(output, node ? "success\n" : "incorrect\n");
        return 1;
    }
    else if (strcmp(cmd, "load_pst") == 0)
    {
        ASTNode *node = parse_postfix(args);
        set_tree(node);
        fprintf(output, node ? "success\n" : "incorrect\n");
        return 1;
    }
    else if (strcmp(cmd, "save_prf") == 0) 
    {
        if (g_tree)
        {
            ast_save_prf(g_tree, output);
        }
        else
        {
            fprintf(output, "not loaded\n");
        }
        return 1;
    }
    else if (strcmp(cmd, "save_pst") == 0)
    {
        if (g_tree)
        {
            ast_save_pst(g_tree, output);
        }
        else
        {
            fprintf(output, "not loaded\n");
        }
        return 1;
    }
    else if (strcmp(cmd, "eval") == 0)
    {
        if (g_tree == NULL)
        {
            fprintf(output, "not loaded\n");
            return 1;
        }
        VarBinding bindings[128];
        int n = 0;
        const char *q = args;

        while (*q)
        {
            while (*q == ' ' || *q == '\t')
                q++;
            if (*q == '\0')
                break;

            char name[64];
            int j = 0;
            while (isalpha((unsigned char)*q) && j < 63)
                name[j++] = *q++;
            name[j] = '\0';

            if (j == 0 || *q != '=')
            {
                fprintf(output, "error: '=' not found\n");
                return 1;
            }
            q++;

            int neg = 0;
            if (*q == '-')
            {
                neg = 1;
                q++;
            }
            if (!isdigit((unsigned char)*q))
            {
                fprintf(output, "error: not digit\n");
                return 1;
            }

            long long val = 0;
            while (isdigit((unsigned char)*q))
                val = val * 10 + (*q++ - '0');
            if (neg)
                val = -val;

            if (n < 128)
            {
                strncpy(bindings[n].name, name, 63);
                bindings[n].name[63] = '\0';
                bindings[n].value = val;
                n++;
            }

            while (*q == ' ' || *q == '\t')
                q++;
            if (*q == ',')
                q++;
        }

        long long result;
        if (ast_eval(g_tree, bindings, n, &result))
        {
            fprintf(output, "%lld\n", result);
        }
        else
        {
            fprintf(output, "error while eval\n");
        }
        return 1;
    }
    else
    {
        fprintf(output, "incorrect command\n");
    }

    return 1;
}

void cmd_clean()
{
    set_tree(NULL);
    write_memstat();
}