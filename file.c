#include "file.h"

bloco* criaBloco(){
	bloco* novo = (bloco*)malloc(sizeof(bloco));
	memset(novo,0,tamBloco);
	strncpy(novo->header, "--", 2);
	return novo;
}

/*
int insereReg(reg, char*)
int removeReg(int*, char*)
void leReg(reg*)
void escreveReg(reg)
int procuraReg(reg*, int*, char*)
*/

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
//bool compactaArquivo(char*)
