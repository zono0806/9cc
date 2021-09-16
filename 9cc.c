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

Token *token;

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
		error("This is not '%c'", op);
	token = token->next;
}

//if the next token is number, reads one more token and returns the number.
int expect_number(){
	if(token->kind != TK_NUM)
		error("This is not number.");
	int val = token->val;
	token = token->next;
	return val;
}

bool at_eof(){
	return token->kind ==TK_EOF;
}


//generate new token and pass to cur
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

		perror("can not tokenize.");
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
