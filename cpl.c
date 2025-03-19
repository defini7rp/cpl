#include "cpl_lexer.h"

#include <stdio.h>
#include <string.h>

const char* const INPUT =
    "func say_hi(name)\n"
    "\tprint('Hello ' + name)\n"
    "end\n";

int main()
{
    cplL_State state = {0};
    cplL_Token token;

    state.input = INPUT;
    state.input_length = strlen(INPUT);

    while (cplL_next_token(&state, &token))
    {
        if (token.type != CPL_TT_UNDEFINED)
        {
            for (size_t i = 0; i < token.length; i++)
                putchar(*(token.start + i));
            putchar('\n');
        }
    }

    return 0;
}