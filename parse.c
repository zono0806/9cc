#include "9cc.h"

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