#include"file.h"

int main() {
	bloco teste;
	reg teste1;
	float f = sizeof(teste);
	criaArquivo();
	printf("%f\n", f);
	f = sizeof(teste1);
	printf("%f\n", f);
	return 0;
}