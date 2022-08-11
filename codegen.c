#include "9cc.h"

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

void codegen(Node *node){

  printf(".intel_syntax noprefix\n");
  printf(".global main\n");
  printf("main:\n");

  // Traverse the AST to emit assembly
  gen(node);

  printf("    pop rax\n");
  printf("    ret\n");
}