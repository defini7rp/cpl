#include "cpl_lexer.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

// TODO: Printing the location of where an error has occured
void cplL_log_err(const char* format, ...)
{
    va_list args;

    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
}

char* cplL_token_as_cstr(cplL_Token* token)
{
    char* buffer = (char*)malloc(token->length + 1);

    for (size_t i = 0; i < token->length; i++)
        buffer[i] = *(token->start + i);
        
    buffer[token->length] = '\0';
    return buffer;
}

bool cplL_is_token_equal(cplL_Token* token, const char* data)
{
    for (size_t i = 0; i < token->length; i++)
    {
        if (data[i] != *(token->start + i))
            return false;
    }

    if (data[token->length] != '\0')
        return false;

    return true;
}

bool cplL_is_digit(char c)
{
    return '0' <= c && c <= '9';
}

bool cplL_is_lower(char c)
{
    return 'a' <= c && c <= 'z';
}

bool cplL_is_upper(char c)
{
    return 'A' <= c && c <= 'Z';
}

bool cplL_is_newline(char c)
{
    return c == '\r' || c == '\n';
}

bool cplL_is_whitespace(char c)
{
    return c == ' ' || c == '\t';
}

bool cplL_is_operator_char(char c)
{
    return
        c == '+' || c == '-' || c == '*' || c == '/' ||
        c == '%' || c == '|' || c == '&' || c == '^' ||
        c == '~' || c == '<' || c == '>' || c == '!' ||
        c == '=';
}

bool cplL_is_symbol_char(char c)
{
    return cplL_is_lower(c) ||
            cplL_is_upper(c) ||
            cplL_is_digit(c) ||
            c == '_';
}

bool cplL_is_quote(char c)
{
    return c == '\'' || c == '"';
}

bool cplL_is_left_paren(char c)
{
    return c == '(' || c == '[' || c == '{';
}

bool cplL_is_right_paren(char c)
{
    return c == ')' || c == ']' || c == '}';
}

bool cplL_is_numeric_prefix(char c)
{
    return c == 'x' || c == 'b' ||
            c == 'X' || c == 'B';
}

bool cplL_is_bin(char c)
{
    return c == '0' || c == '1';
}

bool cplL_is_hex(char c)
{
    return cplL_is_digit(c) ||
            'a' <= c && c <= 'f' ||
            'A' <= c && c <= 'F';
}

void cplL_start_token(cplL_State* state, cplL_Token* token, cplL_TokenType type, cplL_StateType nextState, bool push)
{
    token->type = type;

    if (push)
        token->length++;

    state->nextState = nextState;
}

void cplL_append_char(cplL_State* state, cplL_Token* token, cplL_StateType nextState)
{
    token->length++;
    state->nextState = nextState;
    state->cursor++;
}

