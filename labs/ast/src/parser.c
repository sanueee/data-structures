#include "../include/parser.h"
#include <string.h>
#include <ctype.h>

#define STACK_MAX 512

static void lex_init(Lexer *lex, const char *src);
static Token lex_next(Lexer *lex);
static Token lex_peek(Lexer *lex);

static ASTNode *parse_add(Lexer *lex);
static ASTNode *parse_mul(Lexer *lex);
static ASTNode *parse_lcm(Lexer *lex);
static ASTNode *parse_gcd(Lexer *lex);
static ASTNode *parse_primary(Lexer *lex);

static ASTNode *load_prf_node(Lexer *lex);

// функции парсинга выражений

ASTNode *parse_expr(const char *expr)
{
    if (expr == NULL || *expr == '\0')
    {
        return NULL;
    }

    Lexer lex;
    lex_init(&lex, expr);

    ASTNode *node = parse_add(&lex); // начинаем со сложения
    if (node == NULL)
    {
        return NULL;
    }

    Token tail = lex_next(&lex);
    if (tail.type != TOK_EOF || lex.error)
    {
        ast_free(node);
        return NULL;
    }
    return node;
}

ASTNode *parse_prefix(const char *expr)
{
    if (!expr || *expr == '\0')
        return NULL;

    Lexer lex;
    lex_init(&lex, expr);

    ASTNode *node = load_prf_node(&lex);
    if (!node)
        return NULL;

    Token tail = lex_next(&lex);
    if (tail.type != TOK_EOF || lex.error)
    {
        ast_free(node);
        return NULL;
    }
    return node;
}

ASTNode *parse_postfix(const char *expr)
{
    if (!expr || *expr == '\0')
        return NULL;

    Lexer lex;
    ASTNode *stack[STACK_MAX];
    int top = 0;

    lex_init(&lex, expr);

    for (;;)
    {
        Token t = lex_next(&lex);
        if (t.type == TOK_EOF)
            break;
        if (t.type == TOK_ERROR)
            goto fail;

        if (t.type == TOK_NUM)
        {
            if (top >= STACK_MAX)
                goto fail;
            ASTNode *nd = ast_make_num(t.num);
            if (!nd)
                goto fail;
            stack[top++] = nd;
        }
        else if (t.type == TOK_VAR)
        {
            if (top >= STACK_MAX)
                goto fail;
            ASTNode *nd = ast_make_var(t.var);
            if (!nd)
                goto fail;
            stack[top++] = nd;
        }
        else if (t.type == TOK_OP)
        {
            if (top < 2)
                goto fail;
            ASTNode *right = stack[--top];
            ASTNode *left = stack[--top];
            ASTNode *nd = ast_make_op(t.op, left, right);
            if (!nd)
            {
                ast_free(left);
                ast_free(right);
                goto fail;
            }
            stack[top++] = nd;
        }
        else
            goto fail;
    }

    if (top != 1)
        goto fail;
    return stack[0];

fail:
    for (int i = 0; i < top; i++)
        ast_free(stack[i]);
    return NULL;
}

// лексер для разбора выражения

static void lex_init(Lexer *lex, const char *src)
{ // инициализация
    lex->src = src;
    lex->pos = 0;
    lex->error = 0;
}

static Token lex_next(Lexer *lex)
{ // смотрим следующий токен с сдвигом позиции
    Token t;
    memset(&t, 0, sizeof(t));

    const char *s = lex->src;
    int i = lex->pos;

    while (s[i] == ' ' || s[i] == '\t')
        i++;

    // тип токена +его позиция i+1 (след символ)
    if (s[i] == '\0')
    {
        t.type = TOK_EOF;
        lex->pos = i;
        return t;
    }
    if (s[i] == '(')
    {
        t.type = TOK_LPAREN;
        lex->pos = i + 1;
        return t;
    }
    if (s[i] == ')')
    {
        t.type = TOK_RPAREN;
        lex->pos = i + 1;
        return t;
    }
    if (s[i] == '+' || s[i] == '-' || s[i] == '*' || s[i] == '/' ||
        s[i] == '%' || s[i] == '#' || s[i] == '@')
    {
        t.type = TOK_OP;
        t.op = s[i];
        lex->pos = i + 1;
        return t;
    }
    if (isdigit((unsigned char)s[i]))
    {
        int num = 0;
        while (isdigit((unsigned char)s[i]))
        {
            num = num * 10 + (s[i++] - '0');
        }
        t.type = TOK_NUM;
        t.num = num;
        lex->pos = i;
        return t;
    }
    if (isalpha((unsigned char)s[i]))
    {
        int j = 0;
        while (isalnum((unsigned char)s[i]) && j < 63)
        {
            t.var[j++] = s[i++];
        }
        t.var[j] = '\0';
        t.type = TOK_VAR;
        lex->pos = i;
        return t;
    }

    t.type = TOK_ERROR;
    lex->pos = i + 1;
    lex->error = 1;
    return t;
}

