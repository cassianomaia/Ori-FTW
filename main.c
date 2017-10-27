#include"file.h"

int main() {
	reg teste1;
	teste1.code = 1;
	strcpy(teste1.desc,"Hello world");
	teste1.ano = 1998;
	teste1.valor = 1962;
	float f = sizeof(teste1);
	printf("%f", f);
	int index;
		while (index != (-1)){
		printf("Selecione sua opcao:\n(1) - Criar um arquivo novo.\n(2) - Adcionar um registro.\n(3) - Sair.\n");
		printf("Sua opção:");
		scanf("%d", &index);
		switch(index){
			case 1:
				if(criaArquivo()){
					printf("Arquivo criado com sucesso.\n");
				}else{
					printf("Erro na criação do arquivo.\n");
				}
			break;
			case 2:
				insereReg(teste1);
			break;
			case 3:
				index = -1;
			break;
		}
	}
	return 0;
}