bool cplL_next_token(cplL_State* state, cplL_Token* token)
{
#define CURRENT_CHAR state->input[state->cursor]

    token->col = 0;
    token->row = 0;
    token->length = 0;
    token->type = CPL_TT_UNDEFINED;
    token->start = 0;

    if (state->cursor >= state->input_length)
    {
        if (state->parenthesesBalancer != 0)
            cplL_log_err("parentheses were not balanced");

        if (state->quotesBalancer != 0)
            cplL_log_err("quotes were not balanced");

        return false;
    }

    do
    {
        if (state->currentState == CPL_ST_NEW)
        {
            if (cplL_is_whitespace(CURRENT_CHAR) ||
                cplL_is_newline(CURRENT_CHAR))
            {
                state->nextState = CPL_ST_NEW;
                state->cursor++;
                return true;
            }

            token->start = &CURRENT_CHAR;

            if (cplL_is_digit(CURRENT_CHAR))
            {
                if (CURRENT_CHAR == '0')
                {
                    cplL_start_token(
                        state, token,
                        CPL_TT_NUMERIC_UNKNOWN,
                        CPL_ST_NUMERIC_UNKNOWN,
                        false
                    );
                }
                else
                {
                    cplL_start_token(
                        state, token,
                        CPL_TT_NUMERIC_10,
                        CPL_ST_NUMERIC_10,
                        true
                    );
                }
            }

            else if (cplL_is_operator_char(CURRENT_CHAR))
            {
                bool next = false;

                switch (CURRENT_CHAR)
                {
                case '+': token->type = CPL_TT_OP_MINUS; break;
                case '-': token->type = CPL_TT_OP_MINUS; break;
                case '*': token->type = CPL_TT_OP_ASTERISK; next = true; break;
                case '/': token->type = CPL_TT_OP_SLASH; break;
                case '%': token->type = CPL_TT_OP_MOD; break;
                case '|': token->type = CPL_TT_OP_BOR; next = true; break;
                case '&': token->type = CPL_TT_OP_BAND; next = true; break;
                case '^': token->type = CPL_TT_OP_XOR; break;
                case '~': token->type = CPL_TT_OP_BNOT; break;
                case '<': token->type = CPL_TT_OP_LESS; next = true; break;
                case '>': token->type = CPL_TT_OP_GREATER; next = true; break;
                case '!': token->type = CPL_TT_OP_NOT; next = true; break;
                case '=': token->type = CPL_TT_OP_ASSIGN; break;
                default:
                    cplL_log_err("invalid operator: %c..", CURRENT_CHAR);
                    return false;
                }

                token->length++;
                state->nextState = next ? CPL_ST_OPERATOR : CPL_ST_COMPLETE;
            }

            else if (cplL_is_quote(CURRENT_CHAR))
            {
                cplL_start_token(
                    state, token,
                    CPL_TT_STRING,
                    CPL_ST_STRING,
                    false
                );

                token->start++;
                state->quotesBalancer++;
            }

            else if (cplL_is_symbol_char(CURRENT_CHAR))
            {
                cplL_start_token(
                    state, token,
                    CPL_TT_SYMBOL,
                    CPL_ST_SYMBOL,
                    true
                );
            }

            else if (cplL_is_left_paren(CURRENT_CHAR))
            {
                cplL_start_token(
                    state, token,
                    CPL_TT_OP_LPAREN,
                    CPL_ST_COMPLETE,
                    true
                );

                state->parenthesesBalancer++;
            }

            else if (cplL_is_right_paren(CURRENT_CHAR))
            {
                cplL_start_token(
                    state, token,
                    CPL_TT_OP_RPAREN,
                    CPL_ST_COMPLETE,
                    true
                );

                state->parenthesesBalancer--;
            }

            else if (CURRENT_CHAR == ',')
            {
                cplL_start_token(
                    state, token,
                    CPL_TT_OP_COMMA,
                    CPL_ST_COMPLETE,
                    false
                );
            }

            else
            {
                cplL_log_err("unexpected character %c", CURRENT_CHAR);
                return false;
            }

            state->cursor++;
        }
        else
        {
        #define NUMERIC_CASE(postfix, newState) \
            case newState: \
            { \
                if (cplL_is_##postfix(CURRENT_CHAR)) \
                    cplL_append_char(state, token, newState); \
                else \
                { \
                    if (cplL_is_symbol_char(CURRENT_CHAR)) \
                    { \
                        token->length++; \
                        char* symbol = cplL_token_as_cstr(token); \
                        cplL_log_err("invalid numeric literal or symbol: %s", symbol); \
                        free(symbol); \
                        return false; \
                    } \
                    state->nextState = CPL_ST_COMPLETE; \
                } \
            } \
            break;

            switch (state->currentState)
            {
            NUMERIC_CASE(hex, CPL_ST_NUMERIC_16);
            NUMERIC_CASE(digit, CPL_ST_NUMERIC_10);
            NUMERIC_CASE(bin, CPL_ST_NUMERIC_2);

            case CPL_ST_NUMERIC_UNKNOWN:
            {
                if (cplL_is_numeric_prefix(CURRENT_CHAR))
                {
                    if (CURRENT_CHAR == 'x' || CURRENT_CHAR == 'X')
                    {
                        token->type = CPL_TT_NUMERIC_16;
                        state->nextState = CPL_ST_NUMERIC_16;
                    }
                    
                    else if (CURRENT_CHAR == 'b' || CURRENT_CHAR == 'B')
                    {
                        token->type = CPL_TT_NUMERIC_2;
                        state->nextState = CPL_ST_NUMERIC_2;
                    }

                    state->cursor++;
                }
                else
                    cplL_log_err("uknown prefix for a numeric literal: 0%c", CURRENT_CHAR);
                    return false;
            }
            break;

            case CPL_ST_STRING:
            {
                if (cplL_is_quote(CURRENT_CHAR))
                {
                    state->quotesBalancer--;
                    state->nextState = CPL_ST_COMPLETE;
                    state->cursor++;
                }
                else
                    cplL_append_char(state, token, CPL_ST_STRING);
            }
            break;

            case CPL_ST_OPERATOR:
            {
                if (cplL_is_operator_char(CURRENT_CHAR))
                {
                    bool valid = true;

                #define EXPECT(chr, token_type) \
                    if (CURRENT_CHAR == chr) token->type = token_type; \
                    else valid = false;
                    
                    switch (token->type)
                    {
                    case CPL_TT_OP_ASTERISK:
                        EXPECT('*', CPL_TT_OP_POWER) break;
                    case CPL_TT_OP_BOR:
                        EXPECT('|', CPL_TT_OP_OR) break;
                    case CPL_TT_OP_BAND:
                        EXPECT('&', CPL_TT_OP_AND) break;
                    case CPL_TT_OP_LESS:
                        EXPECT('=', CPL_TT_OP_LESSEQUAL) break;
                    case CPL_TT_OP_GREATER:
                        EXPECT('=', CPL_TT_OP_GREATEREQUAL) break;
                    case CPL_TT_OP_NOT:
                        EXPECT('=', CPL_TT_OP_NOTEQUAL) break;
                    }

                    token->length++;

                    if (valid)
                        cplL_append_char(state, token, CPL_ST_COMPLETE);
                    else
                    {
                        char* op = cplL_token_as_cstr(token);
                        cplL_log_err("invalid operator: %s", op);
                        free(op);
                        return false;
                    }
                }
            }
            break;

            case CPL_ST_SYMBOL:
            {
                if (cplL_is_symbol_char(CURRENT_CHAR))
                    cplL_append_char(state, token, CPL_ST_SYMBOL);
                else
                {
                    if (cplL_is_token_equal(token, "true") ||
                        cplL_is_token_equal(token, "false"))
                        token->type = CPL_TT_BOOLEAN;

                    state->nextState = CPL_ST_COMPLETE;
                }
            }
            break;

            case CPL_ST_COMPLETE:
                state->nextState = CPL_ST_NEW;
            break;

            }
        }

        state->currentState = state->nextState;
    }
    while (state->nextState != CPL_ST_NEW);

    return true;

#undef EXPECT
#undef NUMERIC_CASE
#undef CURRENT_CHAR
}
