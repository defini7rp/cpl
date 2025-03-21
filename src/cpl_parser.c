#include "cpl_parser.h"
#include "cpl_utils.h"

#include <stdarg.h>

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
#define PEEK(node_type) \
    { node = cplU_alloc(cplP_Node); \
    node->type = node_type; \
    node->value = state->current_token; \
    cplP_next_token(state); }

    cplP_Node* node = NULL;

    if (cplP_has_type(state, CPL_TT_OP_MINUS, CPL_TT_OP_PLUS))
    {
        node = cplU_alloc(cplP_Node);

        node->type = CPL_NT_OP_UNARY;
        node->value = state->current_token;

        cplP_next_token(state);
        node->right = cplP_parse_factor(state);
    }

    else if (cplP_has_type(state, CPL_TT_BOOLEAN, CPL_TT_STRING, CPL_TT_NUMERIC_10))
        PEEK(CPL_NT_LITERAL)

    else if (state->current_token.type == CPL_TT_SYMBOL)
        PEEK(CPL_NT_SYMBOL)

    else if (cplP_has_type(state, CPL_TT_OP_LPAREN))
    {
        cplP_next_token(state);
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

    while (cplP_has_type(state, CPL_TT_OP_ASTERISK, CPL_TT_OP_SLASH))
    {
        cplP_Node* node = cplU_alloc(cplP_Node);
        
        node->type = CPL_NT_OP_BIN;
        node->value = state->current_token;
        cplP_next_token(state);

        node->left = left;
        node->right = cplP_parse_factor(state);

        left = node;
    }

    return left;
}

cplP_Node* cplP_parse_expr(cplP_State* state)
{
    cplP_Node* left = cplP_parse_term(state);

    while (cplP_has_type(state, CPL_TT_OP_PLUS, CPL_TT_OP_MINUS, CPL_TT_OP_ASSIGN))
    {
        cplP_Node* node = cplU_alloc(cplP_Node);
        
        node->type = CPL_NT_OP_BIN;
        node->value = state->current_token;
        cplP_next_token(state);

        node->left = left;

        node->right =
            node->value.type == CPL_TT_OP_ASSIGN ?
            cplP_parse_expr(state) :
            cplP_parse_term(state);

        left = node;
    }

    return left;
}

cplP_Node* cplP_parse(cplP_State* state)
{
    cplP_next_token(state);
    return cplP_parse_expr(state);
}

void cplP_free_node(cplP_Node* node)
{
    if (node == NULL)
        return;
        
    cplP_free_node(node->left);
    cplP_free_node(node->right);

    free(node);
}
