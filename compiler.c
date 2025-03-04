#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <setjmp.h>
jmp_buf jmp_buffer;
typedef enum
{
    TOKEN_VAR,
    TOKEN_ASSIGN,
    TOKEN_PRINT,
    TOKEN_IF,
    TOKEN_WHILE,
    TOKEN_FOR,
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_LBRACE,
    TOKEN_RBRACE,
    TOKEN_SEMICOLON,
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_MUL,
    TOKEN_DIV,
    TOKEN_LT,
    TOKEN_GT,
    TOKEN_LE,
    TOKEN_GE,
    TOKEN_EQ,
    TOKEN_NE,
    TOKEN_NUMBER,
    TOKEN_EOF,
    TOKEN_INVALID
} TokenType;

typedef struct
{
    TokenType type;
    char value[256];
} Token;

typedef struct
{
    Token *tokens;
    int current;
} Lexer;

typedef struct
{
    char var_name[256];
    int value;
} Variable;

#define MAX_VARS 256
Variable vars[MAX_VARS];
int num_vars = 0;

void add_variable(const char *name, int value);
int get_variable_value(const char *name);
Lexer lexer_init(char *input);
int parse_expr(Lexer *lexer);
int parse_term(Lexer *lexer);
int parse_factor(Lexer *lexer);
Token lexer_next_token(Lexer *lexer);
void parse_assignment(Lexer *lexer, Token var_token);
void parse_print(Lexer *lexer, FILE *output_file);
void parse_while(Lexer *lexer, FILE *output_file);
void parse_if(Lexer *lexer, FILE *output_file);
void parse_for(Lexer *lexer, FILE *output_file);
int parse_condition(Lexer *lexer);

void add_variable(const char *name, int value)
{

    for (int i = 0; i < num_vars; i++)
    {
        if (strcmp(vars[i].var_name, name) == 0)
        {

            vars[i].value = value;
            return;
        }
    }

    if (num_vars < MAX_VARS)
    {
        strcpy(vars[num_vars].var_name, name);
        vars[num_vars].value = value;
        num_vars++;
    }
    else
    {

        printf("Error: Maximum number of variables reached.\n");
        longjmp(jmp_buffer, 1);
    }
}

int get_variable_value(const char *name)
{
    for (int i = 0; i < num_vars; i++)
    {
        if (strcmp(vars[i].var_name, name) == 0)
        {
            return vars[i].value;
        }
    }

    fprintf(stderr, "Error: Variable '%s' not found.\n", name);
    longjmp(jmp_buffer, 1);

    return 0;
}


