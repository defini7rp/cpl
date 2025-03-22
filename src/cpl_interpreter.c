#include "cpl_interpreter.h"
#include "cpl_utils.h"

#include <stdlib.h>

void cplI_visit_bin(cplI_State* state, cplP_Node* node, double* result)
{
    double left, right;

    cplI_visit(state, node->children->value, &left);
    cplI_visit(state, node->children->next->value, &right);

    switch (node->value.type)
    {
    case CPL_TT_OP_PLUS: *result = left + right; break;
    case CPL_TT_OP_MINUS: *result = left - right; break;
    case CPL_TT_OP_ASTERISK: *result = left * right; break;

    // TODO: handle right == 0
    case CPL_TT_OP_SLASH: *result = left / right; break;
    case CPL_TT_OP_INT_DIV: *result = (double)((int)left / (int)right); break;
    
    default:
        cplU_log_err("TODO: implement interpreting other binary operators\n");
    }
}

void cplI_visit_unary(cplI_State* state, cplP_Node* node, double* result)
{
    double right;
    cplI_visit(state, node->children->value, &right);

    switch (node->value.type)
    {
    case CPL_TT_OP_MINUS: *result = -right; break;
    case CPL_TT_OP_PLUS: *result = right; break;

    default:
        cplU_log_err("TODO: implement interpreting other unary operators\n");
    }
}

void cplI_visit_literal(cplI_State* state, cplP_Node* node, double* result)
{
    if (node->value.type == CPL_TT_NUMERIC_10)
    {
        char* num_as_cstr = cplL_token_to_cstr(&node->value);
        *result = strtod(num_as_cstr, NULL);
        free(num_as_cstr);
        return;
    }

    cplU_log_err("TODO: implement interpreting other literals\n");
}

void cplI_visit_scope(cplI_State* state, cplP_Node* node, double* result)
{
    cplP_NodeChild* child = node->children;

    while (child)
    {
        cplI_visit(state, child->value, result);
        child = child->next;
    }
}

void cplI_visit_assign(cplI_State* state, cplP_Node* node, double* result)
{
    cplI_visit(state, node->children->next->value, result);

    cplHT_insert(
        &state->variables,
        cplL_token_to_cstr(&node->children->value->value),
        *result
    );
}

void cplI_visit_symbol(cplI_State* state, cplP_Node* node, double* result)
{
    char* name = cplL_token_to_cstr(&node->value);

    if (!cplHT_get(&state->variables, name, result))
        cplU_log_err("no such variable: %s", name);

    free(name);
}

void cplI_visit(cplI_State* state, cplP_Node* node, double* result)
{
    if (!node)
    {
        cplU_log_err("visited node == NULL\n");
        return;
    }

    switch (node->type)
    {
    case CPL_NT_LITERAL: cplI_visit_literal(state, node, result); break;
    case CPL_NT_OP_BIN: cplI_visit_bin(state, node, result); break;
    case CPL_NT_OP_UNARY: cplI_visit_unary(state, node, result); break;
    case CPL_NT_SCOPE: cplI_visit_scope(state, node, result); break;
    case CPL_NT_OP_ASSIGN: cplI_visit_assign(state, node, result); break;
    case CPL_NT_SYMBOL: cplI_visit_symbol(state, node, result); break;
    case CPL_NT_BLANK: break;
    
    default:
        cplU_log_err("TODO: implement visiting other nodes\n");

    }
}

void cplI_interpret(cplI_State* state, double* result)
{
    cplP_Node* ast = cplP_parse(state->parser);

    cplHT_init(&state->variables);
    cplI_visit(state, ast, result);

    double dump;
    cplHT_get(&state->variables, "dump", &dump);

    cplU_log_err("%lf\n", dump);

    cplHT_free(&state->variables);
    cplP_free_node(ast);
}
