#include "file.h"

bloco* criaBloco(){
	bloco* novo = (bloco*)malloc(sizeof(bloco));
	memset(novo,0,tamBloco);
	novo->header = "11";
	return novo;
}

/*
int insereReg(reg, char*)
int removeReg(int*, char*)
void leReg(reg*)
void escreveReg(reg)
int procuraReg(reg*, int*, char*)
*/

bool criaArquivo(){
	FILE* arquivo = fopen(arquivo.txt, wb);
	if(!arquivo){
		return false;
	}else{
		bloco* bloco= criaBloco();
		fwrite(bloco, tamBloco, 1, arquivo);
		free bloco;
		fclose(arquivo);
	}
}
//bool compactaArquivo(char*)
