#include "file.h"

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
			       "(3) - Inserção em lotes.(Instavel)\n"
			       "(4) - Insere do arquivo csv. (O nome deve ser MOCK_DATA.csv)\n"
			       "(5) - Remover um registro.\n"
			       "(6) - Procurar registro.\n"
			       "(7) - Procurar pelo indice.\n"
			       "(8) - Listagem de registros.\n"
			       "(9) - Compactar arquivo.\n"
			       "(10) - Sair.\n");
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
				if(criaArquivo_i()){
					printf("Arquivo de indice criado.\n");
				}else{
					printf("Erro ao criar arquivo de indice.\n");
				}
			break;
			case 2:{
				reg regin;
				indexfield control;
				printf("Digite o código do registro a ser inserido:");
				scanf("%d", &regin.code);
				printf("\n");
				fflush(stdin);
				printf("Digite a descrição do Instrumento:");
				gets(desc);
				fflush(stdin);
				printf("\n");
				strcpy(regin.desc, desc);
				printf("Digite o ano de fabricação do instrumento:");
				scanf("%d", &regin.ano);
				printf("\n");
				printf("Digite o valor do instrumento:");
				scanf("%f", &regin.valor);
				printf("\n");
				FILE* arquivo = fopen("arquivo.txt", "rb+");
				control = insereReg(regin, arquivo);
				fclose(arquivo);
				FILE* index = fopen("index.txt", "rb+");
				insereIndex(control, index);	
				fclose(index);
			break;}
			case 3:{
				FILE* arquivo = fopen("arquivo.txt", "rb+");
				FILE* index = fopen("index.txt", "rb+");
				indexfield control;
				printf("Digite quantos registros deseja inserir:");
				scanf("%d", &key);
				while(key > 0){
					control = insereReg(registroaleatorio(), arquivo);
					insereIndex(control, index);
					key--;
				}
				fclose(arquivo);
				fclose(index);
			break;}
			case 4:
				read_csv();
			break;
			case 5:
				do{
					printf("Digite a chave do registro que deseja remover:");
					scanf("%d", &key);
				}while(key <= 0);
				removeReg(key);
				removeIndex(key);
				key = 0;
			break;
			case 6:
				do{
					printf("Digite a chave do registro que deseja encontrar:");
					scanf("%d", &key);
				}while(key <= 0);
				procuraReg(key);
				key = 0;
			break;
			case 7:
				do{
					printf("Digite a chave do registro que deseja encontrar:");
					scanf("%d", &key);
				}while(key <= 0);
				procuraIndex(key);
				key = 0;
			break;
			case 8:
				listaReg();
			break;
			case 9:
				compactaArquivo();
				compactaArquivo_i();
			break;
			case 10:
				index = -1;
			break;
		}
	}
	return 0;
}
