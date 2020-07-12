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
	TokenKind kind; //トークンの型
	Token *next; //次の入力トークン
	int val;
	char *str;
};

Token *token;

void error(char *fmt, ...){
	va_list ap;
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	fprintf(stderr, "\n");
	exit(1);
}

//次のトークンが期待している記号の時は、真を返す。それ以外は偽を返す。
bool consume(char op){
	if (token->kind != TK_RESERVED || token->str[0] != op)
		return false;
	token = token->next;
	return true;
}

//次のトークンが期待した文字のとき、トークンを一つ読み進める。
void expect(char op){
	if (token->kind != TK_RESERVED || token->str[0] != op)
		error("'%c'ではありません", op);
	token = token->next;
}

//次のトークンが数値の場合、トークンを一つ読み進め数値を返す。
int expect_number(){
	if(token->kind != TK_NUM)
		error("数ではありません");
	int val = token->val;
	token = token->next;
	return val;
}

bool at_eof(){
	return token->kind ==TK_EOF;
}


//新しいトークンを作成してcurに捧げる
Token *new_token(TokenKind kind, Token *cur, char *str){
	Token *tok = calloc(1, sizeof(Token));
	tok->kind = kind;
	tok->str = str;
	cur->next = tok;
	return tok;
}

Token *tokenize(char *p){
	Token head;
	head.next = NULL;
	Token *cur = &head;

	while(*p){
		if(isspace(*p)){
			p++;
			continue;
		}

		if(*p == '+' || *p == '-'){
			cur = new_token(TK_RESERVED, cur, p++);
			continue;
		}

		if(isdigit(*p)){
			cur = new_token(TK_NUM, cur, p);
			cur->val = strtol(p, &p, 10);
			continue;
		}

		error("トークナイズできません");
	}

	new_token(TK_EOF, cur, p);
	return head.next;
}

int main(int argc, char **argv){
	if(argc != 2){
		error("invalid ammount of argument");
		return 1;
	}
	
	token = tokenize(argv[1]);

	// char *p = argv[1];
	printf(".intel_syntax noprefix\n");
	printf(".global main\n");
	printf("main:\n");
	printf("	mov rax, %d\n", expect_number());


	while(!at_eof()){
		if(consume('+')){
			printf("	add rax, %d\n", expect_number());
			continue;
		}

		expect('-');
		printf("	sub rax, %d\n", expect_number());

	}
	printf("	ret\n");
	return 0;
}
