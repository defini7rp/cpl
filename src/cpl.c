#include "cpl_utils.h"
#include "cpl_lexer.h"
#include "cpl_parser.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void postorder_traversal(cplP_Unit* tree)
{
    if (!tree)
        return;

    if (tree->oper_or_literal.type == CPL_TT_NUMERIC_10)
    {
        printf("%s ", cplL_token_to_cstr(&tree->oper_or_literal));
        return;
    }

    postorder_traversal(tree->left);
    postorder_traversal(tree->right);

    printf("%s ", cplL_token_to_cstr(&tree->oper_or_literal));
}

int main(int argc, char* argv[])
{
    cplL_State lexer = {0};
    cplL_Token token;

    #ifdef _MSC_VER
    lexer.input = cplU_read_file("../test.cpl", &lexer.input_length);
    #else
    lexer.input = cplU_read_file(argv[1], &lexer.input_length);
    #endif

    if (!lexer.input)
        return 1;

    cplP_State parser = {0};

    parser.lexer = &lexer;
    cplP_Unit* ast = cplP_parse(&parser);

    postorder_traversal(ast);

    free(ast);
    free(lexer.input);

    return 0;
}