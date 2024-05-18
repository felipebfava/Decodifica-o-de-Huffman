
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//----Para permitir a acentuação da língua Português-Brasil----
#include <locale.h>

#define TAM 256

typedef struct no {
	unsigned char caracter;
	int frequencia;
	struct no *	esq, *dir, *prox;
}No;

typedef struct{
	No *inicio;
	int tam;
}Lista;

//----Tabela de Frequência----
void inicializa_tabela_com_zero(unsigned int tab[]){
	int i;
	for(i = 0; i < TAM; i++) {
		tab[i] = 0;
	}
}

void preenche_tab_frequencia(unsigned char texto[], unsigned int tab[]) {
	int i = 0;
	
	while(texto[i] != '\0') {
		tab[texto[i]]++;
		i++;
	}
}

void imprime_tab_frequencia(unsigned int tab[]) {
	int i;
	
	printf("\tTABELA DE FREQUENCIA\n");
	for(i = 0; i < TAM; i++) {
		if(tab[i] > 0) {
			//%d imprime o valor 0 - 255, %u é por causa do unsigned (só valores positivos), %c para caracteres em ASCII
			printf("\t%d = %u = %c\n", i, tab[i], i);
		}
	}
}

//----Lista Encadeada----
void criar_lista(Lista *lista) {
	lista->inicio = NULL;
	lista->tam = 0;
}

void inserir_ordenado(Lista * lista, No *no) {
	No *aux;
	
	if(lista->inicio == NULL) {
		lista->inicio = no;
	}
	else if(no->frequencia < lista->inicio->frequencia) {
		no->prox = lista->inicio;
		lista->inicio = no;
	}
	else {
		aux = lista->inicio;
		
		while(aux->prox && aux->prox->frequencia <= no->frequencia) {
			aux = aux->prox;
		}
		no->prox = aux->prox;
		aux->prox = no;
	}
	
	lista->tam++;
}

void preencher_lista(unsigned int tab[], Lista *lista) {
	int i;
	No *novo;
	
	for(i = 0; i < TAM; i++) {
		if(tab[i] > 0) {
			novo = malloc(sizeof(No));
			
			if(novo) {
				novo->caracter = i;
				novo->frequencia = tab[i];
				novo->dir = NULL;
				novo->esq = NULL;
				novo->prox = NULL;
				
				inserir_ordenado(lista, novo);
			}
			else {
				printf("\n\tERRO ao alocar memoria em: preencher_lista");
				break;
			}
		}
	}
}

void imprimir_lista(Lista *lista) {
	No *aux = lista->inicio;
	
	printf("\n\tLista Ordenada: Tamanho: %d\n", lista->tam);
	while(aux != NULL) {
		printf("\tCaracter: %c Frequência: %d\n", aux->caracter, aux->frequencia);
		aux = aux->prox;
	}
}

//----Árvore----
No* remove_no_inicio(Lista *lista) {
	No *aux = NULL;
	
	if(lista->inicio != NULL) { //if(lista->inicio)
		aux = lista->inicio;
		lista->inicio = aux->prox;
		aux->prox = NULL;
		lista->tam--;
	}
	
	return aux;
}

No* montar_arvore(Lista* lista) {
	No *primeiro, *segundo, *novo;
	
	while(lista->tam > 1) {
		primeiro = remove_no_inicio(lista);
		segundo = remove_no_inicio(lista);
		novo = malloc(sizeof(No));
		
		if(novo) {
			novo->caracter = '+';
			novo->frequencia = primeiro->frequencia + segundo->frequencia;
			novo->esq = primeiro;
			novo->dir = segundo;
			novo->prox = NULL;
			
			inserir_ordenado(lista, novo);
		}
		else {
			printf("\n\tERRO ao alocar memoria em: montar_arvore");
			break; //ou também return null;
		}
	}
	
	return lista->inicio;
}

void imprimir_arvore(No *raiz, int tam) {
	
	if(raiz->esq == NULL && raiz->dir == NULL) {
		printf("\tFolha: %c, \tAltura: %d\n", raiz->caracter, tam);
	}
	else {
		imprimir_arvore(raiz->esq, tam + 1);
		imprimir_arvore(raiz->dir, tam + 1);
	}
}

