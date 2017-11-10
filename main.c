#include"file.h"

int main() {
	//Variaveis para funcionamento do programa
	srand (time(NULL)); //Geração de numeros randomicos para inserçãoo em lotes
	int index;			//Indexação do menu
	int key;			//Chave de proposito geral para as funções
	char desc[50];		//Vetor para armazenar a descrição de um registro que será inserido

		//Loop principal de execução
		while (index != (-1)){
		printf("\n\nSelecione sua opcao:\n"
			       "(1) - Criar um arquivo novo.\n"
			       "(2) - Adcionar um registro.\n"
			       "(3) - Inserção em lotes.\n"
			       "(4) - Remover um registro.\n"
			       "(5) - Procurar registro.\n"
			       "(6) - Listagem de registros.\n"
			       "(7) - Compactar arquivo.\n"
			       "(8) - Sair.\n");
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
			case 2:{
				reg regin;
				printf("Digite o código do registro a ser inserido:\n");
				scanf("%d", &regin.code);
				printf("Digite a descrição do Instrumento:");
				gets(desc);
				fflush(stdin);
				strcpy(regin.desc, desc);
				printf("Digite o ano de fabricação do instrumento:");
				scanf("%d", &regin.ano);
				printf("Digite o valor do instrumento:");
				scanf("%f", &regin.valor);
				FILE* arquivo = fopen("arquivo.txt", "rb+");
				insereReg(regin, arquivo);
				fclose(arquivo);
			break;}
			case 3:{
				FILE* arquivo = fopen("arquivo.txt", "rb+");
				printf("Digite quantos registros deseja inserir:");
				scanf("%d", &key);
				while(key > 0){
					insereReg(registroaleatorio(), arquivo);
					key--;
				}
				fclose(arquivo);
			break;}
			case 4:
				do{
					printf("Digite a chave do registro que deseja remover:");
					scanf("%d", &key);
				}while(key <= 0);
				removeReg(key);
				key = 0;
			break;
			case 5:
				do{
					printf("Digite a chave do registro que deseja encontrar:");
					scanf("%d", &key);
				}while(key <= 0);
				procuraReg(key);
				key = 0;
			break;
			case 6:
				listaReg();
			break;
			case 7:
				compactaArquivo();
			break;
			case 8:
				index = -1;
			break;
		}
	}
	return 0;
}

