#ifndef CPL_PARSER_H_
#define CPL_PARSER_H_

#include "cpl_lexer.h"

typedef struct cplP_Unit
{
   struct cplP_Unit* left;
   struct cplP_Unit* right;
   cplL_Token oper_or_literal;
} cplP_Unit;

typedef struct cplP_State
{
    cplL_Token current_token;
    cplL_State* lexer;
} cplP_State;

bool cplP_has_type_impl(cplP_State* state, size_t count, ...);
#define cplP_has_type(state, ...) cplP_has_type_impl(state, sizeof((cplL_TokenType[]){__VA_ARGS__}) / sizeof(cplL_TokenType), __VA_ARGS__)

bool cplP_next_token(cplP_State* state);

// If the current token is of type "type" then grabs the next token
// otherwise prints an error
bool cplP_expect(cplP_State* state, cplL_TokenType type);

cplP_Unit* cplP_parse_factor(cplP_State* state);
cplP_Unit* cplP_parse_term(cplP_State* state);
cplP_Unit* cplP_parse_expr(cplP_State* state);
cplP_Unit* cplP_parse(cplP_State* state);

#endif
