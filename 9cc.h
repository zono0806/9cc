#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>




//
// tokenize.c
//
typedef enum{
	     TK_RESERVED,
	     TK_IDENT, // 識別子
	     TK_NUM,
	     TK_EOF,
} TokenKind;

typedef struct Token Token;

struct Token{
  TokenKind kind; //type of token
  Token *next;    //next token
  int val;        //Token value  
  char *str;      //token
  int len;        //the length of token
};

Token *token;
char *user_input;

void error_at(char *loc, char *fmt, ...);    
void error(char *fmt, ...);
bool consume(char *op);
void expect(char op);
int expect_number();
bool at_eof();
Token *new_token(TokenKind kind, Token *cur, char *str, int len);
bool startswith(char *p, char *q);
Token *tokenize();



//
// parse.c
//
typedef enum{
        NO_ADD,
        NO_SUB,
        NO_MUL,
        NO_DIV,
        NO_NUM,
        NO_EQ,
        NO_NE,
        NO_LT,
        NO_LE,
	NO_ASSIGN,
}NodeKind;

typedef struct Node Node;

struct Node {
  NodeKind kind;
  Node *lhs;
  Node *rhs;
  Node *next;
  int val;
  int offset;
};

Node *expr();



//
// codegen.c
//
void codegen(Node *node);
