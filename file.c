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
		free(arquivo);
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
	indexcontrol index_out;
	criaTempArquivo_i();
	FILE* temp_index = fopen("tempindex.txt", "rb+");
	FILE* index = fopen("index.txt", "rb+");

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
					index_out = insereReg(tempinicial->index[regn], temparquivo);
					insereIndex(index_out, temp_index);
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
						insereIndex(index_out, temp_index);
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
		remove("index.txt");
		rename("tempindex.txt","index.txt");
		free(tempinicial);
		free(temp);
    	fclose(arquivo);
    	fclose(temparquivo);
    	fclose(temp_index);
    	fclose(index);
	}
}

//Função que recebe um registro e o arquivo em que o registro deve ser inserido
indexfield insereReg(reg newreg, FILE* arquivo){
	//FILE* arqindex = fopen("index.txt", "rb+");
	fseek(arquivo, 0, SEEK_SET);
	int blocon = 0; //para avançar entre os blocos
	int regn = 0;
	bloco* temp = criaBloco(); //bloco temporario para escrevermos o registro
	blocoinicial* tempinicial = criaBlocoInicial();
	indexfield newindex;

	//indexfield newindex;
	newindex.code = newreg.code;

	if(!arquivo){
		printf("Arquivo nao encontrado!\n");
		return newindex;
	}else{
		//Leitura do bloco incial
		fread(tempinicial, tamBloco,1,arquivo);
		if((tempinicial->header[0]=='#')&&(tempinicial->header[1]=='B')&&(tempinicial->header[2]=='L')&&(tempinicial->header[3]=='K')){
				while(regn <= 6){
					//Verificação de espaço vazio
					if(tempinicial->index[regn].code == 0 || tempinicial->index[regn].code == -1){ // zero para vazio | -1 para registro removido
						//newindex.bloco = blocon;
						//newindex.reg = regn;
						//insereIndex(newindex, arqindex);
						newindex.bloco = blocon;
						newindex.reg = regn;
						tempinicial->index[regn] = newreg;
						fseek(arquivo, blocon*tamBloco, SEEK_SET);
              			fwrite(tempinicial, tamBloco, 1, arquivo);
              			AtualizaHeader(arquivo, 1, 0);
              			free(temp);
              			free(tempinicial);
						return newindex;
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
						//newindex.bloco = blocon;
						//newindex.reg = regn;
						//insereIndex(newindex, arqindex);
						newindex.bloco = blocon;
						newindex.reg = regn;
						temp->index[regn] = newreg;
						fseek(arquivo, blocon*tamBloco, SEEK_SET);
              			fwrite(temp, tamBloco, 1, arquivo);
              			AtualizaHeader(arquivo, 1, 0);
              			free(temp);
              			free(tempinicial);
						return newindex;
					}else{
						regn++;
					}
				}
				blocon++;
				regn = 0;
			}else{
				printf("Inconsistencia de dados detectada, o arquivo foi corrompido.\n");
				return newindex;
			}
		}
		//Criação de bloco extra caso não haja espaço em nenhum bloco
		printf("Todos os blocos estão cheios.\n");
		temp = criaBloco();
		//newindex.bloco = blocon;
		//newindex.reg = regn;
		//insereIndex(newindex, arqindex);
		newindex.bloco = blocon;
		newindex.reg = 0;
		temp->index[0] = newreg;
		fseek(arquivo, blocon*tamBloco, SEEK_SET);
        fwrite(temp, tamBloco, 1, arquivo);
        AtualizaHeader(arquivo, 1, 1);
        free(temp);
        free(tempinicial);
		return newindex;
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


bloco_i* criaBloco_i(){
	bloco_i* new = (bloco_i*)malloc(sizeof(bloco_i));
	memset(new,0,tamBloco);
	strncpy(new->header, "#BLK", 4); //Todos os blocos recebem esse header, para confirmação de consistencia na criação
	return new;
}

//Criação do bloco inicial, que tem caracteristica diferente dos demais blocos
blocoinicial_i* criaBlocoInicial_i(){
	blocoinicial_i* new = (blocoinicial_i*)malloc(sizeof(blocoinicial_i));
	memset(new,0,tamBloco);
	strncpy(new->header, "#BLK", 4); //Todos os blocos recebem esse header, para confirmação de consistencia na criação
	new->nblocos = 1; //Como criaBlocoInicial só é chamada na criação do arquivo, temos total certeza de que teremos somente 1 bloco e 0 registros
	new->nindex = 0;
	return new;
}

int criaArquivo_i(){
	FILE* arquivo_index = fopen("index.txt", "wb");
	if(!arquivo_index){
		return 0;
	}else{
		blocoinicial_i* blocoinicial = criaBlocoInicial_i(); //Criação do bloco inicial do arquivo
		fwrite(blocoinicial, tamBloco, 1, arquivo_index);
		free(blocoinicial);
		fclose(arquivo_index);
		return 1;
	}
}

int criaTempArquivo_i(){
	FILE* arquivo = fopen("tempindex.txt", "wb");
	if(!arquivo){
		return 0;
	}else{
		blocoinicial_i* bloco= criaBlocoInicial_i();
		fwrite(bloco, tamBloco, 1, arquivo);
		free(bloco);
		fclose(arquivo);
		return 1;
	}
}

void AtualizaHeader_i(FILE* arquivo, int nindex, int nblocos){
	blocoinicial_i* primeirobloco = (blocoinicial_i*)malloc(sizeof(blocoinicial_i));
	fseek(arquivo, 0, SEEK_SET);
	fread(primeirobloco, tamBloco, 1, arquivo);
	primeirobloco->nblocos += nblocos;
	primeirobloco->nindex += nindex;
	fseek(arquivo, 0, SEEK_SET);
	fwrite(primeirobloco, tamBloco, 1, arquivo);
	free(primeirobloco);
}

int insereIndex(indexfield newindex, FILE* arquivo){
	fseek(arquivo, 0, SEEK_SET);
	int blocon = 0; //para avançar entre os blocos
	int indexn = 0;
	bloco_i* temp = criaBloco_i(); //bloco temporario para escrevermos o registro
	blocoinicial_i* tempinicial = criaBlocoInicial_i();

	if(!arquivo){
		printf("Arquivo nao encontrado!\n");
		return 0;
	}else{
		//Leitura do bloco incial
		fread(tempinicial, tamBloco,1,arquivo);
		if((tempinicial->header[0]=='#')&&(tempinicial->header[1]=='B')&&(tempinicial->header[2]=='L')&&(tempinicial->header[3]=='K')){
				while(indexn <= 41){
					//Verificação de espaço vazio
					if(tempinicial->index[indexn].code == 0 || tempinicial->index[indexn].code == -1){ // zero para vazio | -1 para registro removido
						tempinicial->index[indexn] = newindex;
						fseek(arquivo, blocon*tamBloco, SEEK_SET);
              			fwrite(tempinicial, tamBloco, 1, arquivo);
              			AtualizaHeader_i(arquivo, 1, 0);
              			free(temp);
              			free(tempinicial);
						return 1;
					}else{
						indexn++;
					}
				}
				blocon++;
				indexn = 0;
		}
		//Leitura do resto dos blocos do arquivo
		while ((fread(temp,tamBloco,1,arquivo)) != 0){
			if((temp->header[0]=='#')&&(temp->header[1]=='B')&&(temp->header[2]=='L')&&(temp->header[3]=='K')){
				while(indexn <= 40){
					//Verificação de espaço vazio
					if(temp->index[indexn].code == 0 || temp->index[indexn].code == -1){ // zero para vazio | -1 para registro removido
						tempinicial->index[indexn] = newindex;
						fseek(arquivo, blocon*tamBloco, SEEK_SET);
              			fwrite(temp, tamBloco, 1, arquivo);
              			AtualizaHeader_i(arquivo, 1, 0);
              			free(temp);
              			free(tempinicial);
						return 1;
					}else{
						indexn++;
					}
				}
				blocon++;
				indexn = 0;
			}else{
				printf("Inconsistencia de dados detectada, o arquivo foi corrompido.\n");
				return 0;
			}
		}
		//Criação de bloco extra caso não haja espaço em nenhum bloco
		printf("Todos os blocos estão cheios.\n");
		temp = criaBloco_i();
		temp->index[0] = newindex;
		fseek(arquivo, blocon*tamBloco, SEEK_SET);
        fwrite(temp, tamBloco, 1, arquivo);
        AtualizaHeader_i(arquivo, 1, 1);
        free(temp);
        free(tempinicial);
		return 1;
	}
}

int removeIndex(int rindex){
	int blocon = 0; //para avançar entre os blocos
	int indexn = 0;
	FILE* arquivo = fopen("index.txt", "rb+");
	bloco_i* temp = criaBloco_i(); //bloco temporario para escrevermos o registro
	blocoinicial_i* tempinicial = criaBlocoInicial_i();
	if(!arquivo){
		printf("Arquivo nao encontrado!\n");
		return 0;
	}else{
		printf("Arquivo encontrado\n");
		//Leitura do bloco inicial
		fread(tempinicial, tamBloco,1,arquivo);
		if((tempinicial->header[0]=='#')&&(tempinicial->header[1]=='B')&&(tempinicial->header[2]=='L')&&(tempinicial->header[3]=='K')){
			while(indexn <= 40){
				//Checagem da chave
				if(tempinicial->index[indexn].code == rindex){
					printf("Removendo o registro.\n");
					tempinicial->index[indexn].code = -1;
					fseek(arquivo, blocon*tamBloco, SEEK_SET);
              		fwrite(tempinicial, tamBloco, 1, arquivo);
              		AtualizaHeader(arquivo, -1, 0);
              		free(tempinicial);
              		free(temp);
              		fclose(arquivo);
					return 1;
				}else{
					indexn++;
				}
			}
			blocon++;
			indexn = 0;
		}
		//Leitura dos demais blocos
		while ((fread(temp,tamBloco,1,arquivo)) != 0){
			if((temp->header[0]=='#')&&(temp->header[1]=='B')&&(temp->header[2]=='L')&&(temp->header[3]=='K')){
				while(indexn <= 41){
					//Verificação do codigo do registro
					if(temp->index[indexn].code == rindex){
						printf("Removendo o registro.\n");
						temp->index[indexn].code = -1;
						fseek(arquivo, blocon*tamBloco, SEEK_SET);
              			fwrite(temp, tamBloco, 1, arquivo);
              			AtualizaHeader(arquivo, -1, 0);
              			free(tempinicial);
              			free(temp);
              			fclose(arquivo);
						return 1;
					}else{
						indexn++;
					}
				}
				blocon++;
				indexn = 0;
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

int procuraIndex(int key){
	FILE* index = fopen("index.txt", "rb+");
	FILE* arquivo = fopen("arquivo.txt", "rb+");
	bloco_i* temp = criaBloco_i(); //bloco temporario para escrevermos o registro
	blocoinicial_i* tempinicial = criaBlocoInicial_i();
	bloco* read = criaBloco();
	blocoinicial* readinicial = criaBlocoInicial();
	indexfield search;
	search.code = 0;
	search.bloco = 0;
	search.reg = 0;
	int blocon = 0;
	int indexn = 0;

	if(!index){
		printf("Arquivo nao encontrado!\n");
		return 0;
	}else{
		printf("Arquivo encontrado\n");
		//Leitura do bloco inicial
		fread(tempinicial, tamBloco,1,index);
		if((tempinicial->header[0]=='#')&&(tempinicial->header[1]=='B')&&(tempinicial->header[2]=='L')&&(tempinicial->header[3]=='K')){
			while(indexn <= 40){
				//Checagem da chave
				if(tempinicial->index[indexn].code == key){
					search = tempinicial->index[indexn];
					indexn = 999;
					printf("%d %d %d \n", search.code, search.bloco, search.reg);
				}else{
					indexn++;
				}
			}
			blocon++;
			indexn = 0;
		}
		//Leitura dos demais blocos
		if(search.code == 0){
			while ((fread(temp,tamBloco,1,index)) != 0){
				if((temp->header[0]=='#')&&(temp->header[1]=='B')&&(temp->header[2]=='L')&&(temp->header[3]=='K')){
					while(indexn <= 41){
						//Verificação do codigo do registro
						if(temp->index[indexn].code == key){
							search = temp->index[indexn];
							indexn = 999;
						}else{
							indexn++;
						}
					}
					blocon++;
					indexn = 0;
				}else{
					printf("Inconsistencia de dados detectada, o arquivo foi corrompido.\n");
					return 0;
				}
			}
		}
		if(search.code == 0){
			printf("Registro nao encontrado.\n");
			free(tempinicial);
			free(temp);
        	free(readinicial);
        	free(read);
        	fclose(arquivo);
        	fclose(index);
        	return 0;
    	}else{
    		if(search.bloco == 0){
    			fseek(arquivo,(tamBloco*search.bloco),SEEK_SET);
    			fread(readinicial, tamBloco, 1, arquivo);
    			escreveReg(readinicial->index[search.reg]);
    		}else{
    			fseek(arquivo,(tamBloco*search.bloco),SEEK_SET);
    			fread(read, tamBloco, 1, arquivo);
    			escreveReg(read->index[search.reg]);
    		}
    		return 1;
    	}
	}
}

void read_csv() {
	reg reg_out;
	indexfield index_out;
	int i;
    FILE *inFile = fopen("MOCK_DATA.csv", "r");
    FILE* index = fopen("index.txt", "rb+");
    FILE* arquivo = fopen("arquivo.txt", "rb+");
    for(i = 0; i<500; i++){
    	fscanf(inFile, "%d,%[^,],%d,%f", &reg_out.code, &reg_out.desc, &reg_out.ano, &reg_out.valor);
    	index_out = insereReg(reg_out, arquivo);
    	printf("%d %d %d\n", index_out.code, index_out.bloco, index_out.reg);
    	insereIndex(index_out, index);
	}
	fclose(inFile);
	fclose(arquivo);
	fclose(index);
}