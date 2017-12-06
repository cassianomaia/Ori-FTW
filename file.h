
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Definição dos tamanhos fixos de conteudos, registro, blocos e header
#define tamCod 4
#define tamDesc 50
#define tamAno 4
#define tamValor 4
#define tamReg 64
#define tamBloco 512
#define tamHeader 4
#define tamIndexField 12

// Struct que define registros de 62 bytes para um arquivo de catalogo de violinos
typedef struct {
	int code;
	char desc[50];
	int ano;
	float valor;
} reg;

// Struct que define os blocos do programa, com tamanho fixo de 512 bytes
typedef struct {
	char header[4];	
	reg index[8];	//Quantidade de registros de 62 bytes que cabem no bloco
} bloco; 

//Bloco inicial, com header especial para conter numero de blocos e registros contidos no arquivo
typedef struct {
	int nblocos;
	int nregistros;
	char header[4];	
	reg index[8];
} blocoinicial;

typedef struct {
	int code;
	int bloco;
	int reg;
} indexfield;

typedef struct {
	char header[4];	
	indexfield index[42];	//Quantidade de registros de 62 bytes que cabem no bloco
} bloco_i; 

typedef struct {
	int nblocos;
	int nindex;
	char header[4];	
	indexfield index[42];
} blocoinicial_i;



//Operações do bloco
bloco* criaBloco();
blocoinicial* criaBlocoInicial();

//Operações do arquivo
int criaArquivo();
int criaTempArquivo();
void AtualizaHeader(FILE*, int, int);
void compactaArquivo();


// Operações dos registros
indexfield insereReg(reg, FILE*);			
void escreveReg(reg);
int removeReg(int);	
int procuraReg();	
int listaReg();

//Operação que gera registros aleatórios para inserção em lotes
reg registroaleatorio();

bloco_i* criaBloco_i();
blocoinicial_i* criaBlocoInicial_i();

int criaArquivo_i();
int criaTempArquivo_i();
void AtualizaHeader_i(FILE*, int, int);
void compactaArquivo_i();

int insereIndex(indexfield, FILE*);	
int removeIndex(int);
int procuraIndex(int key);