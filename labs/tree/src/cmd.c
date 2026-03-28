#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "../include/ast.h"
#include "../include/parser.h"
#include "../include/eval.h"
#include "../include/cmd.h"
#include "../include/memstat.h"

static ASTNode *g_tree = NULL;

static void set_tree(ASTNode *node)
{
    if (g_tree)
        ast_free(g_tree);
    g_tree = node;
}

static const char *skip_spaces(const char *s)
{
    while (*s == ' ' || *s == '\t')
        s++;
    return s;
}

void execute_command(const char *line, FILE *out)
{
    const char *p = line;
    while (*p && !isspace((unsigned char)*p))
        p++;

    char keyword[32] = {0};
    int klen = (int)(p - line);
    if (klen >= 32)
        klen = 31;
    strncpy(keyword, line, klen);

    const char *arg = skip_spaces(p);
    if (strcmp(keyword, "exit") == 0)
    {
        return 1;
    }
    else if (strcmp(keyword, "parse") == 0)
    {
        ASTNode *node = parse_natural(arg);
        set_tree(node);
        fprintf(out, node ? "success\n" : "incorrect\n");
    }
    else if (strcmp(keyword, "load_prf") == 0)
    {
        ASTNode *node = parse_prefix(arg);
        set_tree(node);
        fprintf(out, node ? "success\n" : "incorrect\n");
    }
    else if (strcmp(keyword, "load_pst") == 0)
    {
        ASTNode *node = parse_postfix(arg);
        set_tree(node);
        fprintf(out, node ? "success\n" : "incorrect\n");
    }
    else if (strcmp(keyword, "save_prf") == 0)
    {
        if (!g_tree)
            fprintf(out, "not_loaded\n");
        else
            ast_save_prf(g_tree, out);
    }
    else if (strcmp(keyword, "save_pst") == 0)
    {
        if (!g_tree)
            fprintf(out, "not_loaded\n");
        else
            ast_save_pst(g_tree, out);
    }
    else if (strcmp(keyword, "eval") == 0)
    {
        if (!g_tree)
        {
            fprintf(out, "not_loaded\n");
            return;
        }

        VarBinding bindings[128];
        int n = 0;
        const char *q = arg;

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
                fprintf(out, "error\n");
                return;
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
                fprintf(out, "error\n");
                return;
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
            fprintf(out, "%lld\n", result);
        else
            fprintf(out, "error\n");
    }
    else
    {
        fprintf(out, "unknown command\n");
    }
}

void cmd_cleanup(void)
{
    set_tree(NULL);
    write_memstat();
}
