#include "cpl_parser.h"
#include "cpl_utils.h"

#include <stdarg.h>
#include <stdlib.h>

cplP_NodeChild* cplP_new_node_child(cplP_Node* value)
{
    cplP_NodeChild* child = (cplP_NodeChild*)calloc(1, sizeof(cplP_NodeChild));
    child->value = value;
    return child;
}

cplP_Node* cplP_new_node(size_t children_count, cplP_NodeType node_type, cplL_Token* value)
{
    cplP_Node* node = (cplP_Node*)calloc(1, sizeof(cplP_Node));

    node->type = node_type;

    if (value)
        node->value = *value;

    if (children_count != 0)
    {
        cplP_NodeChild** child = &node->children;

        while (children_count-- != 0)
        {
            *child = cplP_new_node_child(NULL);
            child = &(*child)->next;
        }
    }

    return node;
}

bool cplP_has_type_impl(cplP_State* state, size_t count, ...)
{
    va_list args;
    va_start(args, count);

    for (size_t i = 0; i < count; i++)
    {
        if (state->current_token.type == va_arg(args, cplL_TokenType))
        {
            va_end(args);
            return true;
        }
    }

    va_end(args);
    return false;
}

bool cplP_next_token(cplP_State* state)
{
    return cplL_next_token(state->lexer, &state->current_token);
}

bool cplP_expect(cplP_State* state, cplL_TokenType type)
{
    if (state->current_token.type != type)
    {
        cplU_log_err(
            "expected %s but got %s\n",
            cplL_token_type_to_cstr(type),
            cplL_token_type_to_cstr(state->current_token.type)
        );

        return false;
    }

    cplP_next_token(state);
    return true;
}

cplP_Node* cplP_parse_factor(cplP_State* state)
{
#define PEEK(children_count, node_type) \
    { node = cplP_new_node(children_count, node_type, &state->current_token); \
    cplP_next_token(state); }

    cplP_Node* node = NULL;

    if (cplP_has_type(state, CPL_TT_OP_MINUS, CPL_TT_OP_PLUS))
    {
        node = cplP_new_node(1, CPL_NT_OP_UNARY, &state->current_token);
        cplP_next_token(state); // +, -

        node->children->value = cplP_parse_factor(state);
    }

    else if (cplP_has_type(state, CPL_TT_BOOLEAN, CPL_TT_STRING, CPL_TT_NUMERIC_10))
        PEEK(0, CPL_NT_LITERAL)

    else if (state->current_token.type == CPL_TT_SYMBOL)
        PEEK(0, CPL_NT_SYMBOL)

    else if (cplP_has_type(state, CPL_TT_OP_LPAREN))
    {
        cplP_next_token(state); // (
        node = cplP_parse_expr(state);
        cplP_expect(state, CPL_TT_OP_RPAREN);
    }

    else
        cplU_log_err("TODO: parse other factors\n");

    return node;
}

cplP_Node* cplP_parse_term(cplP_State* state)
{
    cplP_Node* left = cplP_parse_factor(state);

    while (cplP_has_type(state, CPL_TT_OP_ASTERISK, CPL_TT_OP_SLASH, CPL_TT_OP_INT_DIV))
    {
        cplP_Node* node = cplP_new_node(2, CPL_NT_OP_BIN, &state->current_token);
        cplP_next_token(state); // operator

        node->children->value = left;
        node->children->next->value = cplP_parse_factor(state);

        left = node;
    }

    return left;
}

cplP_Node* cplP_parse_expr(cplP_State* state)
{
    cplP_Node* left = cplP_parse_term(state);

    while (cplP_has_type(state, CPL_TT_OP_PLUS, CPL_TT_OP_MINUS))
    {
        cplP_Node* node = cplP_new_node(2, CPL_NT_OP_BIN, &state->current_token);
        cplP_next_token(state); // operator

        node->children->value = left;
        node->children->next->value = cplP_parse_term(state);

        left = node;
    }

    return left;
}

cplP_Node* cplP_parse_func_decl(cplP_State* state)
{
    cplP_next_token(state); // func

    cplP_Node* node = cplP_new_node(
        0, CPL_NT_FUNC_DECL,
        &state->current_token);
    
    cplP_next_token(state); // symbol
    cplP_expect(state, CPL_TT_OP_LPAREN);

    cplP_NodeChild** child = &node->children;

    while (true)
    {
        *child = cplP_new_node_child(cplP_new_node(0, CPL_NT_SYMBOL, &state->current_token));
        cplP_expect(state, CPL_TT_SYMBOL);

        child = &(*child)->next;

        if (state->current_token.type == CPL_TT_OP_RPAREN)
        {
            cplP_next_token(state);
            break;
        }

        cplP_expect(state, CPL_TT_OP_COMMA);
    }

    *child = cplP_new_node_child(cplP_parse_scope(state));

    return node;
}

cplP_Node* cplP_parse_assignment(cplP_State* state)
{
    cplP_Node* node = cplP_new_node(2, CPL_NT_OP_ASSIGN, NULL);
    
    node->children->value = cplP_new_node(0, CPL_NT_SYMBOL, &state->current_token);
    
    cplP_next_token(state);
    cplP_expect(state, CPL_TT_OP_ASSIGN);

    node->children->next->value = cplP_parse_expr(state);

    return node;
}

cplP_Node* cplP_parse_statement(cplP_State* state)
{
    switch (state->current_token.type)
    {
    case CPL_TT_OP_LBRACE: return cplP_parse_scope(state);
    case CPL_TT_SYMBOL: return cplP_parse_assignment(state);
    case CPL_TT_KW_FUNC: return cplP_parse_func_decl(state);
    }

    return cplP_new_node(0, CPL_NT_BLANK, NULL);
}

cplP_Node* cplP_parse_statements(cplP_State* state)
{
    cplP_Node* node = cplP_new_node(0, CPL_NT_SCOPE, NULL);
    cplP_NodeChild** child = &node->children;

#define NEXT_STATEMENT cplP_new_node_child(cplP_parse_statement(state))

    *child = NEXT_STATEMENT;
    child = &(*child)->next;

    while (cplP_has_type(state, CPL_TT_OP_SEMICOLON, CPL_TT_OP_RBRACE))
    {
        cplP_next_token(state); // ;
        *child = NEXT_STATEMENT;
        child = &(*child)->next;
    }

    return node;
}

cplP_Node* cplP_parse_scope(cplP_State* state)
{
    cplP_expect(state, CPL_TT_OP_LBRACE);
    cplP_Node* node = cplP_parse_statements(state);
    cplP_expect(state, CPL_TT_OP_RBRACE);

    return node;
}

cplP_Node* cplP_parse(cplP_State* state)
{
    cplP_next_token(state);
    return cplP_parse_statements(state);
}

void cplP_free_node_children(cplP_NodeChild* child)
{
    if (!child)
        return;

    cplP_free_node_children(child->next);
    free(child);
}

void cplP_free_node(cplP_Node* node)
{
    if (node == NULL)
        return;
        
    cplP_free_node_children(node->children);    
    free(node);
}
