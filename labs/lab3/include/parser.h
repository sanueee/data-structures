#ifndef PARSER_H
#define PARSER_H

#include "../include/ast.h"

ASTNode *parse_expr(const char *expr);
ASTNode *parse_prefix(const char *expr);
ASTNode *parse_postfix(const char *expr);

#endif