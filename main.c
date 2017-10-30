#include"file.h"

int main() {
	reg teste;
	teste.code = 1;
	strcpy(teste.desc,"Lorem ipsum dolor sit amet.");
	teste.ano = 1998;
	teste.valor = 1962;
	int index;
	int key;
		while (index != (-1)){
		printf("\n\nSelecione sua opcao:\n"
			       "(1) - Criar um arquivo novo.\n"
			       "(2) - Adcionar um registro.\n"
			       "(3) - Remover um registro.\n"
			       "(4) - Procurar registro.\n"
			       "(5) - Listagem de registros.\n"
			       "(6) - Sair.\n");
		printf("Sua opção:");
		scanf("%d", &index);
		system("clear");
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
				do{
					printf("Digite a chave do registro que deseja encontrar:");
					scanf("%d", &key);
				}while(key <= 0);
				procuraReg(key);
			break;
			case 5:
				listaReg();
			break;
			case 6:
				index = -1;
			break;
		}
	}
	return 0;
}