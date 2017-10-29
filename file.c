#include "file.h"

bloco* criaBloco(){
	bloco* novo = (bloco*)malloc(sizeof(bloco));
	memset(novo,0,tamBloco);
	strncpy(novo->header, "#BLK", 4);
	return novo;
}

int criaArquivo(){
	FILE* arquivo = fopen("arquivo.txt", "wb");
	if(!arquivo){
		return 0;
	}else{
		bloco* bloco= criaBloco();
		fwrite(bloco, tamBloco, 1, arquivo);
		free(bloco);
		fclose(arquivo);
		return 1;
	}
}

int insereReg(reg newreg){
	int blocon = 0; //para avançar entre os blocos
	int regn = 0;
	FILE* arquivo = fopen("arquivo.txt", "rb+");
	bloco* temp = criaBloco(); //bloco temporario para escrevermos o registro

	if(!arquivo){
		printf("Arquivo nao encontrado!\n");
		return 0;
	}else{
		printf("Arquivo encontrado\n");
		while ((fread(temp,tamBloco,1,arquivo)) != 0){
			if((temp->header[0]=='#')&&(temp->header[1]=='B')&&(temp->header[2]=='L')&&(temp->header[3]=='K')){
				printf("Bloco validado.\n");
				while(regn < 6){
					printf("Procurando registro vazio.\n");
					if(temp->index[regn].code == 0){		 //todo here
						printf("Escrevendo dados.\n");
						temp->index[regn] = newreg;
						fseek(arquivo, blocon*tamBloco, SEEK_SET);
              			fwrite(temp, tamBloco, 1, arquivo);
              			fclose(arquivo);
              			free(temp);
              			fclose(arquivo);
						return 1;
					}else{
						printf("Procurando prox registro.\n");
						regn++;
					}
				}
				blocon++;
				regn = 0;
			}else{
				printf("Inconsistencia de dados detectada, o arquivo foi corrompido.\n");
				return 0;
			}
		}
		printf("Todos os blocos estão cheios.\n");
		temp = criaBloco();
		temp->index[0] = newreg;
		fseek(arquivo, blocon*tamBloco, SEEK_SET);
        fwrite(temp, tamBloco, 1, arquivo);
        free(temp);
        fclose(arquivo);
		return 1;
	}
}

/*
void leReg(reg* regout){		//lê input e coloca em um novo registro
	int key, inAno; 			//reg->codigo, reg->ano
	char inDesc[50]; 			//reg->desc
	float inValor;				//reg->valor
	
	
	printf("Insira o codigo do violino: \n");
	scanf("%d", &key);
	regout.code = key;
	
	printf("Insira uma breve descricao do violino: \n");
	fgets(inDesc, sizeof(inDesc), stdin);
	strncpy(regout.desc, inDesc, tamDesc);
	
	printf("Insira o ano do violino: \n");
	scanf("%d", &inAno);
	regout.ano = inAno;
	
	printf("Insira o valor do violino: \n");
	scanf("%f", &inValor);
	regout.valor = inValor;
}

void escreveReg(reg regout){		//escreve o conteudo de um registro, NÃO TESTEI

	char temp[50];
	unsigned int i; 

	printf("Codigo: %d / Descricao: ", &regout.code);
	memset(temp, 0, sizeof(temp));
	strncpy(temp, regout.desc, tamDesc);
	for (i = 0; i < strlen(temp); i++) {
		putchar(temp[i]);
	}
	printf("/ Ano: %d / Valor: %f", &regout.ano, &regout.valor);

}
*/

int removeReg(int rindex){
	int blocon = 0; //para avançar entre os blocos
	int regn = 0;
	FILE* arquivo = fopen("arquivo.txt", "rb+");
	bloco* temp = criaBloco(); //bloco temporario para escrevermos o registro
	if(!arquivo){
		printf("Arquivo nao encontrado!\n");
		return 0;
	}else{
		printf("Arquivo encontrado\n");
		while ((fread(temp,tamBloco,1,arquivo)) != 0){
			if((temp->header[0]=='#')&&(temp->header[1]=='B')&&(temp->header[2]=='L')&&(temp->header[3]=='K')){
				printf("Bloco validado.\n");
				while(regn < 6){
					printf("Procurando o registro a ser removido.\n");
					if(temp->index[regn].code == rindex){
						printf("Removendo o registro.\n");
						temp->index[regn].code = -1;
						fseek(arquivo, blocon*tamBloco, SEEK_SET);
              			fwrite(temp, tamBloco, 1, arquivo);
              			fclose(arquivo);
              			free(temp);
              			fclose(arquivo);
						return 1;
					}else{
						regn++;
					}
				}
				blocon++;
				regn = 0;
			}else{
				printf("Inconsistencia de dados detectada, o arquivo foi corrompido.\n");
				return 0;
			}
		}
		printf("Registro nao encontrado.\n");
		free(temp);
        fclose(arquivo);
		return 0;
	}
}



/*
int procuraReg(reg*, int*, char*)
bool compactaArquivo(char*)
*/
