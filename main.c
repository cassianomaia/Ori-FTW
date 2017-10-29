#include"file.h"

int main() {
	reg teste;
	teste.code = 1;
	strcpy(teste.desc,"Lorem ipsum dolor sit amet, consectetur cras amet.");
	teste.ano = 1998;
	teste.valor = 1962;
	int index;
		while (index != (-1)){
		printf("\n\nSelecione sua opcao:\n"
			       "(1) - Criar um arquivo novo.\n"
			       "(2) - Adcionar um registro.\n"
			       "(3) - Remover um registro.\n"
			       "(4) - Sair.\n");
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
				insereReg(teste);
			break;
			case 3:
				removeReg(teste.code);
			break;
			case 4:
				index = -1;
			break;
		}
	}
	return 0;
}