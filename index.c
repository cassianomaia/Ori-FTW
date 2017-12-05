#include "index.h"

bloco_i* criaBloco_i(){
	bloco_i* novo = (bloco_i*)malloc(sizeof(bloco_i));
	memset(novo,0,tamBloco);
	strncpy(novo->header, "#BLK", 4); //Todos os blocos recebem esse header, para confirmação de consistencia na criação
	return novo;
}

//Criação do bloco inicial, que tem caracteristica diferente dos demais blocos
blocoinicial_i* criaBlocoInicial_i(){
	blocoinicial_i* novo = (blocoinicial_i*)malloc(sizeof(blocoinicial_i));
	memset(novo,0,tamBloco);
	strncpy(novo->header, "#BLK", 4); //Todos os blocos recebem esse header, para confirmação de consistencia na criação
	novo->nblocos = 1; //Como criaBlocoInicial só é chamada na criação do arquivo, temos total certeza de que teremos somente 1 bloco e 0 registros
	novo->nindex = 0;
	return novo;
}

int criaArquivo_i(){
	FILE* arquivo = fopen("index.txt", "wb");
	if(!arquivo){
		return 0;
	}else{
		blocoinicial_i* bloco = criaBlocoInicial_i(); //Criação do bloco inicial do arquivo
		fwrite(bloco, tamBloco, 1, arquivo);
		free(bloco);
		fclose(arquivo);
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

void compactaArquivo_i(){
	FILE* arquivo = fopen("index.txt", "rb+");
	criaTempArquivo_i();
	FILE* temparquivo = fopen("tempindex.txt", "rb+");
	bloco_i* temp = criaBloco_i();
	blocoinicial_i* tempinicial = criaBlocoInicial_i();
	int blocon = 0;
	int indexn = 0;

	if(!arquivo){
		printf("Arquivo nao encontrado!\n");
	}else{
		printf("Arquivo encontrado\n");
		//Leitura do bloco inicial
		fread(tempinicial, tamBloco,1,arquivo);
		if((tempinicial->header[0]=='#')&&(tempinicial->header[1]=='B')&&(tempinicial->header[2]=='L')&&(tempinicial->header[3]=='K')){
			while(indexn <= 41){
				if(tempinicial->indexfield[indexn].code <= 0 ){
					indexn++;
				}else{
					//Passagem dos registros validos do bloco original para o novo arquivo
					insereIndex(tempinicial->indexfield[indexn], temparquivo);
					indexn++;
				}
			}
			blocon++;
			indexn = 0;
		}
		//Leitura dos demais blocos
		while ((fread(temp,tamBloco,1,arquivo)) != 0){
			if((temp->header[0]=='#')&&(temp->header[1]=='B')&&(temp->header[2]=='L')&&(temp->header[3]=='K')){
				while(indexn <= 6){
					if(temp->indexfield[indexn].code <= 0 ){
						indexn++;
					}else{
						insereIndex(temp->indexfield[indexn], temparquivo);
						indexn++;
					}
				}
				blocon++;
				indexn = 0;
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
					if(tempinicial->indexfield[indexn].code == 0 || tempinicial->indexfield[indexn].code == -1){ // zero para vazio | -1 para registro removido
						tempinicial->indexfield[indexn] = newindex;
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
				while(indexn <= 41){
					//Verificação de espaço vazio
					if(temp->indexfield[indexn].code == 0 || temp->indexfield[indexn].code == -1){ // zero para vazio | -1 para registro removido
						tempinicial->indexfield[indexn] = newindex;
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
		temp->indexfield[0] = newindex;
		fseek(arquivo, blocon*tamBloco, SEEK_SET);
        fwrite(temp, tamBloco, 1, arquivo);
        AtualizaHeader_i(arquivo, 1, 1);
        free(temp);
        free(tempinicial);
		return 1;
	}
}