static Token lex_peek(Lexer *lex)
{ // смотрим следующий токен без сдвига позиции
    int saved_pos = lex->pos;
    int saved_err = lex->error;
    Token t = lex_next(lex);
    lex->pos = saved_pos;
    lex->error = saved_err;
    return t;
}

// иерархичный парсинг операторов

static ASTNode *parse_add(Lexer *lex)
{
    ASTNode *left = parse_mul(lex);
    if (!left)
        return NULL;

    for (;;)
    {
        Token t = lex_peek(lex);
        if (t.type != TOK_OP)
            break;
        if (t.op != '+' && t.op != '-')
            break;
        lex_next(lex);

        ASTNode *right = parse_mul(lex);
        if (!right)
        {
            ast_free(left);
            return NULL;
        }

        ASTNode *node = ast_make_op(t.op, left, right);
        if (!node)
        {
            ast_free(left);
            ast_free(right);
            return NULL;
        }
        left = node;
    }
    return left;
}

static ASTNode *parse_mul(Lexer *lex)
{
    ASTNode *left = parse_lcm(lex);
    if (!left)
        return NULL;

    for (;;)
    {
        Token t = lex_peek(lex);
        if (t.type != TOK_OP)
            break;
        if (t.op != '*' && t.op != '/' && t.op != '%')
            break;
        lex_next(lex);

        ASTNode *right = parse_lcm(lex);
        if (!right)
        {
            ast_free(left);
            return NULL;
        }

        ASTNode *node = ast_make_op(t.op, left, right);
        if (!node)
        {
            ast_free(left);
            ast_free(right);
            return NULL;
        }
        left = node;
    }
    return left;
}

static ASTNode *parse_lcm(Lexer *lex)
{
    ASTNode *left = parse_gcd(lex);
    if (!left)
        return NULL;

    for (;;)
    {
        Token t = lex_peek(lex);
        if (t.type != TOK_OP || t.op != '@')
            break;
        lex_next(lex);

        ASTNode *right = parse_gcd(lex);
        if (!right)
        {
            ast_free(left);
            return NULL;
        }

        ASTNode *node = ast_make_op('@', left, right);
        if (!node)
        {
            ast_free(left);
            ast_free(right);
            return NULL;
        }
        left = node;
    }
    return left;
}

static ASTNode *parse_gcd(Lexer *lex)
{
    ASTNode *left = parse_primary(lex);
    if (!left)
        return NULL;

    for (;;)
    {
        Token t = lex_peek(lex);
        if (t.type != TOK_OP || t.op != '#')
            break;
        lex_next(lex);

        ASTNode *right = parse_primary(lex);
        if (!right)
        {
            ast_free(left);
            return NULL;
        }

        ASTNode *node = ast_make_op('#', left, right);
        if (!node)
        {
            ast_free(left);
            ast_free(right);
            return NULL;
        }
        left = node;
    }
    return left;
}

static ASTNode *parse_primary(Lexer *lex)
{
    Token t = lex_peek(lex);

    if (t.type == TOK_LPAREN)
    {
        lex_next(lex);
        ASTNode *inner = parse_add(lex); // заново
        if (!inner)
            return NULL;
        Token cl = lex_next(lex);
        if (cl.type != TOK_RPAREN)
        {
            ast_free(inner);
            return NULL;
        }
        return inner;
    }

    if (t.type == TOK_NUM)
    {
        lex_next(lex);
        return ast_make_num(t.num);
    }
    if (t.type == TOK_VAR)
    {
        lex_next(lex);
        return ast_make_var(t.var);
    }

    return NULL;
}

// для префиксного обхода

static ASTNode *load_prf_node(Lexer *lex)
{
    Token t = lex_next(lex);

    if (t.type == TOK_EOF || t.type == TOK_ERROR)
        return NULL;
    if (t.type == TOK_NUM)
        return ast_make_num(t.num);
    if (t.type == TOK_VAR)
        return ast_make_var(t.var);

    if (t.type == TOK_OP)
    {
        ASTNode *left = load_prf_node(lex);
        if (!left)
            return NULL;
        ASTNode *right = load_prf_node(lex);
        if (!right)
        {
            ast_free(left);
            return NULL;
        }
        ASTNode *node = ast_make_op(t.op, left, right);
        if (!node)
        {
            ast_free(left);
            ast_free(right);
            return NULL;
        }
        return node;
    }
    return NULL;
}