Lexer lexer_init(char *input)
{
    Lexer lexer;
    lexer.tokens = malloc(1024 * sizeof(Token));
    lexer.current = 0;

    int i = 0, token_idx = 0;
    while (input[i] != '\0')
    {
        if (isspace(input[i]) || input[i] == '\n')
        {
            i++;
        }
        else if (isdigit(input[i]))
        {
            int start = i;
            while (isdigit(input[i]))
                i++;
            strncpy(lexer.tokens[token_idx].value, input + start, i - start);
            lexer.tokens[token_idx].value[i - start] = '\0';
            lexer.tokens[token_idx].type = TOKEN_NUMBER;
            token_idx++;
        }
        else if (strncmp(input + i, "print", 5) == 0)
        {
            lexer.tokens[token_idx].type = TOKEN_PRINT;
            strncpy(lexer.tokens[token_idx].value, "print", 5);
            lexer.tokens[token_idx].value[5] = '\0';
            token_idx++;
            i += 5;
        }
        else if (strncmp(input + i, "while", 5) == 0)
        {
            lexer.tokens[token_idx].type = TOKEN_WHILE;
            strncpy(lexer.tokens[token_idx].value, "while", 5);
            lexer.tokens[token_idx].value[5] = '\0';
            token_idx++;
            i += 5;
        }
        else if (strncmp(input + i, "if", 2) == 0)
        {
            lexer.tokens[token_idx].type = TOKEN_IF;
            strncpy(lexer.tokens[token_idx].value, "if", 2);
            lexer.tokens[token_idx].value[2] = '\0';
            token_idx++;
            i += 2;
        }
        else if (strncmp(input + i, "for", 3) == 0)
        {
            lexer.tokens[token_idx].type = TOKEN_FOR;
            strncpy(lexer.tokens[token_idx].value, "for", 3);
            lexer.tokens[token_idx].value[3] = '\0';
            token_idx++;
            i += 3;
        }

        else if (isalpha(input[i]))
        {
            int start = i;
            while (isalnum(input[i]))
                i++;
            strncpy(lexer.tokens[token_idx].value, input + start, i - start);
            lexer.tokens[token_idx].value[i - start] = '\0';
            lexer.tokens[token_idx].type = TOKEN_VAR;
            token_idx++;
        }
        else if (input[i] == '+')
        {
            lexer.tokens[token_idx].type = TOKEN_PLUS;
            lexer.tokens[token_idx].value[0] = '+';
            lexer.tokens[token_idx].value[1] = '\0';
            token_idx++;
            i++;
        }
        else if (input[i] == '-')
        {
            lexer.tokens[token_idx].type = TOKEN_MINUS;
            lexer.tokens[token_idx].value[0] = '-';
            lexer.tokens[token_idx].value[1] = '\0';
            token_idx++;
            i++;
        }
        else if (input[i] == '*')
        {
            lexer.tokens[token_idx].type = TOKEN_MUL;
            lexer.tokens[token_idx].value[0] = '*';
            lexer.tokens[token_idx].value[1] = '\0';
            token_idx++;
            i++;
        }
        else if (input[i] == '/')
        {
            lexer.tokens[token_idx].type = TOKEN_DIV;
            lexer.tokens[token_idx].value[0] = '/';
            lexer.tokens[token_idx].value[1] = '\0';
            token_idx++;
            i++;
        }
        else if (input[i] == '(')
        {
            lexer.tokens[token_idx].type = TOKEN_LPAREN;
            lexer.tokens[token_idx].value[0] = '(';
            lexer.tokens[token_idx].value[1] = '\0';
            token_idx++;
            i++;
        }
        else if (input[i] == ')')
        {
            lexer.tokens[token_idx].type = TOKEN_RPAREN;
            lexer.tokens[token_idx].value[0] = ')';
            lexer.tokens[token_idx].value[1] = '\0';
            token_idx++;
            i++;
        }
        else if (input[i] == '{')
        {
            lexer.tokens[token_idx].type = TOKEN_LBRACE;
            lexer.tokens[token_idx].value[0] = '{';
            lexer.tokens[token_idx].value[1] = '\0';
            token_idx++;
            i++;
        }
        else if (input[i] == '}')
        {
            lexer.tokens[token_idx].type = TOKEN_RBRACE;
            lexer.tokens[token_idx].value[0] = '}';
            lexer.tokens[token_idx].value[1] = '\0';
            token_idx++;
            i++;
        }
        else if (input[i] == ';')
        {
            lexer.tokens[token_idx].type = TOKEN_SEMICOLON;
            lexer.tokens[token_idx].value[0] = ';';
            lexer.tokens[token_idx].value[1] = '\0';
            token_idx++;
            i++;
        }
        else if (input[i] == '<')
        {
            if (input[i + 1] == '=')
            {
                lexer.tokens[token_idx].type = TOKEN_LE;
                strncpy(lexer.tokens[token_idx].value, "<=", 2);
                lexer.tokens[token_idx].value[2] = '\0';
                token_idx++;
                i += 2;
            }
            else
            {
                lexer.tokens[token_idx].type = TOKEN_LT;
                lexer.tokens[token_idx].value[0] = '<';
                lexer.tokens[token_idx].value[1] = '\0';
                token_idx++;
                i++;
            }
        }
        else if (input[i] == '>')
        {
            if (input[i + 1] == '=')
            {
                lexer.tokens[token_idx].type = TOKEN_GE;
                strncpy(lexer.tokens[token_idx].value, ">=", 2);
                lexer.tokens[token_idx].value[2] = '\0';
                token_idx++;
                i += 2;
            }
            else
            {
                lexer.tokens[token_idx].type = TOKEN_GT;
                lexer.tokens[token_idx].value[0] = '>';
                lexer.tokens[token_idx].value[1] = '\0';
                token_idx++;
                i++;
            }
        }
        else if (input[i] == '=')
        {
            if (input[i + 1] == '=')
            {
                lexer.tokens[token_idx].type = TOKEN_EQ;
                strncpy(lexer.tokens[token_idx].value, "==", 2);
                lexer.tokens[token_idx].value[2] = '\0';
                token_idx++;
                i += 2;
            }
            else
            {
                lexer.tokens[token_idx].type = TOKEN_ASSIGN;
                lexer.tokens[token_idx].value[0] = '=';
                lexer.tokens[token_idx].value[1] = '\0';
                token_idx++;
                i++;
            }
        }
        else if (input[i] == '!')
        {
            if (input[i + 1] == '=')
            {
                lexer.tokens[token_idx].type = TOKEN_NE;
                strncpy(lexer.tokens[token_idx].value, "!=", 2);
                lexer.tokens[token_idx].value[2] = '\0';
                token_idx++;
                i += 2;
            }
        }
        else
        {
            lexer.tokens[token_idx].type = TOKEN_INVALID;
            lexer.tokens[token_idx].value[0] = input[i];
            lexer.tokens[token_idx].value[1] = '\0';
            token_idx++;
            i++;
        }
    }
    lexer.tokens[token_idx].type = TOKEN_EOF;
    lexer.tokens[token_idx].value[0] = '\0';
    return lexer;
}

