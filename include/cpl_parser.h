#ifndef CPL_PARSER_H_
#define CPL_PARSER_H_

#include "cpl_lexer.h"

typedef enum cplP_NodeType
{
    CPL_NT_LITERAL,
    CPL_NT_OP_BIN,
    CPL_NT_OP_UNARY
} cplP_NodeType;

typedef struct cplP_Node
{
   struct cplP_Node* left;
   struct cplP_Node* right;
   cplL_Token value;
   cplP_NodeType type;
} cplP_Node;

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

cplP_Node* cplP_parse_factor(cplP_State* state);
cplP_Node* cplP_parse_term(cplP_State* state);
cplP_Node* cplP_parse_expr(cplP_State* state);
cplP_Node* cplP_parse(cplP_State* state);

void cplP_free_node(cplP_Node* node);

#endif
