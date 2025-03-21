#include "cpl_interpreter.h"
#include "cpl_utils.h"

double cplI_visit_bin(cplI_State* state, cplP_Node* node)
{
    if (node->value.type == CPL_TT_OP_ASSIGN)
    {
        double value = cplI_visit(state, node->right);

        cplHT_insert(
            &state->variables,
            cplL_token_to_cstr(&node->left->value),
            value
        );

        return value;
    }
    else
    {
        double left = cplI_visit(state, node->left);
        double right = cplI_visit(state, node->right);

        switch (node->value.type)
        {
        case CPL_TT_OP_PLUS: return left + right; break;
        case CPL_TT_OP_MINUS: return left - right; break;
        case CPL_TT_OP_ASTERISK: return left * right; break;

        // TODO: handle right == 0
        case CPL_TT_OP_SLASH: return left / right; break;
        }
    }

    cplU_log_err("TODO: implement interpreting other binary operators\n");
    return 0.0;
}

double cplI_visit_unary(cplI_State* state, cplP_Node* node)
{
    double right = cplI_visit(state, node->right);

    switch (node->value.type)
    {
    case CPL_TT_OP_MINUS: return -right;
    case CPL_TT_OP_PLUS: return right;
    }

    cplU_log_err("TODO: implement interpreting other unary operators\n");
    return 0.0;
}

double cplI_visit_literal(cplI_State* state, cplP_Node* node)
{
    if (node->value.type == CPL_TT_NUMERIC_10)
    {
        char* num_as_cstr = cplL_token_to_cstr(&node->value);
        double result = strtod(num_as_cstr, NULL);
        free(num_as_cstr);
        return result;
    }

    cplU_log_err("TODO: implement interpreting other literals\n");
    return 0.0;
}

double cplI_visit(cplI_State* state, cplP_Node* node)
{
    if (!node)
    {
        cplU_log_err("visited node == NULL\n");
        return 0.0;
    }

    switch (node->type)
    {
    case CPL_NT_LITERAL: return cplI_visit_literal(state, node);
    case CPL_NT_OP_BIN: return cplI_visit_bin(state, node);
    case CPL_NT_OP_UNARY: return cplI_visit_unary(state, node);
    
    default:
        cplU_log_err("TODO: implement visiting other nodes\n");

    }

    return 0.0;
}

double cplI_interpret(cplI_State* state)
{
    cplP_Node* ast = cplP_parse(state->parser);

    cplHT_init(&state->variables);

    double result = cplI_visit(state, ast);

    cplHT_free(&state->variables);
    cplP_free_node(ast);

    return result;
}
