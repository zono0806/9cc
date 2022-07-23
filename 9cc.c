#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

typedef enum{
	     TK_RESERVED,
	     TK_NUM,
	     TK_EOF,
} TokenKind;

typedef struct Token Token;

struct Token{
  TokenKind kind; //type of token
  Token *next; //next token
  int val;
  char *str;
};

//current token
// global variable
Token *token;

//input program
char *user_input;

//reports an error and exits
// loc : 入力途中の文字を指すポインタ
// 先頭とlocの差をとるとエラーが発生した場所までのバイト数が得られる
void error_at(char *loc, char *fmt, ...){
  va_list ap;
  va_start(ap, fmt);
  int pos = loc - user_input;
  fprintf(stderr, "%s\n", user_input);
  fprintf(stderr, "%*s", pos, "");
  fprintf(stderr, "^ ");
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}
    
void error(char *fmt, ...){
  va_list ap;
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

//if the next token is expected token, this function returns true, else false.
bool consume(char op){
  if (token->kind != TK_RESERVED || token->str[0] != op)
    return false;
  token = token->next;
  return true;
}

//if the next token is expected token, read one more token.
void expect(char op){
	if (token->kind != TK_RESERVED || token->str[0] != op)
		error_at(token->str, "expectd '%c'", op);
	token = token->next;
}

//if the next token is number, reads one more token and returns the number.
int expect_number(){
  if(token->kind != TK_NUM)
    error_at(token->str, "Not a number");
    int val = token->val;
    token = token->next;
    return val;
}

bool at_eof(){
  return token->kind ==TK_EOF;
}


//generate new token and pass to cur
Token *new_token(TokenKind kind, Token *cur, char *str){
  //printf("str: %s\n ", str);
  Token *tok = calloc(1, sizeof(Token));
  tok->kind = kind;
  tok->str = str;
  cur->next = tok;
  // return cur->nextと同じ動作をするのか？
  return tok;
}

// トークンに分解
Token *tokenize(){
  char *p = user_input;
  //headを生成
  Token head;
  head.next = NULL;
  // curの指す先をheadに向ける
  Token *cur = &head;

  while(*p){
    // if space 
    if(isspace(*p)){
      // pを進める
      p++;
      continue;
    }

    // if + or -
    if(*p == '+' || *p == '-'){
      cur = new_token(TK_RESERVED, cur, p++);
      continue;
    }
    
    // strtol function proceed the p to next letter so we do not need p++.
    if(isdigit(*p)){
      cur = new_token(TK_NUM, cur, p);
      cur->val = strtol(p, &p, 10);
      continue;
    }

    error_at(p,"expected a number");
  }

  new_token(TK_EOF, cur, p);
  return head.next;
}


int main(int argc, char **argv){
  //コマンドライン引数が二つではない場合、エラー
  if(argc != 2){
    error("invalid ammount of argument");
    return 1;
  }
  //user_inputにソースコードを代入する
  user_input = argv[1];	
  token = tokenize();

  // char *p = argv[1];
  printf(".intel_syntax noprefix\n");
  printf(".global main\n");
  printf("main:\n");
  // Assume token's first letter is number.
  printf("	mov rax, %d\n", expect_number());


  while(!at_eof()){
    if(consume('+')){
			printf("	add rax, %d\n", expect_number());
			continue;
		}
		if(consume('-')){
		        printf("	sub rax, %d\n", expect_number());
		        continue;
		}

	}
	printf("	ret\n");
	return 0;
}
