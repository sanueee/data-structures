#include "include/avl.h"
#include <stdio.h>

static void process(FILE *in, int interactive)
{
    node *root = NULL;
    char cmd[32];

    while (1)
    {
        if (interactive)
        {
            printf("> ");
            fflush(stdout);
        }
        if (fscanf(in, "%31s", cmd) != 1)
            break;

        if (strcmp(cmd, "insert") == 0)
        {
            int x;
            fscanf(in, "%d", &x);
            root = insert(root, x);
        }
        else if (strcmp(cmd, "delete") == 0)
        {
            int x;
            fscanf(in, "%d", &x);
            root = delete_node(root, x);
        }
        else if (strcmp(cmd, "find") == 0)
        {
            int x;
            fscanf(in, "%d", &x);
            fprintf(stdout, find(root, x) ? "YES\n" : "NO\n");
        }
        else if (strcmp(cmd, "print") == 0)
        {
            print(root);
            fprintf(stdout, "\n");
        }
        else if (strcmp(cmd, "kth") == 0)
        {
            int k;
            fscanf(in, "%d", &k);
            if (k < 0 || k > get_size(root))
            {
                fprintf(stdout, "NO SUCH ELEMENT\n");
            }
            else
            {
                node *res = kth(root, k - 1);
                if (res)
                    fprintf(stdout, "%d\n", res->value);
                else
                    fprintf(stdout, "NO SUCH ELEMENT\n");
            }
        }
        else if (strcmp(cmd, "range_count") == 0)
        {
            int l, r;
            fscanf(in, "%d %d", &l, &r);
            fprintf(stdout, "%llu\n", range_count(root, l, r));
        }
        else if (strcmp(cmd, "prev") == 0)
        {
            int x, result = 0;
            fscanf(in, "%d", &x);
            if (prev(root, x, &result))
                fprintf(stdout, "%d\n", result);
            else
                fprintf(stdout, "NONE\n");
        }
        else if (strcmp(cmd, "next") == 0)
        {
            int x, result = 0;
            fscanf(in, "%d", &x);
            if (next(root, x, &result))
                fprintf(stdout, "%d\n", result);
            else
                fprintf(stdout, "NONE\n");
        }
        else if (strcmp(cmd, "size") == 0)
        {
            int x;
            fscanf(in, "%d", &x);
            node *cur = root;
            int found = 0;
            while (cur)
            {
                if (x == cur->value)
                {
                    fprintf(stdout, "%llu\n", cur->size);
                    found = 1;
                    break;
                }
                cur = (x < cur->value) ? cur->left : cur->right;
            }
            if (!found)
                fprintf(stdout, "NO SUCH ELEMENT\n");
        }
    }
}

int main(int argc, char **argv)
{
    if (argc == 2)
    {
        FILE *f = fopen(argv[1], "r");
        if (!f)
        {
            fprintf(stderr, "error: cannot open file %s\n", argv[1]);
            return 1;
        }
        process(f, 0);
        fclose(f);
    }
    else
    {
        process(stdin, 1);
    }
    return 0;
}