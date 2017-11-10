#include "file.h"

bloco* criaBloco(){
	bloco* novo = (bloco*)malloc(sizeof(bloco));
	memset(novo,0,tamBloco);
	strncpy(novo->header, "#BLK", 4); //Todos os blocos recebem esse header, para confirmação de consistencia na criação
	return novo;
}

//Criação do bloco inicial, que tem caracteristica diferente dos demais blocos
blocoinicial* criaBlocoInicial(){
	blocoinicial* novo = (blocoinicial*)malloc(sizeof(blocoinicial));
	memset(novo,0,tamBloco);
	strncpy(novo->header, "#BLK", 4); //Todos os blocos recebem esse header, para confirmação de consistencia na criação
	novo->nblocos = 1; //Como criaBlocoInicial só é chamada na criação do arquivo, temos total certeza de que teremos somente 1 bloco e 0 registros
	novo->nregistros = 0;
	return novo;
}

//Função de criação do arquivo
int criaArquivo(){
	FILE* arquivo = fopen("arquivo.txt", "wb");
	if(!arquivo){
		return 0;
	}else{
		blocoinicial* bloco= criaBlocoInicial(); //Criação do bloco inicial do arquivo
		fwrite(bloco, tamBloco, 1, arquivo);
		free(bloco);
		fclose(arquivo);
		return 1;
	}
}

//Função para criar o arquivo utilizado na compactação
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

//Função que atualiza o header do arquivo
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
		//Leitura do bloco inicial
		fread(tempinicial, tamBloco,1,arquivo);
		if((tempinicial->header[0]=='#')&&(tempinicial->header[1]=='B')&&(tempinicial->header[2]=='L')&&(tempinicial->header[3]=='K')){
			while(regn <= 6){
				if(tempinicial->index[regn].code <= 0 ){
					regn++;
				}else{
					//Passagem dos registros validos do bloco original para o novo arquivo
					insereReg(tempinicial->index[regn], temparquivo);
					regn++;
				}
			}
			blocon++;
			regn = 0;
		}
		//Leitura dos demais blocos
		while ((fread(temp,tamBloco,1,arquivo)) != 0){
			if((temp->header[0]=='#')&&(temp->header[1]=='B')&&(temp->header[2]=='L')&&(temp->header[3]=='K')){
				while(regn <= 6){
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
		//Substituição do arquivo original pelo novo arquivo gerado sem fragmentações
		remove("arquivo.txt");
		rename("temparquivo.txt","arquivo.txt");
		free(tempinicial);
		free(temp);
    	fclose(arquivo);
    	fclose(temparquivo);
	}
}

//Função que recebe um registro e o arquivo em que o registro deve ser inserido
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
		//Leitura do bloco incial
		fread(tempinicial, tamBloco,1,arquivo);
		if((tempinicial->header[0]=='#')&&(tempinicial->header[1]=='B')&&(tempinicial->header[2]=='L')&&(tempinicial->header[3]=='K')){
				while(regn <= 6){
					//Verificação de espaço vazio
					if(tempinicial->index[regn].code == 0 || tempinicial->index[regn].code == -1){ // zero para vazio | -1 para registro removido
						tempinicial->index[regn] = newreg;
						fseek(arquivo, blocon*tamBloco, SEEK_SET);
              			fwrite(tempinicial, tamBloco, 1, arquivo);
              			AtualizaHeader(arquivo, 1, 0);
              			free(temp);
              			free(tempinicial);
						return 1;
					}else{
						regn++;
					}
				}
				blocon++;
				regn = 0;
		}
		//Leitura do resto dos blocos do arquivo
		while ((fread(temp,tamBloco,1,arquivo)) != 0){
			if((temp->header[0]=='#')&&(temp->header[1]=='B')&&(temp->header[2]=='L')&&(temp->header[3]=='K')){
				while(regn <= 6){
					//Verificação de espaço vazio
					if(temp->index[regn].code == 0 || temp->index[regn].code == -1){ // zero para vazio | -1 para registro removido
						temp->index[regn] = newreg;
						fseek(arquivo, blocon*tamBloco, SEEK_SET);
              			fwrite(temp, tamBloco, 1, arquivo);
              			AtualizaHeader(arquivo, 1, 0);
              			free(temp);
              			free(tempinicial);
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
		//Criação de bloco extra caso não haja espaço em nenhum bloco
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

//Printa o conteudo de um registro
void escreveReg(reg regout){		
	printf("\nConteudo do registro:\n"
		   "Codigo: %d\n"
		   "Descrição: %s\n"
		   "Ano: %d\n"
		   "Valor: %f\n", regout.code, regout.desc, regout.ano, regout.valor);
}

//Remove um registro, dada a sua chave
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
		//Leitura do bloco inicial
		fread(tempinicial, tamBloco,1,arquivo);
		if((tempinicial->header[0]=='#')&&(tempinicial->header[1]=='B')&&(tempinicial->header[2]=='L')&&(tempinicial->header[3]=='K')){
			while(regn <= 6){
				//Checagem da chave
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
					regn++;
				}
			}
			blocon++;
			regn = 0;
		}
		//Leitura dos demais blocos
		while ((fread(temp,tamBloco,1,arquivo)) != 0){
			if((temp->header[0]=='#')&&(temp->header[1]=='B')&&(temp->header[2]=='L')&&(temp->header[3]=='K')){
				while(regn <= 6){
					//Verificação do codigo do registro
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
			while(regn <= 6){
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
				while(regn <= 6){
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

//Lista todos os registros validos no arquivo
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
			while(regn <= 6){
				//Checa se não é um registro valido, e o printa se for valido
				if(tempinicial->index[regn].code <= 0 ){ // qualquer registro de código 0 é um espaço vazio e -1  é um registro deletado logicamente
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
				while(regn <= 6){
					//Checa se não é um registro valido, e o printa se for valido
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

//Função que gera um registro aleatório para inserção em lotes
reg registroaleatorio(){
	reg regin;
	regin.code = (rand()%100) + 1;
	regin.ano = rand()%410 + 1600; // Recebe um ano acima de 1600, ano em que o primeiro violino foi datado
	regin.valor = rand()%10000 + 320; //Recebe um valor acima de 320, que é o valor de um violino estudante
	switch (rand()%10 + 1){
		case 1:
			strcpy(regin.desc, "Antonius Stradivarius Cremonenfis, Faciebat");
		break;
		case 2:
			strcpy(regin.desc, "Jean Baptiste Vuillaume a Paris, Rue Croix des Pet");
		break;
		case 3:
			strcpy(regin.desc, "Copy of Antonius Stradivarius, made in Czech Rep.");
		break;
		case 4:
			strcpy(regin.desc, "Francesco Ruggieri detto il per Cremona");
		break;
		case 5:
			strcpy(regin.desc, "G. Carlettinius fec. Centum");
		break;
		case 6:
			strcpy(regin.desc, "Enrico Orselli, Liutaio, Pesaro");
		break;
		case 7:
			strcpy(regin.desc, "T. J. Holder, Luthier, Paris, Model 4/4");
		break;
		case 8:
			strcpy(regin.desc, "Johann Glass, Getgenmacher in Lelpzig");
		break;
		case 9:
			strcpy(regin.desc, "Giovanni Leoni Filius, Parmo n 1432, 4/4");
		break;
		case 10:
			strcpy(regin.desc, "Luigi Gambelimberti, fece a Seveso l'anno 1920");
		break;
		case 11:
			strcpy(regin.desc, "Georges Defat, Luthier a Paris, anno 1900");
		break;
	}
	return regin;
}