#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include "../include/list.h"
#include "../include/types.h"
#include "../include/memstat.h"
#include "../include/cmd.h"

static void out(FILE *f, const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vfprintf(f, fmt, ap);
    va_end(ap);
    va_start(ap, fmt);
    vfprintf(stdout, fmt, ap);
    va_end(ap);
}

static const char *status_str(process_status s)
{
    switch (s)
    {
    case running:
        return "running";
    case ready:
        return "ready";
    case paused:
        return "paused";
    case blocked:
        return "blocked";
    case dying:
        return "dying";
    case sleeping:
        return "sleeping";
    default:
        return "unknown";
    }
}

static int str_to_status(const char *s, process_status *out_s)
{
    if (!strcmp(s, "running"))
    {
        *out_s = running;
        return 1;
    }
    if (!strcmp(s, "ready"))
    {
        *out_s = ready;
        return 1;
    }
    if (!strcmp(s, "paused"))
    {
        *out_s = paused;
        return 1;
    }
    if (!strcmp(s, "blocked"))
    {
        *out_s = blocked;
        return 1;
    }
    if (!strcmp(s, "dying"))
    {
        *out_s = dying;
        return 1;
    }
    if (!strcmp(s, "sleeping"))
    {
        *out_s = sleeping;
        return 1;
    }
    return 0;
}

static int parse_time(const char *s, time_t *t)
{
    struct tm tm = {0};
    int d, mo, y, h = 0, mi = 0;
    if (sscanf(s, "%d.%d.%d_%d:%d", &d, &mo, &y, &h, &mi) < 3)
        return 0;
    tm.tm_mday = d;
    tm.tm_mon = mo - 1;
    tm.tm_year = y - 1900;
    tm.tm_hour = h;
    tm.tm_min = mi;
    tm.tm_isdst = -1;
    *t = mktime(&tm);
    return (*t != (time_t)-1);
}

static void fmt_time(time_t t, char *buf, size_t sz)
{
    struct tm *tm = localtime(&t);
    strftime(buf, sz, "%d.%m.%Y_%H:%M", tm);
}

static const char *FIELDS[] =
    {
        "pid", "name", "priority", "kern_tm", "file_tm", "cpu_usage", "status", NULL};

static int valid_field(const char *f)
{
    for (int i = 0; FIELDS[i]; i++)
        if (!strcmp(f, FIELDS[i]))
            return 1;
    return 0;
}

static void get_field(const process *p, const char *f, char *buf, size_t sz)
{
    if (!strcmp(f, "pid"))
        snprintf(buf, sz, "%d", p->pid);
    else if (!strcmp(f, "name"))
        snprintf(buf, sz, "%s", p->name);
    else if (!strcmp(f, "priority"))
        snprintf(buf, sz, "%d", p->priority);
    else if (!strcmp(f, "kern_tm"))
        fmt_time(p->kern_tm, buf, sz);
    else if (!strcmp(f, "file_tm"))
        fmt_time(p->file_tm, buf, sz);
    else if (!strcmp(f, "cpu_usage"))
        snprintf(buf, sz, "%.2f", p->cpu_usage);
    else if (!strcmp(f, "status"))
        snprintf(buf, sz, "%s", status_str(p->status));
    else
        snprintf(buf, sz, "?");
}

static int set_field(process *p, const char *f, const char *v)
{
    if (!strcmp(f, "pid"))
    {
        char *e;
        long n = strtol(v, &e, 10);
        if (*e)
            return 0;
        p->pid = (int)n;
    }
    else if (!strcmp(f, "name"))
    {
        strncpy(p->name, v, MAX_NAME - 1);
        p->name[MAX_NAME - 1] = '\0';
    }
    else if (!strcmp(f, "priority"))
    {
        char *e;
        long n = strtol(v, &e, 10);
        if (*e)
            return 0;
        p->priority = (int)n;
    }
    else if (!strcmp(f, "kern_tm"))
    {
        if (!parse_time(v, &p->kern_tm))
            return 0;
    }
    else if (!strcmp(f, "file_tm"))
    {
        if (!parse_time(v, &p->file_tm))
            return 0;
    }
    else if (!strcmp(f, "cpu_usage"))
    {
        char *e;
        float n = strtof(v, &e);
        if (*e)
            return 0;
        p->cpu_usage = n;
    }
    else if (!strcmp(f, "status"))
    {
        if (!str_to_status(v, &p->status))
            return 0;
    }
    else
    {
        return 0;
    }
    return 1;
}

