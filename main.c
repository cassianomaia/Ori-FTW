#include"file.h"

int main() {
	bloco teste;
	reg teste1;
	float f = sizeof(teste);
	if(criaArquivo()){
		printf("Arquivo criado com sucesso.\n");
	}else{
		printf("Erro na criação do arquivo.\n");
	}
	printf("%f\n", f);
	f = sizeof(teste1);
	printf("%f\n", f);
	return 0;
}