Token lexer_next_token(Lexer *lexer)
{
    return lexer->tokens[lexer->current++];
}

int parse_expr(Lexer *lexer)
{
    int result = parse_term(lexer);

    while (1)
    {
        Token token = lexer->tokens[lexer->current];
        if (token.type == TOKEN_PLUS)
        {
            lexer_next_token(lexer);
            result += parse_term(lexer);
        }
        else if (token.type == TOKEN_MINUS)
        {
            lexer_next_token(lexer);
            result -= parse_term(lexer);
        }
        else
        {
            break;
        }
    }
    return result;
}

int parse_term(Lexer *lexer)
{
    int result = parse_factor(lexer);

    while (1)
    {
        Token token = lexer->tokens[lexer->current];
        if (token.type == TOKEN_MUL)
        {
            lexer_next_token(lexer);
            result *= parse_factor(lexer);
        }
        else if (token.type == TOKEN_DIV)
        {
            lexer_next_token(lexer);
            int divisor = parse_factor(lexer);
            if (divisor == 0)
            {
                fprintf(stderr, "Error: Division by zero.\n");
                longjmp(jmp_buffer, 1);
            }
            result /= divisor;
        }
        else
        {
            break;
        }
    }
    return result;
}

int parse_factor(Lexer *lexer)
{

    Token token = lexer_next_token(lexer);

    if (token.type == TOKEN_NUMBER)
    {
        return atoi(token.value);
    }
    else if (token.type == TOKEN_VAR)
    {
        return get_variable_value(token.value);
    }
    else if (token.type == TOKEN_LPAREN)
    {
        int result = parse_expr(lexer);
        lexer_next_token(lexer);
        return result;
    }

    return 0;
}

void parse_assignment(Lexer *lexer, Token var_token)
{

    if (var_token.type == TOKEN_VAR)
    {
        Token assign_token = lexer_next_token(lexer);

        if (assign_token.type == TOKEN_ASSIGN)
        {

            int value = parse_expr(lexer);

            Token semicolon_token = lexer_next_token(lexer);
            if (semicolon_token.type == TOKEN_SEMICOLON)
            {

                add_variable(var_token.value, value);
            }
            else
            {

                fprintf(stderr, "Error: Expected ';' after assignment.\n");
                longjmp(jmp_buffer, 1);
            }
        }
        else
        {

            fprintf(stderr, "Error: Expected '=' after variable name.\n");
            longjmp(jmp_buffer, 1);
        }
    }
    else
    {

        fprintf(stderr, "Error: Expected variable name.\n");
        longjmp(jmp_buffer, 1);
    }
}

int parse_condition(Lexer *lexer)
{
    int left = parse_expr(lexer);

    Token token = lexer->tokens[lexer->current];
    lexer_next_token(lexer);

    int right = parse_expr(lexer);

    switch (token.type)
    {
    case TOKEN_LT:
        return left < right;
    case TOKEN_GT:
        return left > right;
    case TOKEN_LE:
        return left <= right;
    case TOKEN_GE:
        return left >= right;
    case TOKEN_EQ:
        return left == right;
    case TOKEN_NE:
        return left != right;
    default:
        fprintf(stderr, "Error: Invalid condition.\n");
        longjmp(jmp_buffer, 1);
        return 0;
    }
}
void parse_for(Lexer *lexer, FILE *output_file)
{
    lexer_next_token(lexer);

    Token init_var = lexer_next_token(lexer);
    parse_assignment(lexer, init_var);

    int condition_start = lexer->current;

    int condition = parse_condition(lexer);

    lexer_next_token(lexer);

    int increment_start = lexer->current;

    Token increment_var = lexer_next_token(lexer);
    lexer_next_token(lexer);
    int increment_value = parse_expr(lexer);

    lexer_next_token(lexer);
    lexer_next_token(lexer);

    int body_start = lexer->current;

    while (condition)
    {
        lexer->current = body_start;

        while (lexer->tokens[lexer->current].type != TOKEN_RBRACE)
        {
            Token token = lexer_next_token(lexer);
            if (token.type == TOKEN_VAR)
            {
                parse_assignment(lexer, token);
            }
            else if (token.type == TOKEN_IF)
            {
                parse_if(lexer, output_file);
            }
            else if (token.type == TOKEN_PRINT)
            {
                parse_print(lexer, output_file);
            }
        }

        lexer->current = increment_start;
        int current_value = get_variable_value(increment_var.value);
        add_variable(increment_var.value, current_value + increment_value);

        lexer->current = condition_start;
        condition = parse_condition(lexer);
    }

    while (lexer->tokens[lexer->current].type != TOKEN_RBRACE)
    {
        lexer_next_token(lexer);
    }
    lexer_next_token(lexer);
}

