#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>


//
// Tokenizer
//
typedef enum{
	     TK_RESERVED,
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


  //
  // parser
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
  }NodeKind;

  typedef struct Node Node;

  struct Node {
    NodeKind kind;
    Node *lhs;
    Node *rhs;
    int val;
  };

  Node *new_node(NodeKind kind){
    Node *node = calloc(1, sizeof(Node));
    node->kind = kind;
    return node;
  }

  Node *new_binary(NodeKind kind, Node *lhs, Node *rhs){
    Node *node = new_node(kind);
    node->lhs = lhs;
    node->rhs = rhs;
    return node;

  }

  Node *new_num(int val){
    Node *node = new_node(NO_NUM);
    node->val = val;
    return node;
  }

  Node *expr();
  Node *equality();
  Node *relational();
  Node *add();
  Node *mul();
  Node *unary();
  Node *primary();


// BNF
// expr       = equality
// equality   = relational ("==" relational | "!=" relational)*
// relational = add ("<" | "<=" add | ">" add | ">=" add)* 
// add        = mul ( "+" mul | "-" mul)*
// mul        = unary ( "*" unary | "/" unary )*
// unary      = ( "+" | "-" )? primary
// primary    = num | "(" expr ")"


  // expr = equality
  Node *expr(){
    return equality();
  }

  //equality = relational("==" relational | "!=" relational)*;
  Node *equality(){
    Node *node = relational();

    for(;;){
      if(consume("=="))
	node = new_binary(NO_EQ, node, relational());
      else if(consume("!="))
	node = new_binary(NO_NE, node, relational());
      else
	return node;
    }
  }

  //relational = add("<" add | "<=" add | ">" add | ">=" add)
  Node *relational(){
    Node *node = add();

    for(;;){
      if(consume("<"))
	node = new_binary(NO_LT, node, add());
      else if(consume("<="))
	node = new_binary(NO_LE, node, add());
      else if(consume(">"))
	node = new_binary(NO_LT, add(), node);
      else if(consume(">="))
	node = new_binary(NO_LE, add(), node);
      else
	return node;
    }

    
  }

  // add = mul ( "+" mul | "-" mul)*
  Node *add(){
    Node *node = mul();

    for(;;){
	if(consume("+"))
	  node = new_binary(NO_ADD, node, mul());
	else if(consume("-"))
	  node = new_binary(NO_SUB, node, mul());
	else
	  return node;
    }

  }

    
  
  // mul = unary("*" unary | "/" unary)*
  Node *mul(){
    Node *node = unary();

    for(;;){
      if(consume("*"))
	node = new_binary(NO_MUL, node, unary());
      else if(consume("/"))
	node = new_binary(NO_DIV, node, unary());
      else
	return node;
    }

  }


  // unary = ( "+" | "-" )? | unary
  Node *unary(){
    if(consume("+"))
      return unary();
    if(consume("-"))
      return new_binary(NO_SUB, new_num(0), unary());
    return primary();
    
  }


  // primary = num | "(" expr ")"
  Node *primary(){
    if(consume("(")){
      Node *node = expr();
      expect(")");
      return node;
    }

    return new_num(expect_number());
  }



  //
  // code generator
  //
  void gen(Node *node){
    if(node->kind == NO_NUM){
      printf("    push %d\n", node->val);
      return;
    }

    gen(node->lhs);
    gen(node->rhs);

    printf("    pop rdi\n");
    printf("    pop rax\n");

    switch(node->kind){
    case NO_ADD:
      printf("    add rax, rdi\n");
      break;
    case NO_SUB:
      printf("    sub rax, rdi\n");
      break;
    case NO_MUL:
      printf("    imul rax, rdi\n");
      break;
    case NO_DIV:
      printf("    cqo\n");
      printf("    idiv rdi\n");
      break;
    case NO_EQ:
      printf("    cmp rax, rdi\n");
      printf("    sete al\n");
      printf("    movzb rax, al\n");
      break;
    case NO_NE:
      printf("    cmp rax, rdi\n");
      printf("    setne al\n");
      printf("    movzb rax, al\n");
      break;
    case NO_LT:
      printf("    cmp rax, rdi\n");
      printf("    setl al\n");
      printf("    movzb rax, al\n");
      break;
    case NO_LE:
      printf("    cmp rax, rdi\n");
      printf("    setle al\n");
      printf("    movzb rax, al\n");
      break;

    }

    printf("    push rax\n");
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
  Node *node = expr();
  
  printf(".intel_syntax noprefix\n");
  printf(".global main\n");
  printf("main:\n");

  // Traverse the AST to emit assembly
  gen(node);

  printf("    pop rax\n");
  printf("    ret\n");

  return 0;
}
