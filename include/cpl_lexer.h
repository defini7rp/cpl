#ifndef CPL_LEXER_H_
#define CPL_LEXER_H_

#include <stdbool.h>
#include <stddef.h>

#define CPL_ARRAY_SIZE(a) sizeof(a) / sizeof(a[0])

typedef enum cplL_TokenType
{
    CPL_TT_UNDEFINED,
    CPL_TT_STRING,
    CPL_TT_NUMERIC_UNKNOWN,
    CPL_TT_NUMERIC_2,
    CPL_TT_NUMERIC_10,
    CPL_TT_NUMERIC_16,
    CPL_TT_BOOLEAN,
    CPL_TT_SYMBOL,

    // !!! The order is reserved starting from CPL_TT_OPERATOR !!!
    CPL_TT_OPERATOR,
    CPL_TT_OP_LPAREN,
    CPL_TT_OP_RPAREN,
    CPL_TT_OP_LBRACE,
    CPL_TT_OP_RBRACE,
    CPL_TT_OP_PLUS,
    CPL_TT_OP_MINUS,
    CPL_TT_OP_ASTERISK,
    CPL_TT_OP_SLASH,
    CPL_TT_OP_MOD,
    CPL_TT_OP_BOR,
    CPL_TT_OP_BAND,
    CPL_TT_OP_XOR,
    CPL_TT_OP_BNOT,
    CPL_TT_OP_LESS,
    CPL_TT_OP_GREATER,
    CPL_TT_OP_NOT,
    CPL_TT_OP_ASSIGN,
    CPL_TT_OP_COMMA,
    CPL_TT_OP_POWER,
    CPL_TT_OP_AND,
    CPL_TT_OP_OR,
    CPL_TT_OP_SHIFTLEFT,
    CPL_TT_OP_SHIFTRIGHT,
    CPL_TT_OP_LESSEQUAL,
    CPL_TT_OP_GREATEREQUAL,
    CPL_TT_OP_ISEQUAL,
    CPL_TT_OP_NOTEQUAL,
    CPL_TT_OP_SEMICOLON,

    CPL_TT_KEYWORD,
    CPL_TT_KW_IF,
    CPL_TT_KW_VAR,
    CPL_TT_KW_FOR,
    CPL_TT_KW_ELSE,
    CPL_TT_KW_ELIF,
    CPL_TT_KW_FUNC,
    CPL_TT_KW_WHILE
} cplL_TokenType;

typedef enum cplL_StateType
{
    CPL_ST_NEW,
    CPL_ST_COMPLETE,
    CPL_ST_NUMERIC_UNKNOWN,
    CPL_ST_NUMERIC_2,
    CPL_ST_NUMERIC_10,
    CPL_ST_NUMERIC_16,
    CPL_ST_STRING,
    CPL_ST_OPERATOR,
    CPL_ST_SYMBOL,
    CPL_ST_LPAREN,
    CPL_ST_RPAREN,
} cplL_StateType;

// !!! The order is reserved (sorted by length) !!!
static const char* const CPL_KEYWORDS[] =
{
    "if", "do", "var", "for", "end",
    "then", "else", "elif", "func", "while"
};

static const size_t CPL_KEYWORDS_COUNT = CPL_ARRAY_SIZE(CPL_KEYWORDS);

typedef struct cplL_Token
{
    cplL_TokenType type;
    const char* start; // pointer to a character from cplL_State::input
    size_t length;

    int row;
    int col;
} cplL_Token;

typedef struct cplL_State
{
    char* input;
    size_t input_length;
    size_t cursor;
    size_t line;
    size_t line_start;

    cplL_StateType current_state;
    cplL_StateType next_state;

    int parentheses_balancer;
    int quotes_balancer;
} cplL_State;

const char* cplL_token_type_to_cstr(cplL_TokenType type);
char* cplL_token_to_cstr(cplL_Token* token);

void cplL_print_token(cplL_Token* token);
bool cplL_is_token_equal(cplL_Token* token, const char* data);

bool cplL_is_digit(char c);
bool cplL_is_lower(char c);
bool cplL_is_upper(char c);
bool cplL_is_newline(char c);
bool cplL_is_whitespace(char c);
bool cplL_is_operator_char(char c);
bool cplL_is_symbol_char(char c);
bool cplL_is_quote(char c);
bool cplL_is_left_paren(char c);
bool cplL_is_right_paren(char c);
bool cplL_is_numeric_prefix(char c);
bool cplL_is_bin(char c);
bool cplL_is_hex(char c);

void cplL_start_token(cplL_State* state, cplL_Token* token, cplL_TokenType type, cplL_StateType nextState, bool push);
void cplL_append_char(cplL_State* state, cplL_Token* token, cplL_StateType nextState);

bool cplL_check_eof(cplL_State* state);

// Returns true if we've got a token and returns false in a case of invalid token
// or an end of the file
bool cplL_next_token(cplL_State* state, cplL_Token* token);

#endif // CPL_LEXER_H_
