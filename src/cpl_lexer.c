#include "cpl_lexer.h"
#include "cpl_utils.h"

#include <stdio.h>
#include <stdlib.h>

const char* cplL_token_type_to_cstr(cplL_TokenType type)
{
    const char* prefix;

    switch (type)
    {
    case CPL_TT_UNDEFINED:       prefix = "UNDEFINED";       break;
    case CPL_TT_STRING:          prefix = "STRING";          break;
    case CPL_TT_NUMERIC_UNKNOWN: prefix = "NUMERIC_UNKNOWN"; break;
    case CPL_TT_NUMERIC_2:       prefix = "NUMERIC_2";       break;
    case CPL_TT_NUMERIC_10:      prefix = "NUMERIC_10";      break;
    case CPL_TT_NUMERIC_16:      prefix = "NUMERIC_16";      break;
    case CPL_TT_BOOLEAN:         prefix = "BOOLEAN";         break;
    case CPL_TT_SYMBOL:          prefix = "SYMBOL";          break;
    case CPL_TT_OP_LPAREN:       prefix = "OP_LPAREN";       break;
    case CPL_TT_OP_RPAREN:       prefix = "OP_RPAREN";       break;
    case CPL_TT_OP_LBRACE:       prefix = "OP_LBRACE";       break;
    case CPL_TT_OP_RBRACE:       prefix = "OP_RBRACE";       break;
    case CPL_TT_OP_PLUS:         prefix = "OP_PLUS";         break;
    case CPL_TT_OP_MINUS:        prefix = "OP_MINUS";        break;
    case CPL_TT_OP_ASTERISK:     prefix = "OP_ASTERISK";     break;
    case CPL_TT_OP_SLASH:        prefix = "OP_SLASH";        break;
    case CPL_TT_OP_MOD:          prefix = "OP_MOD";          break;
    case CPL_TT_OP_BOR:          prefix = "OP_BOR";          break;
    case CPL_TT_OP_BAND:         prefix = "OP_BAND";         break;
    case CPL_TT_OP_XOR:          prefix = "OP_XOR";          break;
    case CPL_TT_OP_BNOT:         prefix = "OP_BNOT";         break;
    case CPL_TT_OP_LESS:         prefix = "OP_LESS";         break;
    case CPL_TT_OP_GREATER:      prefix = "OP_GREATER";      break;
    case CPL_TT_OP_NOT:          prefix = "OP_NOT";          break;
    case CPL_TT_OP_ASSIGN:       prefix = "OP_ASSIGN";       break;
    case CPL_TT_OP_COMMA:        prefix = "OP_COMMA";        break;
    case CPL_TT_OP_POWER:        prefix = "OP_POWER";        break;
    case CPL_TT_OP_AND:          prefix = "OP_AND";          break;
    case CPL_TT_OP_OR:           prefix = "OP_OR";           break;
    case CPL_TT_OP_SHIFTLEFT:    prefix = "OP_SHIFTLEFT";    break;
    case CPL_TT_OP_SHIFTRIGHT:   prefix = "OP_SHIFTRIGHT";   break;
    case CPL_TT_OP_LESSEQUAL:    prefix = "OP_LESSEQUAL";    break;
    case CPL_TT_OP_GREATEREQUAL: prefix = "OP_GREATEREQUAL"; break;
    case CPL_TT_OP_ISEQUAL:      prefix = "OP_ISEQUAL";      break;
    case CPL_TT_OP_NOTEQUAL:     prefix = "OP_NOTEQUAL";     break;
    case CPL_TT_KW_FUNC:         prefix = "KW_FUNC";         break;
    }

    return prefix;
}

char* cplL_token_to_cstr(cplL_Token* token)
{
    char* buffer = (char*)malloc(token->length + 1);

    for (size_t i = 0; i < token->length; i++)
        buffer[i] = *(token->start + i);
        
    buffer[token->length] = '\0';
    return buffer;
}

void cplL_print_token(cplL_Token* token)
{
    printf("[%s] ", cplL_token_type_to_cstr(token->type));
    
    for (size_t i = 0; i < token->length; i++)
        putchar(*(token->start + i));

    printf("\n");
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
    return '0' <= c && c <= '9' || c == '.';
}

bool cplL_is_alpha(char c)
{
    return 'a' <= c && c <= 'z' || 'A' <= c && c <= 'Z';
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
    return cplL_is_alpha(c) ||
           cplL_is_digit(c) ||
           c == '_';
}

bool cplL_is_quote(char c)
{
    return c == '\'' || c == '"';
}

bool cplL_is_left_paren(char c)
{
    return c == '(' || c == '{';
}