static int parse_fv(const char *src, fv_pair *pairs)
{
    char buf[4096];
    strncpy(buf, src, sizeof(buf) - 1);
    buf[sizeof(buf) - 1] = '\0';
    int n = 0;
    char *tok = strtok(buf, ",");
    while (tok && n < MAX_FV)
    {
        char *eq = strchr(tok, '=');
        if (!eq)
            return -1;
        *eq = '\0';
        strncpy(pairs[n].f, tok, sizeof(pairs[n].f) - 1);
        strncpy(pairs[n].v, eq + 1, sizeof(pairs[n].v) - 1);
        n++;
        tok = strtok(NULL, ",");
    }
    return n;
}

static int parse_names(const char *src, char names[][64], int max)
{
    char buf[1024];
    strncpy(buf, src, sizeof(buf) - 1);
    buf[sizeof(buf) - 1] = '\0';
    int n = 0;
    char *tok = strtok(buf, ",");
    while (tok && n < max)
    {
        strncpy(names[n], tok, 63);
        names[n][63] = '\0';
        n++;
        tok = strtok(NULL, ",");
    }
    return n;
}

static void strip_quotes(char *s)
{
    size_t len = strlen(s);
    if (len >= 2 && s[0] == '\'' && s[len - 1] == '\'')
    {
        memmove(s, s + 1, len - 2);
        s[len - 2] = '\0';
    }
}

/* Парсит одно условие, возвращает 1/0 */
static int parse_cond(const char *src, cond *c)
{
    /* /in/ */
    const char *sl = strchr(src, '/');
    if (sl)
    {
        size_t fl = sl - src;
        strncpy(c->f, src, fl < 63 ? fl : 63);
        c->f[fl < 63 ? fl : 63] = '\0';
        const char *sl2 = strchr(sl + 1, '/');
        if (!sl2)
            return 0;
        char op[32];
        size_t ol = sl2 - sl - 1;
        strncpy(op, sl + 1, ol < 31 ? ol : 31);
        op[ol < 31 ? ol : 31] = '\0';
        if (!strcmp(op, "in"))
            c->op = OP_IN;
        else
            return 0;
        strncpy(c->v, sl2 + 1, 255);
        c->v[255] = '\0';
        return 1;
    }
    /* двухсимвольные операторы первыми, чтобы не срабатывал '<' раньше '<=' */
    static const struct
    {
        const char *str;
        op_t op;
    } OPS[] = {
        {"<=", OP_LE}, {">=", OP_GE}, {"!=", OP_NE}, {"<", OP_LT}, {">", OP_GT}, {"=", OP_EQ}};
    for (int i = 0; i < 6; i++)
    {
        const char *pos = strstr(src, OPS[i].str);
        if (!pos)
            continue;
        size_t fl = pos - src;
        strncpy(c->f, src, fl < 63 ? fl : 63);
        c->f[fl < 63 ? fl : 63] = '\0';
        c->op = OPS[i].op;
        strncpy(c->v, pos + strlen(OPS[i].str), 255);
        c->v[255] = '\0';
        strip_quotes(c->v);
        return 1;
    }
    return 0;
}

/* Разбивает оставшуюся строку на условия через пробел */
static int parse_conds(char *rest, cond *conds)
{
    int n = 0;
    char *tok = strtok(rest, " \t");
    while (tok && n < MAX_CONDS)
    {
        if (!parse_cond(tok, &conds[n]))
            return -1;
        n++;
        tok = strtok(NULL, " \t");
    }
    return n;
}

/* Вычисление одного условия */
static int eval_cond(const process *p, const cond *c)
{
    char fval[256];
    get_field(p, c->f, fval, sizeof(fval));

    if (c->op == OP_IN)
    {
        /* ['val1','val2',...] */
        char list[256];
        strncpy(list, c->v, 255);
        char *s = list;
        if (*s == '[')
            s++;
        size_t len = strlen(s);
        if (len && s[len - 1] == ']')
            s[len - 1] = '\0';
        char *tok = strtok(s, ",");
        while (tok)
        {
            char v[256];
            strncpy(v, tok, 255);
            strip_quotes(v);
            if (!strcmp(fval, v))
                return 1;
            tok = strtok(NULL, ",");
        }
        return 0;
    }

    /* сравнение: числовое для pid/priority, время для *_tm, float для cpu_usage, иначе строка */
    int cmp;
    if (!strcmp(c->f, "pid") || !strcmp(c->f, "priority"))
    {
        long a = atol(fval), b = atol(c->v);
        cmp = (a > b) - (a < b);
    }
    else if (!strcmp(c->f, "kern_tm") || !strcmp(c->f, "file_tm"))
    {
        time_t ta, tb;
        if (!parse_time(fval, &ta) || !parse_time(c->v, &tb))
            return 0;
        cmp = (ta > tb) - (ta < tb);
    }
    else if (!strcmp(c->f, "cpu_usage"))
    {
        float a = atof(fval), b = atof(c->v);
        cmp = (a > b) - (a < b);
    }
    else
    {
        cmp = strcmp(fval, c->v);
    }

    switch (c->op)
    {
    case OP_LT:
        return cmp < 0;
    case OP_LE:
        return cmp <= 0;
    case OP_GT:
        return cmp > 0;
    case OP_GE:
        return cmp >= 0;
    case OP_EQ:
        return cmp == 0;
    case OP_NE:
        return cmp != 0;
    default:
        return 0;
    }
}

