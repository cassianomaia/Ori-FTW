
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define tamReg 62
#define tamBloco 512
#define tamCod 4
#define tamDesc 50
#define tamAno 4
#define tamValor 4
#define tamIndex 4


// struct que define registros de 62 bytes para um arquivo de catalogo de violinos
typedef struct {
	int code;
	char desc[50];
	int ano;
	int valor;		// 4 + 4 + 50 + 4 = 62 	
} reg;


// struct que define os blocos do programa, com tamanho fixo de 512 bytes
typedef struct {
	char header[4];	//CATITO TA SOBRANDO 12 BYTES
	reg index[8];	//quantidade de registros de 62 bytes que cabem no bloco
} bloco; 

//operação do bloco
bloco* criaBloco();
//operações do arquivo
int criaArquivo();
// operações dos registros
int insereReg(reg);			//parametros: registro a ser inserido
//int removeReg(int*, char*);			//codigo (key) e filepath
//void leReg(reg*);
//void escreveReg(reg);
//int procuraReg(reg*, int*, char*);	//reg, key e filepath


//bool compactaArquivo();
