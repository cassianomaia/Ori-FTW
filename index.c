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
		blocoinicial_i* bloco= criaBlocoInicial_i(); //Criação do bloco inicial do arquivo
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
		blocoinicial* bloco= criaBlocoInicial_i();
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