static int eval_all(const process *p, cond *conds, int n)
{
    for (int i = 0; i < n; i++)
        if (!eval_cond(p, &conds[i]))
            return 0;
    return 1;
}

static node *new_node(const process *p)
{
    node *n = MALLOC(sizeof(node));
    if (!n)
        return NULL;
    n->data = *p;
    n->next = NULL;
    return n;
}

static void list_append(list *db, const process *p)
{
    node *n = new_node(p);
    if (!n)
        return;
    if (!db->head)
    {
        db->head = n;
    }
    else
    {
        node *cur = db->head;
        while (cur->next)
            cur = cur->next;
        cur->next = n;
    }
    db->size++;
}

static void print_record(FILE *f, const process *p,
                         char names[][64], int nnames)
{
    for (int i = 0; i < nnames; i++)
    {
        char v[256];
        get_field(p, names[i], v, sizeof(v));
        out(f, "%s%s=%s", i ? " " : "", names[i], v);
    }
    out(f, "\n");
}

/* ── команды ──────────────────────────────────────────────────────── */

static void cmd_insert(const char *args, list *db, FILE *f)
{
    fv_pair pairs[MAX_FV];
    int n = parse_fv(args, pairs);
    if (n < 0)
    {
        out(f, "incorrect %.20s\n", args);
        return;
    }

    /* все 7 полей, без дублей */
    int found[7] = {0};
    for (int i = 0; i < n; i++)
    {
        int idx = -1;
        for (int j = 0; FIELDS[j]; j++)
            if (!strcmp(pairs[i].f, FIELDS[j]))
            {
                idx = j;
                break;
            }
        if (idx < 0 || !valid_field(pairs[i].f))
        {
            out(f, "incorrect %.20s\n", args);
            return;
        }
        if (found[idx])
        {
            out(f, "incorrect %.20s\n", args);
            return;
        }
        found[idx] = 1;
    }
    for (int j = 0; FIELDS[j]; j++)
        if (!found[j])
        {
            out(f, "incorrect %.20s\n", args);
            return;
        }

    process p;
    memset(&p, 0, sizeof(p));
    for (int i = 0; i < n; i++)
        if (!set_field(&p, pairs[i].f, pairs[i].v))
        {
            out(f, "incorrect %.20s\n", args);
            return;
        }

    list_append(db, &p);
    out(f, "insert: ok\n");
}

static void cmd_select(const char *args, list *db, FILE *f)
{
    char buf[4096];
    strncpy(buf, args, sizeof(buf) - 1);
    buf[sizeof(buf) - 1] = '\0';

    /* первый токен — имена полей */
    char *sp = buf;
    while (*sp == ' ')
        sp++;
    char *fe = sp;
    while (*fe && *fe != ' ' && *fe != '\t')
        fe++;
    char names_str[512];
    size_t fl = fe - sp;
    strncpy(names_str, sp, fl < 511 ? fl : 511);
    names_str[fl < 511 ? fl : 511] = '\0';

    char names[MAX_NAMES][64];
    int nn = parse_names(names_str, names, MAX_NAMES);
    for (int i = 0; i < nn; i++)
        if (!valid_field(names[i]))
        {
            out(f, "incorrect %.20s\n", args);
            return;
        }

    cond conds[MAX_CONDS];
    int nc = 0;
    if (*fe)
    {
        char rest[4096];
        strncpy(rest, fe + 1, sizeof(rest) - 1);
        nc = parse_conds(rest, conds);
        if (nc < 0)
        {
            out(f, "incorrect %.20s\n", args);
            return;
        }
    }

    int cnt = 0;
    for (node *cur = db->head; cur; cur = cur->next)
        if (eval_all(&cur->data, conds, nc))
            cnt++;

    out(f, "select: %d\n", cnt);
    for (node *cur = db->head; cur; cur = cur->next)
        if (eval_all(&cur->data, conds, nc))
            print_record(f, &cur->data, names, nn);
}

