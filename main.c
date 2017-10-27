#include"file.h"

int main() {
	reg teste1;
	teste1.code = 1;
	teste1.ano = 1998;
	teste1.valor = 3000;
	insereReg(teste1);
	/*
	if(criaArquivo()){
		printf("Arquivo criado com sucesso.\n");
	}else{
		printf("Erro na criação do arquivo.\n");
	}
	*/
	return 0;
}