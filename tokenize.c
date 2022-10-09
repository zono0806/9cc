#include "9cc.h"

//
// tokenize.c
//

Token *token;
// char *user_input;

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
// 受け取った文字をconsumeする。
// 
// トークンの種類がreserved or 
// 文字長がトークンに保存された長さと同じ or
// トークンの文字列と解析してる文字の長さが同じ
// 
// 
// extern int memcmp (const void *__s1, const void *__s2, size_t __n)
// memcmp:  Compare N bytes of S1 and S2. 
bool consume(char *op){
  if (token->kind != TK_RESERVED || strlen(&(op)) != token->len || memcmp(token->str, &(op), token->len))
    return false;
  // 次のトークンにつなげる
  token = token->next;
  return true;
}

//if the next token is expected token, read one more token.
void expect(char op){
  if (token->kind != TK_RESERVED || strlen(&(op)) != token->len || memcmp(token->str, &(op), token->len))
    error_at(token->str, "expectd '%c'", op);
  printf("passing5\n");
  token = token->next;
}

//if the next token is number, reads one more token and returns the number.
int expect_number(){
  if(token->kind != TK_NUM)
    error_at(token->str, "Not a number");
  // トークンの値を取り出し、その値を返り値にする。
  int val = token->val;
  // トークンを次に進める。
  token = token->next;
  return val;
}

bool at_eof(){
  return token->kind ==TK_EOF;
}


//generate new token and pass to cur
Token *new_token(TokenKind kind, Token *cur, char *str, int len){
  // 新たなトークンを生成して、returnする。
  Token *tok = calloc(1, sizeof(Token));
  tok->kind = kind;
  tok->str = str;
  tok->len = len;
  //トークナイズ済みの文字を指しているcurに、次のトークンとしてつなげる。
  cur->next = tok;
  return tok;
}

bool startswith(char *p, char *q){
  return memcmp(p, q, strlen(q)) == 0;
}

// トークンに分解
Token *tokenize(){

  // user_inputはグローバル変数
  char *p = user_input;
  printf("FIRST p : %s\n", p);
    
  //headを生成
  Token head;
  head.next = NULL;
  
  // curの指す先をheadに向ける
  Token *cur = &head;

  while(*p){

    printf("cur->val: %d\n", cur->val);

    // if space 
    if(isspace(*p)){
      // pを進める
      p++;
      continue;
    }

    if(startswith(p, "==") || startswith(p, "!=") || startswith(p, "<=") || startswith(p, ">=")){
      // printf("p : %s\n", p);
      cur = new_token(TK_RESERVED, cur, p, 2);
      p += 2;
      continue;
    }

    if(strchr("+-*/()<>", *p)) {
      cur = new_token(TK_RESERVED, cur, p++, 1);
      // printf("p : %s\n", p);
      continue;
    }
    
    printf("p : %s\n", p);

    // strtol function proceed the p to next letter so we do not need p++.
    // 数字は一文字ずつではなくスペースや記号が来るまでトークナイズする。
    if(isdigit(*p)){
      printf("p : %s\n", p);
      cur = new_token(TK_NUM, cur, p, 0);
      char *q = p;
      // cur->valはpが数字の場合だけ
      cur->val = strtol(p, &p, 10);
      cur->len = p - q;
      printf("cur; %d\n", cur->val);
      continue;
    }

    // tokenize alphabet letter
    if('a' <= *p && *p <= 'z') {
      cur = new_token(TK_IDENT, cur, p++,1);
      cur -> len = 1;
      continue;
    }
    
    error_at(p,"invalid token...");
  }

  new_token(TK_EOF, cur, p, 0);

  // 
  // struct Token{
  //   TokenKind kind; //type of token
  //   Token *next;    //next token
  //   int val;        //Token value  
  //   char *str;      //token
  //   int len;        //the length of token
  // }
  // トークンの構造は上記の通り。
  return head.next;
}
