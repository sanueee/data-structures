#ifndef _PARSER_H_
#define _PARSER_H_

#include "ast.h"

ASTNode *parse_natural(const char *expr);
ASTNode *parse_prefix(const char *expr);
ASTNode *parse_postfix(const char *expr);

#endif
