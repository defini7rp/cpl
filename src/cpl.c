#include "cpl_lexer.h"
#include "cpl_utils.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char* argv[])
{
    cplL_State state = {0};
    cplL_Token token;

    state.input = cplU_read_file(argv[1], &state.input_length);
    printf("%d\n", state.input_length);

    if (!state.input)
        return 1;

    while (cplL_next_token(&state, &token))
        cplL_print_token(&token);

    free(state.input);

    return 0;
}