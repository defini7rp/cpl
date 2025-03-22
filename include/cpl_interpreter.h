#ifndef CPL_INTERPRETER_H_
#define CPL_INTERPRETER_H_

#include "cpl_parser.h"
#include "cpl_hashtable.h"

typedef struct cplI_State
{
    cplP_State* parser;
    cplHT_Table variables;
} cplI_State;

void cplI_visit_bin(cplI_State* state, cplP_Node* node, double* result);
void cplI_visit_unary(cplI_State* state, cplP_Node* node, double* result);
void cplI_visit_literal(cplI_State* state, cplP_Node* node, double* result);
void cplI_visit_scope(cplI_State* state, cplP_Node* node, double* result);
void cplI_visit_assign(cplI_State* state, cplP_Node* node, double* result);
void cplI_visit_symbol(cplI_State* state, cplP_Node* node, double* result);
void cplI_visit(cplI_State* state, cplP_Node* node, double* result);

void cplI_interpret(cplI_State* state, double* result);

#endif