void parse_if(Lexer *lexer, FILE *output_file)
{
    lexer_next_token(lexer);
    int condition = parse_condition(lexer);
    lexer_next_token(lexer);
    lexer_next_token(lexer);

    if (condition)
    {
        while (lexer->tokens[lexer->current].type != TOKEN_RBRACE)
        {
            Token token = lexer_next_token(lexer);
            if (token.type == TOKEN_VAR)
            {
                parse_assignment(lexer, token);
            }
            else if (token.type == TOKEN_PRINT)
            {
                parse_print(lexer, output_file);
            }
        }
    }
    else
    {

        while (lexer->tokens[lexer->current].type != TOKEN_RBRACE)
        {
            lexer_next_token(lexer);
        }
    }

    lexer_next_token(lexer);
}

void parse_while(Lexer *lexer, FILE *output_file)
{
    int loop_start = lexer->current - 1;
    lexer_next_token(lexer);

    int condition_start = lexer->current;
    int condition = parse_condition(lexer);
    lexer_next_token(lexer);
    lexer_next_token(lexer);

    int body_start = lexer->current;

    while (condition)
    {
        lexer->current = body_start;

        while (lexer->tokens[lexer->current].type != TOKEN_RBRACE)
        {
            Token token = lexer_next_token(lexer);
            if (token.type == TOKEN_VAR)
            {
                parse_assignment(lexer, token);
            }
            else if (token.type == TOKEN_IF)
            {
                parse_if(lexer, output_file);
            }
            else if (token.type == TOKEN_PRINT)
            {
                parse_print(lexer, output_file);
            }
        }

        lexer->current = condition_start;
        condition = parse_condition(lexer);
    }

    while (lexer->tokens[lexer->current].type != TOKEN_RBRACE)
    {
        lexer_next_token(lexer);
    }
    lexer_next_token(lexer);
}

void parse_print(Lexer *lexer, FILE *output_file)
{
    lexer_next_token(lexer);
    Token var_token = lexer_next_token(lexer);

    if (var_token.type == TOKEN_VAR)
    {
        int value = get_variable_value(var_token.value);
        fprintf(output_file, "%s = %d\n", var_token.value, value);
    }
    else
    {
        fprintf(output_file, "Error: Expected a variable name after 'print'.\n");
    }
    lexer_next_token(lexer);
    Token semicolon_token = lexer_next_token(lexer);
    if (semicolon_token.type != TOKEN_SEMICOLON)
    {
        fprintf(output_file, "Error: Missing semicolon after 'print'.\n");
    }
}

int main(int argc, char *argv[])
{
    if (setjmp(jmp_buffer) == 0)
    {
        if (argc != 3)
        {
            fprintf(stderr, "Usage: %s <input_file> <output_file>\n", argv[0]);
            return 1;
        }

        FILE *input_file = fopen(argv[1], "r");
        if (input_file == NULL)
        {
            perror("Error opening input file");
            return 1;
        }

        fseek(input_file, 0, SEEK_END);
        long file_size = ftell(input_file);
        fseek(input_file, 0, SEEK_SET);

        char *input = malloc(file_size + 1);
        if (input == NULL)
        {
            fprintf(stderr, "Memory allocation failed\n");
            fclose(input_file);
            return 1;
        }

        fread(input, 1, file_size, input_file);
        input[file_size] = '\0';
        fclose(input_file);

        Lexer lexer = lexer_init(input);

        FILE *output_file = fopen(argv[2], "w");
        if (output_file == NULL)
        {
            perror("Error opening output file");
            free(input);
            return 1;
        }

        while (lexer.tokens[lexer.current].type != TOKEN_EOF)
        {
            Token token = lexer_next_token(&lexer);

            if (token.type == TOKEN_VAR)
            {
                Token next_token = lexer.tokens[lexer.current];
                if (next_token.type == TOKEN_SEMICOLON)
                {
                    lexer_next_token(&lexer);
                    add_variable(token.value, 0);
                }
                else if (next_token.type == TOKEN_ASSIGN)
                {
                    parse_assignment(&lexer, token);
                }
            }
            else if (token.type == TOKEN_IF)
            {
                parse_if(&lexer, output_file);
            }
            else if (token.type == TOKEN_WHILE)
            {
                parse_while(&lexer, output_file);
            }
            else if (token.type == TOKEN_FOR)
            {
                parse_for(&lexer, output_file);
            }
            else if (token.type == TOKEN_PRINT)
            {
                parse_print(&lexer, output_file);
            }
        }

        fclose(output_file);
        free(input);
    }
    else
    {
        fprintf(stderr, "An error occurred during execution.\n");
        return 1;
    }

    return 0;
}