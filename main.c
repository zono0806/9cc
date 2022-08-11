#include "9cc.h"

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

  codegen(node);

  return 0;
}
