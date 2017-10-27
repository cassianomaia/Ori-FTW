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
	printf("init\n");
	int offset = 0; //variavel para correr o arquivo
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
			printf("inside while\n");
			if(strcmp(temp->header, "#BLK")){
				printf("Bloco validado.\n");
				while(regn < 6){
					printf("Procurando registro vazio.\n");
					if(temp->index[regn].code == 0){
						printf("Escrevendo dados.\n");
						temp->index[regn] = newreg;
						fseek(arquivo, blocon*tamBloco, SEEK_SET);
              			fwrite(temp, tamBloco, 1, arquivo);
              			fclose(arquivo);
						return 1;
					}else{
						printf("Procurando prox registro.\n");
						regn++;
					}
				}
				blocon++;	
			}else{
				printf("Inconsisencia de dados detectada, o arquivo foi corrompido.\n");
				return 0;
			}
		}
	}
}
/*
int removeReg(int*, char*)
void leReg(reg*)
void escreveReg(reg)
int procuraReg(reg*, int*, char*)
bool compactaArquivo(char*)
*/