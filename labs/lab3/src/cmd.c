#include "../include/cmd.h"
#include "../include/parser.h"
#include "../include/ast.h"

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
            fprintf(output, "not_loaded\n");
            return 1;
        }
        
        return 1;
    }
}

void cmd_clean()
{
    set_tree(NULL);
    write_memstat();
}