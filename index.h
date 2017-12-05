
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define TAM_INDEXFIELD 12
#define tamBloco 512

typedef struct {
	int code;
	int bloco;
	int reg;
} indexfield;

typedef struct {
	char header[4];	
	indexfield indexfield[42];	//Quantidade de registros de 62 bytes que cabem no bloco
} bloco_i; 

typedef struct {
	int nblocos;
	int nindex;
	char header[4];	
	indexfield indexfield[41];
} blocoinicial_i;


bloco_i* criaBloco_i();
blocoinicial_i* criaBlocoInicial_i();

int criaArquivo_i();
int criaTempArquivo_i();
void AtualizaHeader_i(FILE*, int, int);
void compactaArquivo_i();

int insereIndex(indexfield, FILE*);	