bool cplL_is_right_paren(char c)
{
    return c == ')' || c == '}';
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

void cplL_start_token(cplL_State* state, cplL_Token* token, cplL_TokenType type, cplL_StateType next_state, bool push)
{
    token->type = type;

    if (push)
        token->length++;

    state->next_state = next_state;
}

void cplL_append_char(cplL_State* state, cplL_Token* token, cplL_StateType next_state)
{
    token->length++;
    state->next_state = next_state;
    state->cursor++;
}

bool cplL_check_eof(cplL_State* state)
{
    if (state->cursor >= state->input_length)
    {
        if (state->parentheses_balancer != 0)
            cplU_log_err("parentheses were not balanced\n");

        if (state->quotes_balancer != 0)
            cplU_log_err("quotes were not balanced\n");

        return true;
    }

    return false;
}

bool cplL_skip_whitespaces(cplL_State* state)
{
    bool eof;

    while (!(eof = cplL_check_eof(state)) && (
        cplL_is_whitespace(state->input[state->cursor]) ||
        cplL_is_newline(state->input[state->cursor])))
        state->cursor++;

    return !eof;
}

bool cplL_skip_comments(cplL_State* state)
{
    if (cplL_check_eof(state))
        return false;

    if (state->input[state->cursor] == '#')
    {
        state->cursor++;

        bool eof;

        while (!(eof = cplL_check_eof(state)) && !cplL_is_newline(state->input[state->cursor]))
            state->cursor++;

        if (!cplL_skip_whitespaces(state))
            return false;

        return !eof;
    }

    return true;
}

bool cplL_next_token(cplL_State* state, cplL_Token* token)
{
#define CURRENT_CHAR state->input[state->cursor]

    if (!state->input)
        return false;

    token->col = 0;
    token->row = 0;
    token->length = 0;
    token->type = CPL_TT_UNDEFINED;
    token->start = 0;

    if (cplL_check_eof(state))
        return false;

    do
    {
        if (state->current_state == CPL_ST_NEW)
        {
            if (!cplL_skip_whitespaces(state))
                return false;

            if (!cplL_skip_comments(state))
                return false;

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
                case '+': token->type = CPL_TT_OP_PLUS; break;
                case '-': token->type = CPL_TT_OP_MINUS; break;
                case '*': token->type = CPL_TT_OP_ASTERISK; next = true; break;
                case '/': token->type = CPL_TT_OP_SLASH; next = true; break;
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
                    cplU_log_err("invalid operator: %c..\n", CURRENT_CHAR);
                    return false;
                }

                token->length++;
                state->next_state = next ? CPL_ST_OPERATOR : CPL_ST_COMPLETE;
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
                state->quotes_balancer++;
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

#define PEEK_COMPLETE(token_type, push) \
            cplL_start_token(state, token, token_type, CPL_ST_COMPLETE, push)

            else if (cplL_is_left_paren(CURRENT_CHAR))
            {
                PEEK_COMPLETE(CURRENT_CHAR == '(' ? CPL_TT_OP_LPAREN : CPL_TT_OP_LBRACE, true);
                state->parentheses_balancer++;
            }

            else if (cplL_is_right_paren(CURRENT_CHAR))
            {
                PEEK_COMPLETE(CURRENT_CHAR == ')' ? CPL_TT_OP_RPAREN : CPL_TT_OP_RBRACE, true);
                state->parentheses_balancer--;
            }

            else if (CURRENT_CHAR == ',')
                PEEK_COMPLETE(CPL_TT_OP_COMMA, false);

            else if (CURRENT_CHAR == ';')
                PEEK_COMPLETE(CPL_TT_OP_SEMICOLON, false);

            else
            {
                cplU_log_err("unexpected character %c\n", CURRENT_CHAR);
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
                        char* symbol = cplL_token_to_cstr(token); \
                        cplU_log_err("invalid numeric literal or symbol: %s\n", symbol); \
                        free(symbol); \
                        return false; \
                    } \
                    state->next_state = CPL_ST_COMPLETE; \
                } \
            } \
            break;

            switch (state->current_state)
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
                        state->next_state = CPL_ST_NUMERIC_16;
                    }
                    
                    else if (CURRENT_CHAR == 'b' || CURRENT_CHAR == 'B')
                    {
                        token->type = CPL_TT_NUMERIC_2;
                        state->next_state = CPL_ST_NUMERIC_2;
                    }

                    state->cursor++;
                }
                else
                {
                    cplU_log_err("unknown prefix for a numeric literal: 0%c\n", CURRENT_CHAR);
                    return false;
                }
            }
            break;

            case CPL_ST_STRING:
            {
                if (cplL_is_quote(CURRENT_CHAR))
                {
                    state->quotes_balancer--;
                    state->next_state = CPL_ST_COMPLETE;
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
                    case CPL_TT_OP_SLASH:
                        EXPECT('/', CPL_TT_OP_INT_DIV) break;
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
                        char* op = cplL_token_to_cstr(token);
                        cplU_log_err("invalid operator: %s\n", op);
                        free(op);
                        return false;
                    }
                }
                else
                    state->next_state = CPL_ST_COMPLETE;
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

                    if (cplL_is_token_equal(token, "func"))
                        token->type = CPL_TT_KW_FUNC;

                    state->next_state = CPL_ST_COMPLETE;
                }
            }
            break;

            case CPL_ST_COMPLETE:
                state->next_state = CPL_ST_NEW;
            break;

            }
        }

        state->current_state = state->next_state;
    }
    while (state->next_state != CPL_ST_NEW);

    return true;

#undef EXPECT
#undef NUMERIC_CASE
#undef CURRENT_CHAR
}
