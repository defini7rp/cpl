#include "cpl_utils.h"
#include "cpl_lexer.h"
#include "cpl_parser.h"
#include "cpl_interpreter.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char* argv[])
{
    cplL_State lexer = {0};

    #ifdef _MSC_VER
    lexer.input = cplU_read_file("../test.cpl", &lexer.input_length);
    #else
    lexer.input = cplU_read_file(argv[1], &lexer.input_length);
    #endif

    if (!lexer.input)
        return 1;

    cplP_State parser = {0};
    parser.lexer = &lexer;

    cplI_State interpreter = {0};
    interpreter.parser = &parser;

    double result = cplI_interpret(&interpreter);    
    printf("%lf\n", result);

    free(lexer.input);

    return 0;
}