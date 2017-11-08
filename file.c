#include "file.h"

bloco* criaBloco(){
	bloco* novo = (bloco*)malloc(sizeof(bloco));
	memset(novo,0,tamBloco);
	strncpy(novo->header, "#BLK", 4);
	return novo;
}

blocoinicial* criaBlocoInicial(){
	blocoinicial* novo = (blocoinicial*)malloc(sizeof(blocoinicial));
	memset(novo,0,tamBloco);
	strncpy(novo->header, "#BLK", 4);
	novo->nblocos = 1;
	novo->nregistros = 0;
	return novo;
}

int criaArquivo(){
	FILE* arquivo = fopen("arquivo.txt", "wb");
	if(!arquivo){
		return 0;
	}else{
		blocoinicial* bloco= criaBlocoInicial();
		fwrite(bloco, tamBloco, 1, arquivo);
		free(bloco);
		fclose(arquivo);
		return 1;
	}
}

int criaTempArquivo(){
	FILE* arquivo = fopen("temparquivo.txt", "wb");
	if(!arquivo){
		return 0;
	}else{
		blocoinicial* bloco= criaBlocoInicial();
		fwrite(bloco, tamBloco, 1, arquivo);
		free(bloco);
		fclose(arquivo);
		return 1;
	}
}

void AtualizaHeader(FILE* arquivo, int nregistros, int nblocos){
	blocoinicial* primeirobloco = (blocoinicial*)malloc(sizeof(blocoinicial));
	fseek(arquivo, 0, SEEK_SET);
	fread(primeirobloco, tamBloco, 1, arquivo);
	primeirobloco->nblocos += nblocos;
	primeirobloco->nregistros += nregistros;
	fseek(arquivo, 0, SEEK_SET);
	fwrite(primeirobloco, tamBloco, 1, arquivo);
	free(primeirobloco);
}