//----Dicionário----
int altura_arvore(No *raiz) {
	
	int esq, dir;
	
	if(raiz == NULL) {
		return -1;
	}
	else {
		esq = altura_arvore(raiz->esq) + 1;
		dir = altura_arvore(raiz->dir) + 1;
		
		if(esq > dir)
			return esq;
		else
			return dir;
	}
}

char** aloca_dicionario(int colunas) {
	
	char **dicionario;
	int i;
	
	dicionario = malloc(sizeof(char*) * TAM);
	
	for(i = 0; i < TAM; i++)
		dicionario[i] = calloc(colunas, sizeof(char));
	
	return dicionario;
}

void gerar_dicionario(char **dicionario, No *raiz, char *caminho, int colunas) {
	
	char esquerda[colunas], direita[colunas];
	
	if(raiz->esq == NULL && raiz->dir == NULL) {
		strcpy(dicionario[raiz->caracter], caminho);
	}
	else {
		strcpy(esquerda, caminho);
		strcpy(direita, caminho);
		
		strcat(esquerda, "0");
		strcat(direita, "1");
		
		gerar_dicionario(dicionario, raiz->esq, esquerda, colunas);
		gerar_dicionario(dicionario, raiz->dir, direita, colunas);
	}
}

void imprime_dicionario(char **dicionario) {
	int i;
	
	printf("\n\tDicionario:\n");
	for(i = 0; i < TAM; i++) {
		if(strlen(dicionario[i]) > 0)
			printf("\t%3d: %s\n", i, dicionario[i]);
	}
}

//----Codificar----

int calcula_tamanho_string(char **dicionario, unsigned char *texto) {
	
	int i = 0, tam = 0;
	
	while(texto[i] != '\0') {
		tam = tam + strlen(dicionario[texto[i]]);
		i++;
	}
	
	//contando o caracter de fim de string também
	return tam + 1;
}


char* codificar(char **dicionario, unsigned char *texto) {
	
	int i = 0, tam = calcula_tamanho_string(dicionario, texto);
	char *codigo = calloc(tam, sizeof(char));
	
	while(texto[i] != '\0') {
		strcat(codigo, dicionario[texto[i]]);
		i++;
	}
	
	return codigo;
}

//----Decodificar----
char* decodificar(unsigned char texto[], No *raiz) {
	
	int i = 0;
	No *aux = raiz;
	char temp[2];
	char *decodificado = calloc(strlen(texto), sizeof(char));
	
	while(texto[i] != '\0') {
		if(texto[i] == '0')
			aux = aux->esq;
		else
			aux = aux->dir;
		
		if(aux->esq == NULL && aux->dir == NULL) {
			temp[0] = aux->caracter;
			temp[1] = '\0';
			strcat(decodificado, temp);
			aux = raiz;
		}
		
		i++;
	}
	return decodificado;
}
    
int main(){
    
    unsigned char texto[] = "O rato roeu a roupa do rei de Roma";
    unsigned int tabela_frequencia[TAM];
    Lista lista;
    No *arvore;
    int colunas;
    char **dicionario;
    char *codificado, *decodificado;
    
    //----Para permitir a acentuação da língua Português-Brasil----
    setlocale(LC_ALL, "Portuguese");
	
    //----Tabela de Frequência----
	inicializa_tabela_com_zero(tabela_frequencia);
	preenche_tab_frequencia(texto, tabela_frequencia);
	imprime_tab_frequencia(tabela_frequencia);
	
	//----Lista Encadeada e Ordenada----
	criar_lista(&lista);
	preencher_lista(tabela_frequencia, &lista);
	imprimir_lista(&lista);
	
	//----Árvore----
	arvore = montar_arvore(&lista);
	printf("\n\tÁrvore de Huffman\n");
	imprimir_arvore(arvore, 0);
	
	//----Dicionário----
    colunas = altura_arvore(arvore) + 1;
    dicionario = aloca_dicionario(colunas);
    gerar_dicionario(dicionario, arvore, "", colunas);
    imprime_dicionario(dicionario);
    
    //----Codificar----
    codificado = codificar(dicionario, texto);
    printf("\n\tTexto Codificado: %s\n", codificado);
    
    //----Decodificar----
    decodificado = decodificar(codificado, arvore);
    printf("\n\t\nTexto Decodificado: %s\n", decodificado);
    
    return 0;
}
