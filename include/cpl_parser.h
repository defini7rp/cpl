#ifndef CPL_PARSER_H_
#define CPL_PARSER_H_

#include "cpl_lexer.h"

typedef enum cplP_NodeType
{
    CPL_NT_BLANK,
    CPL_NT_LITERAL,
    CPL_NT_OP_BIN,
    CPL_NT_OP_UNARY,
    CPL_NT_SYMBOL,
    CPL_NT_OP_ASSIGN,
    CPL_NT_STATEMENT,
    CPL_NT_SCOPE,
    CPL_NT_FUNC_DECL
} cplP_NodeType;

struct cplP_Node;

typedef struct cplP_NodeChild
{
    struct cplP_Node* value;
    struct cplP_NodeChild* next;
} cplP_NodeChild;

typedef struct cplP_Node
{
   struct cplP_NodeChild* children;

   cplL_Token value;
   cplP_NodeType type;
} cplP_Node;

typedef struct cplP_State
{
    cplL_Token current_token;
    cplL_State* lexer;
} cplP_State;

cplP_NodeChild* cplP_new_node_child(cplP_Node* value);
cplP_Node* cplP_new_node(size_t children_count, cplP_NodeType node_type, cplL_Token* value);

bool cplP_has_type_impl(cplP_State* state, size_t count, ...);
#define cplP_has_type(state, ...) cplP_has_type_impl(state, sizeof((cplL_TokenType[]){__VA_ARGS__}) / sizeof(cplL_TokenType), __VA_ARGS__)

bool cplP_next_token(cplP_State* state);

// If the current token is of type "type" then grabs the next token
// otherwise prints an error
bool cplP_expect(cplP_State* state, cplL_TokenType type);

cplP_Node* cplP_parse_factor(cplP_State* state);
cplP_Node* cplP_parse_term(cplP_State* state);
cplP_Node* cplP_parse_expr(cplP_State* state);

cplP_Node* cplP_parse_func_decl(cplP_State* state);
cplP_Node* cplP_parse_assignment(cplP_State* state);
cplP_Node* cplP_parse_statement(cplP_State* state);
cplP_Node* cplP_parse_statements(cplP_State* state);
cplP_Node* cplP_parse_scope(cplP_State* state);
cplP_Node* cplP_parse(cplP_State* state);

void cplP_free_node_children(cplP_NodeChild* child);
void cplP_free_node(cplP_Node* node);

#endif
