
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define tamCod 4
#define tamDesc 50
#define tamAno 4
#define tamValor 4
#define tamReg 64
#define tamBloco 512
#define tamHeader 4


// struct que define registros de 62 bytes para um arquivo de catalogo de violinos // 64bytes size
typedef struct {
	int code;
	char desc[50];
	int ano;
	float valor;		// 4 + 4 + 50 + 4 = 62 	
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
int removeReg(int);			//codigo
//void leReg(reg*);
//void escreveReg(reg);
int procuraReg();	//reg, key e filepath
int listaReg(char*); 	//filepath


//bool compactaArquivo();