static void cmd_delete(const char *args, list *db, FILE *f)
{
    cond conds[MAX_CONDS];
    int nc = 0;
    if (args && *args)
    {
        char buf[4096];
        strncpy(buf, args, sizeof(buf) - 1);
        nc = parse_conds(buf, conds);
        if (nc < 0)
        {
            out(f, "incorrect %.20s\n", args);
            return;
        }
    }

    int del = 0;
    node **pp = &db->head;
    while (*pp)
    {
        node *cur = *pp;
        if (eval_all(&cur->data, conds, nc))
        {
            *pp = cur->next;
            FREE(cur);
            db->size--;
            del++;
        }
        else
        {
            pp = &cur->next;
        }
    }
    out(f, "delete: %d\n", del);
}

static void cmd_update(const char *args, list *db, FILE *f)
{
    char buf[4096];
    strncpy(buf, args, sizeof(buf) - 1);
    buf[sizeof(buf) - 1] = '\0';

    char *sp = buf;
    while (*sp == ' ')
        sp++;
    char *fe = sp;
    while (*fe && *fe != ' ' && *fe != '\t')
        fe++;

    char fv_str[2048];
    size_t fl = fe - sp;
    strncpy(fv_str, sp, fl < 2047 ? fl : 2047);
    fv_str[fl < 2047 ? fl : 2047] = '\0';

    fv_pair pairs[MAX_FV];
    int n = parse_fv(fv_str, pairs);
    if (n < 0)
    {
        out(f, "incorrect %.20s\n", args);
        return;
    }
    for (int i = 0; i < n; i++)
        if (!valid_field(pairs[i].f))
        {
            out(f, "incorrect %.20s\n", args);
            return;
        }

    cond conds[MAX_CONDS];
    int nc = 0;
    if (*fe)
    {
        char rest[4096];
        strncpy(rest, fe + 1, sizeof(rest) - 1);
        nc = parse_conds(rest, conds);
        if (nc < 0)
        {
            out(f, "incorrect %.20s\n", args);
            return;
        }
    }

    int upd = 0;
    for (node *cur = db->head; cur; cur = cur->next)
    {
        if (eval_all(&cur->data, conds, nc))
        {
            for (int i = 0; i < n; i++)
                set_field(&cur->data, pairs[i].f, pairs[i].v);
            upd++;
        }
    }
    out(f, "update: %d\n", upd);
}

static void cmd_uniq(const char *args, list *db, FILE *f)
{
    char buf[1024];
    strncpy(buf, args, sizeof(buf) - 1);
    char names[MAX_NAMES][64];
    int nn = parse_names(buf, names, MAX_NAMES);
    for (int i = 0; i < nn; i++)
        if (!valid_field(names[i]))
        {
            out(f, "incorrect %.20s\n", args);
            return;
        }

    int del = 0;
    node **pp = &db->head;
    while (*pp)
    {
        node *cur = *pp;
        /* ищем дубликат позже в списке */
        int dup = 0;
        for (node *later = cur->next; later && !dup; later = later->next)
        {
            int match = 1;
            for (int i = 0; i < nn && match; i++)
            {
                char v1[256], v2[256];
                get_field(&cur->data, names[i], v1, sizeof(v1));
                get_field(&later->data, names[i], v2, sizeof(v2));
                if (strcmp(v1, v2))
                    match = 0;
            }
            if (match)
                dup = 1;
        }
        if (dup)
        {
            *pp = cur->next;
            FREE(cur);
            db->size--;
            del++;
        }
        else
            pp = &cur->next;
    }
    out(f, "uniq: %d\n", del);
}

void execute_command(char *line, list *db, FILE *f)
{
    char kw[32];
    if (sscanf(line, "%31s", kw) != 1)
        return;

    const char *args = line + strlen(kw);
    while (*args == ' ' || *args == '\t')
        args++;

    if (!strcmp(kw, "insert"))
        cmd_insert(args, db, f);
    else if (!strcmp(kw, "select"))
        cmd_select(args, db, f);
    else if (!strcmp(kw, "delete"))
        cmd_delete(args, db, f);
    else if (!strcmp(kw, "update"))
        cmd_update(args, db, f);
    else if (!strcmp(kw, "uniq"))
        cmd_uniq(args, db, f);
    else
        out(f, "incorrect %.20s\n", line);
}

void _free(list *db)
{
    node *curr = db->head;
    while (curr)
    {
        node *next = curr->next;
        FREE(curr);
        curr = next;
    }
    db->head = NULL;
    db->size = 0;
}