#include "9cc.h"

//
// tokenize.c
//

Token *token;
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
bool consume(char *op){
  if (token->kind != TK_RESERVED || strlen(op) != token->len || memcmp(token->str, op, token->len))
    return false;
  token = token->next;
  return true;
}

//if the next token is expected token, read one more token.
void expect(char op){
  if (token->kind != TK_RESERVED || strlen(op) != token->len || memcmp(token->str, op, token->len))
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
Token *new_token(TokenKind kind, Token *cur, char *str, int len){
  //printf("str: %s\n ", str);
  Token *tok = calloc(1, sizeof(Token));
  tok->kind = kind;
  tok->str = str;
  tok->len = len;
  cur->next = tok;
  // return cur->nextと同じ動作をするのか？
  return tok;
}

bool startswith(char *p, char *q){
  return memcmp(p, q, strlen(q)) == 0;
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

    if(startswith(p, "==") || startswith(p, "!=") || startswith(p, "<=") || startswith(p, ">=")){
      cur = new_token(TK_RESERVED, cur, p, 2);
      p += 2;
      continue;
    }

    if(strchr("+-*/()<>", *p)) {
      cur = new_token(TK_RESERVED, cur, p++, 1);
      continue;
    }
    
    // strtol function proceed the p to next letter so we do not need p++.
    if(isdigit(*p)){
      cur = new_token(TK_NUM, cur, p, 0);
      char *q = p;
      cur->val = strtol(p, &p, 10);
      cur->len = p - q;
      continue;
    }

    error_at(p,"invalid token...");
  }

  new_token(TK_EOF, cur, p, 0);
  return head.next;
}