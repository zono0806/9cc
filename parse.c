#include "9cc.h"

Node *code[100];

// 新しいNodeの作成
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

// NUMのノードを作り、引数の値を代入する。
Node *new_num(int val){
  Node *node = new_node(NO_NUM);
  node->val = val;
  return node;
}

Node *program();
Node *stmt();
Node *assign();
Node *expr();
Node *equality();
Node *relational();
Node *add();
Node *mul();
Node *unary();
Node *primary();


// BNF
// program    = stmt
// stmt       = expr ";"
// expr       = assign
// assign     = equality ("=" assign)?
// equality   = relational ("==" relational | "!=" relational)*
// relational = add ("<" add | "<=" add | ">" add | ">=" add)* 
// add        = mul ( "+" mul | "-" mul)*
// mul        = unary ( "*" unary | "/" unary )*
// unary      = ( "+" | "-" )? primary
// primary    = num | ident | "(" expr ")"


// program = stmt;
Node *program(){
  //int i = 0;
  Node head;
  head.next = NULL;
  Node *cur = &head;

  while(!at_eof()){
    cur->next = stmt();
    cur = cur -> next;
  }

  return head.next;
  
  //while(!at_eof()){
  //code[i++] = stmt();
  //}
  //code[i] = NULL;
}

// stmt = expr ";"
Node *stmt(){
  Node *node = expr();
  expect(";");
  return node;
}

// expr = equality
Node *expr(){
  return assign();
}

// assign = equality ( "=" assign)?
Node *assign(){
  Node *node = equality();
  if(consume("="))
    return node = new_binary(NO_ASSIGN, node, assign());
  return node;
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
  // 数字はここでノードになる
  return new_num(expect_number());
}
