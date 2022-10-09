#include "9cc.h"

int main(int argc, char **argv){

  //コマンドライン引数が二つではない場合、エラー
  if(argc != 2){
    error("invalid ammount of argument");
    return 1;
  }

  // user_inputにソースコードを代入する
  // 引数からは直接コードが渡される
  user_input = argv[1];	

  // 9cc.hにtokenizeが宣言されている
  // 入力文字列を一文字ずつトークンに分ける
  token = tokenize();
  Node *node = program();

  codegen(node);

  return 0;
}