int insereReg(reg newreg, FILE* arquivo){
	fseek(arquivo, 0, SEEK_SET);
	int blocon = 0; //para avançar entre os blocos
	int regn = 0;
	bloco* temp = criaBloco(); //bloco temporario para escrevermos o registro
	blocoinicial* tempinicial = criaBlocoInicial();

	if(!arquivo){
		printf("Arquivo nao encontrado!\n");
		return 0;
	}else{
		printf("Arquivo encontrado\n");
		fread(tempinicial, tamBloco,1,arquivo);
		if((tempinicial->header[0]=='#')&&(tempinicial->header[1]=='B')&&(tempinicial->header[2]=='L')&&(tempinicial->header[3]=='K')){
			printf("Bloco validado.\n");
				while(regn <= 6){
					printf("Procurando registro vazio.\n");
					if(tempinicial->index[regn].code == 0 || tempinicial->index[regn].code == -1){ // zero para vazio | -1 para registro removido
						printf("Escrevendo dados.\n");
						tempinicial->index[regn] = newreg;
						fseek(arquivo, blocon*tamBloco, SEEK_SET);
              			fwrite(tempinicial, tamBloco, 1, arquivo);
              			AtualizaHeader(arquivo, 1, 0);
              			free(temp);
              			free(tempinicial);
						return 1;
					}else{
						printf("Procurando prox registro.\n");
						regn++;
					}
				}
				blocon++;
				regn = 0;
		}
		while ((fread(temp,tamBloco,1,arquivo)) != 0){
			if((temp->header[0]=='#')&&(temp->header[1]=='B')&&(temp->header[2]=='L')&&(temp->header[3]=='K')){
				printf("Bloco validado.\n");
				while(regn <= 6){
					printf("Procurando registro vazio.\n");
					if(temp->index[regn].code == 0 || temp->index[regn].code == -1){ // zero para vazio | -1 para registro removido
						printf("Escrevendo dados.\n");
						temp->index[regn] = newreg;
						fseek(arquivo, blocon*tamBloco, SEEK_SET);
              			fwrite(temp, tamBloco, 1, arquivo);
              			AtualizaHeader(arquivo, 1, 0);
              			free(temp);
              			free(tempinicial);
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
        AtualizaHeader(arquivo, 1, 1);
        free(temp);
        free(tempinicial);
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
*/

void escreveReg(reg regout){		//escreve o conteudo de um registro, NÃO TESTEI
	printf("\nConteudo do registro:\n"
		   "Codigo: %d\n"
		   "Descrição: %s\n"
		   "Ano: %d\n"
		   "Valor: %f\n\n", regout.code, regout.desc, regout.ano, regout.valor);
}


int removeReg(int rindex){
	int blocon = 0; //para avançar entre os blocos
	int regn = 0;
	FILE* arquivo = fopen("arquivo.txt", "rb+");
	bloco* temp = criaBloco(); //bloco temporario para escrevermos o registro
	blocoinicial* tempinicial = criaBlocoInicial();
	if(!arquivo){
		printf("Arquivo nao encontrado!\n");
		return 0;
	}else{
		printf("Arquivo encontrado\n");
		fread(tempinicial, tamBloco,1,arquivo);
		if((tempinicial->header[0]=='#')&&(tempinicial->header[1]=='B')&&(tempinicial->header[2]=='L')&&(tempinicial->header[3]=='K')){
			printf("Bloco validado.\n");
			while(regn <= 6){
				printf("Procurando o registro a ser removido.\n");
				if(tempinicial->index[regn].code == rindex){
					printf("Removendo o registro.\n");
					tempinicial->index[regn].code = -1;
					fseek(arquivo, blocon*tamBloco, SEEK_SET);
              		fwrite(tempinicial, tamBloco, 1, arquivo);
              		AtualizaHeader(arquivo, -1, 0);
              		free(tempinicial);
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
		}
		while ((fread(temp,tamBloco,1,arquivo)) != 0){
			if((temp->header[0]=='#')&&(temp->header[1]=='B')&&(temp->header[2]=='L')&&(temp->header[3]=='K')){
				printf("Bloco validado.\n");
				while(regn <= 6){
					printf("Procurando o registro a ser removido.\n");
					if(temp->index[regn].code == rindex){
						printf("Removendo o registro.\n");
						temp->index[regn].code = -1;
						fseek(arquivo, blocon*tamBloco, SEEK_SET);
              			fwrite(temp, tamBloco, 1, arquivo);
              			AtualizaHeader(arquivo, -1, 0);
              			free(tempinicial);
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
		free(tempinicial);
		free(temp);
        fclose(arquivo);
		return 0;
	}
}

int procuraReg(int key){
	FILE* arquivo = fopen("arquivo.txt", "rb+");
	bloco* temp = criaBloco();
	blocoinicial* tempinicial = criaBlocoInicial();
	int blocon = 0; //para avançar entre os blocos
	int regn = 0;

	if(!arquivo){
		printf("Arquivo nao encontrado!\n");
		return 0;
	}else{
		printf("Arquivo encontrado\n");
		fread(tempinicial, tamBloco,1,arquivo);
		if((tempinicial->header[0]=='#')&&(tempinicial->header[1]=='B')&&(tempinicial->header[2]=='L')&&(tempinicial->header[3]=='K')){
			printf("Bloco validado.\n");
			while(regn <= 6){
				printf("Procurando o registro.\n");
				if(tempinicial->index[regn].code == key){
					escreveReg(tempinicial->index[regn]);
              		fclose(arquivo);
              		free(tempinicial);
              		free(temp);
              		fclose(arquivo);
					return 1;
				}else{
					regn++;
				}
			}
			blocon++;
			regn = 0;
		}
		while ((fread(temp,tamBloco,1,arquivo)) != 0){
			if((temp->header[0]=='#')&&(temp->header[1]=='B')&&(temp->header[2]=='L')&&(temp->header[3]=='K')){
				printf("Bloco validado.\n");
				while(regn <= 6){
					printf("Procurando o registro.\n");
					if(temp->index[regn].code == key){
						escreveReg(temp->index[regn]);
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

int listaReg(){
	FILE* arquivo = fopen("arquivo.txt", "rb+");
	bloco* temp = criaBloco();
	blocoinicial* tempinicial = criaBlocoInicial();
	int blocon = 0; //para avançar entre os blocos
	int regn = 0;

	if(!arquivo){
		printf("Arquivo nao encontrado!\n");
		return 0;
	}else{
		printf("Arquivo encontrado\n");
		fread(tempinicial, tamBloco,1,arquivo);
		if((tempinicial->header[0]=='#')&&(tempinicial->header[1]=='B')&&(tempinicial->header[2]=='L')&&(tempinicial->header[3]=='K')){
			printf("Bloco validado.\n");
			while(regn <= 6){
				printf("Procurando o registro.\n");
				if(tempinicial->index[regn].code <= 0 ){
					regn++;
				}else{
					escreveReg(tempinicial->index[regn]);
					regn++;
				}
			}
			blocon++;
			regn = 0;
		}
		while ((fread(temp,tamBloco,1,arquivo)) != 0){
			if((temp->header[0]=='#')&&(temp->header[1]=='B')&&(temp->header[2]=='L')&&(temp->header[3]=='K')){
				printf("Bloco validado.\n");
				while(regn <= 6){
					printf("Procurando o registro.\n");
					if(temp->index[regn].code <= 0 ){
						regn++;
					}else{
						escreveReg(temp->index[regn]);
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
		free(tempinicial);
		free(temp);
        fclose(arquivo);
        return 1;
	}
}

void compactaArquivo(){
	FILE* arquivo = fopen("arquivo.txt", "rb+");
	criaTempArquivo();
	FILE* temparquivo = fopen("temparquivo.txt", "rb+");
	bloco* temp = criaBloco();
	blocoinicial* tempinicial = criaBlocoInicial();
	int blocon = 0;
	int regn = 0;

	if(!arquivo){
		printf("Arquivo nao encontrado!\n");
	}else{
		printf("Arquivo encontrado\n");
		fread(tempinicial, tamBloco,1,arquivo);
		if((tempinicial->header[0]=='#')&&(tempinicial->header[1]=='B')&&(tempinicial->header[2]=='L')&&(tempinicial->header[3]=='K')){
			printf("Bloco validado.\n");
			while(regn <= 6){
				printf("Procurando o registro.\n");
				if(tempinicial->index[regn].code <= 0 ){
					printf("Excluido.\n");
					regn++;
				}else{
					insereReg(tempinicial->index[regn], temparquivo);
					regn++;
				}
			}
			blocon++;
			regn = 0;
		}
		while ((fread(temp,tamBloco,1,arquivo)) != 0){
			if((temp->header[0]=='#')&&(temp->header[1]=='B')&&(temp->header[2]=='L')&&(temp->header[3]=='K')){
				printf("Bloco validado.\n");
				while(regn <= 6){
					printf("Procurando o registro.\n");
					if(temp->index[regn].code <= 0 ){
						regn++;
					}else{
						insereReg(temp->index[regn], temparquivo);
						regn++;
					}
				}
				blocon++;
				regn = 0;
			}else{
				printf("Inconsistencia de dados detectada, o arquivo foi corrompido.\n");
			}
		}
		remove("arquivo.txt");
		rename("temp.txt","arquivo.txt");
		free(tempinicial);
		free(temp);
    	fclose(arquivo);
    	fclose(temparquivo);
	}
}