#ifndef CPL_INTERPRETER_H_
#define CPL_INTERPRETER_H_

#include "cpl_parser.h"

typedef struct cplI_State
{
    cplP_State* parser;
} cplI_State;

double cplI_visit_bin(cplI_State* state, cplP_Node* node);
double cplI_visit_literal(cplI_State* state, cplP_Node* node);
double cplI_visit(cplI_State* state, cplP_Node* node);

double cplI_interpret(cplI_State* state);

#endif
