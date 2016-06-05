#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <graphviz/cgraph.h>
#include "grafo.h"

typedef struct aresta *aresta;

static aresta cria_aresta( lista lv, Agedge_t *e );

static aresta copia_aresta( aresta a, grafo ng );

static aresta busca_aresta( lista l, vertice origem, vertice destino );

static lista arestas( grafo g );

static vertice busca_vertice( lista l, char *nome );

static int destroi_aresta( void *ptr );

static int destroi_vertice( void *ptr );

//------------------------------------------------------------------------------
// (apontador para) estrutura de dados para representar um grafo
//
// o grafo pode ser
// - direcionado ou não
// - com pesos nas arestas ou não
//
// além dos vértices e arestas, o grafo tem um nome, que é uma "string"
//
// num grafo com pesos nas arestas todas as arestas tem peso, que é um long int
//
// o peso default de uma aresta é 0

struct grafo {

	char        *nome;
	lista		 vertices;
	int          direcionado;
	int          ponderado;
	unsigned int n_vertices;
	unsigned int n_arestas;
};

//------------------------------------------------------------------------------

struct vertice {

	char *nome;
	lista arestas_saida;
	lista arestas_entrada;
	lista vizinhos_direita;
	int *label;
	int visitado;
	
};

//------------------------------------------------------------------------------

struct aresta {

	long int peso;
	vertice	 origem;
	vertice	 destino;
};

//------------------------------------------------------------------------------
// Cria e devolve uma estrutura de aresta
// a partir de uma aresta da biblioteca cgraph

static aresta cria_aresta( lista lv, Agedge_t *e ){

	char *findme = malloc(sizeof(char) * strlen("peso\0")+1);
	strcpy( findme, "peso\0" );
	aresta a   = malloc(sizeof(struct aresta));
	a->origem  = busca_vertice(lv, agnameof(agtail(e)));
	a->destino = busca_vertice(lv, agnameof(aghead(e)));
	char *peso = agget( e, findme );
	a->peso    = peso ? strtol(peso,NULL,10) : 0;
	free(findme);
	return a;
}


//------------------------------------------------------------------------------
// Cria e devolve uma estrutura de aresta mapeando as referências
// de origem e destino da 'aresta a' para o 'grafo ng'

static aresta copia_aresta( aresta a, grafo ng ) {

	aresta at   = malloc(sizeof(struct aresta)+1);
	at->peso    = a->peso;
	at->origem  = busca_vertice(ng->vertices, nome_vertice(a->origem));
	at->destino = busca_vertice(ng->vertices, nome_vertice(a->destino));
	return at;
}

//------------------------------------------------------------------------------
// Procura por uma aresta em uma lista dados os vértices de origem e destino
// Retorna o ponteiro para a resta encontrada ou NULL caso contrário

static aresta busca_aresta( lista l, vertice origem, vertice destino ) {

	if( !l ) return NULL;

	for( no n = primeiro_no(l); n; n = proximo_no(n) ){

		aresta  a = conteudo(n);
		if( (origem == a->origem) && (destino == a->destino) )
			return a;
	}
	return NULL;
}

//------------------------------------------------------------------------------
// Devolve uma lista com as arestas do grafo g

static lista arestas( grafo g ) {

	lista la = constroi_lista();
	if( !g ) return la;

	for( no nv = primeiro_no(g->vertices); nv; nv = proximo_no(nv) ){

		vertice v = conteudo(nv);

		for( no na = primeiro_no(v->arestas_saida); na; na = proximo_no(na) ){
			aresta a = conteudo(na);
			if( !busca_aresta(la, a->origem, a->destino) )
				insere_lista( a, la );
		}

		if( g->direcionado ) {
			for( no na = primeiro_no(v->arestas_entrada); na; na = proximo_no(na) ){
				aresta a = conteudo(na);
				if( !busca_aresta(la, a->origem, a->destino) )
					insere_lista( a, la );
			}
		}
	}
	return la;
}

//------------------------------------------------------------------------------
// Retorna apontador para o vértice com o nome dado como argumento
// ou NULL caso o vértice for encontrado.

static vertice busca_vertice(lista l, char *nome) {

	if( !l )    return NULL;
	if( !nome ) return NULL;

	for( no n = primeiro_no(l); n; n = proximo_no(n) ){
		vertice v = conteudo(n);
		if( ! strcmp(nome, v->nome) )
			return v;
	}

	return NULL;
}

//------------------------------------------------------------------------------
// Libera a memória ocupada por uma aresta

static int destroi_aresta( void *ptr ){

	struct aresta *a = ptr;

	if( a ){
		free( a );
		return 1;
	}
	return 0;
}

//------------------------------------------------------------------------------
// Libera a memória ocupada por um vértice

static int destroi_vertice( void *ptr ){

	struct vertice *v = ptr;

	if( v ){
		int e = destroi_lista( v->arestas_entrada, destroi_aresta );
		int s = destroi_lista( v->arestas_saida,   destroi_aresta );
		if( e && s ) {
			free( v );
			return 1;
		}
	}
	return 0;
}

//------------------------------------------------------------------------------
// devolve o nome do grafo g

char *nome_grafo(grafo g) {
	return g->nome;
}

//------------------------------------------------------------------------------
// devolve 1, se g é direcionado, ou
//         0, caso contrário

int direcionado(grafo g) {
	return g->direcionado;
}

//------------------------------------------------------------------------------
// devolve 1, se g tem pesos nas arestas/arcos,
//      ou 0, caso contrário

int ponderado(grafo g) {
	return g->ponderado;
}

//------------------------------------------------------------------------------
// devolve o número de vértices do grafo g

unsigned int n_vertices(grafo g) {
	return g->n_vertices;
}

//------------------------------------------------------------------------------
// devolve o número de arestas/arcos do grafo g

unsigned int n_arestas(grafo g) {
	return g->n_arestas;
}

//------------------------------------------------------------------------------
// devolve o nome do vertice v

char *nome_vertice(vertice v) {
	return v->nome;
}

//------------------------------------------------------------------------------
// lê um grafo no formato dot de input, usando as rotinas de libcgraph
//
// desconsidera todos os atributos do grafo lido exceto o atributo
// "peso" quando ocorrer; neste caso o valor do atributo é o peso da
// aresta/arco que é um long int
//
// num grafo com pesos todas as arestas/arcos tem peso
//
// o peso default de uma aresta num grafo com pesos é 0
//
// todas as estruturas de dados alocadas pela libcgraph são
// desalocadas ao final da execução
//
// devolve o grafo lido ou
//         NULL em caso de erro

grafo le_grafo(FILE *input) {

    Agraph_t *ag = agread(input, 0);
    if(!(ag && agisstrict(ag)))
        return NULL;

//	struct grafo *g = malloc(sizeof(struct grafo));
	grafo g = malloc(sizeof(struct grafo));
    if( !g ) return NULL;

	g->vertices = constroi_lista();
    g->nome     = malloc(sizeof(char) * strlen(agnameof(ag)+1));
    strcpy(g->nome, agnameof(ag));
    g->direcionado = agisdirected(ag);
    g->n_vertices  = (unsigned int)agnnodes(ag);
    g->n_arestas   = (unsigned int)agnedges(ag);
    g->ponderado   = 0;

	for( Agnode_t *v = agfstnode(ag); v; v = agnxtnode(ag,v) ){

		vertice vt = malloc(sizeof(struct vertice));
		vt->nome   = malloc(sizeof(char) * strlen(agnameof(v))+1);
		strcpy( vt->nome, agnameof(v) );
		vt->arestas_saida   = constroi_lista();
		vt->arestas_entrada = constroi_lista();

		insere_lista(vt, g->vertices);
	}

	for( Agnode_t *v = agfstnode(ag); v; v = agnxtnode(ag,v) ){

		vertice vt = busca_vertice(g->vertices, agnameof(v));

		if( g-> direcionado ){
			for( Agedge_t *e = agfstout(ag,v); e; e = agnxtout(ag,e) ){
				aresta at = cria_aresta(g->vertices, e);
				if( at->peso != 0 ) g->ponderado = 1;
				insere_lista(at, vt->arestas_saida);
			}
			for( Agedge_t *e = agfstin(ag,v); e; e = agnxtin(ag,e) ){
				aresta at = cria_aresta(g->vertices, e);
				if( at->peso != 0 ) g->ponderado = 1;
				insere_lista(at, vt->arestas_entrada);
			}
		}
		else {

			for( Agedge_t *e = agfstedge(ag,v); e; e = agnxtedge(ag,e,v) ){
				aresta at = cria_aresta(g->vertices, e);
				if( at->peso != 0 ) g->ponderado = 1;
				insere_lista(at, vt->arestas_saida);
			}
		}
	}

	if( agclose(ag) )
		return NULL;
	return g;
}

//------------------------------------------------------------------------------
// desaloca toda a memória usada em *g
//
// devolve 1 em caso de sucesso ou
//         0 caso contrário
//
// g é um (void *) para que destroi_grafo() possa ser usada como argumento de
// destroi_lista()

int destroi_grafo(void *g) {

	struct grafo *r = g;

	if( r ) {
		free( r->nome );
		if( destroi_lista( r->vertices, destroi_vertice ) ){
			free( g );
			return 1;
		}
	}
	return 0;
}

//------------------------------------------------------------------------------
// escreve o grafo g em output usando o formato dot, de forma que
//
// 1. todos os vértices são escritos antes de todas as arestas/arcos
//
// 2. se uma aresta tem peso, este deve ser escrito como um atributo
//    de nome "peso"
//
// devolve o grafo escrito ou
//         NULL em caso de erro

grafo escreve_grafo(FILE *output, grafo g) {

	if(!(g && output)) return NULL;

	vertice v;
	aresta  a;
	no nv, na;

    printf("strict %sgraph \"%s\" {\n\n", g->direcionado ? "di" : "", g->nome);

    // Imprime vertices

	for( nv = primeiro_no(g->vertices); nv; nv = proximo_no(nv) ){
		v = conteudo(nv);
		printf("    \"%s\"\n", v->nome);
	}
	printf("\n");

    // Imprime arestas

	lista la = arestas(g);
	const char *dir = g->direcionado ? "->" : "--";
	for( na = primeiro_no(la); na; na = proximo_no(na) ){
		a = conteudo(na);
		printf("    \"%s\" %s \"%s\"", nome_vertice(a->origem), dir, nome_vertice(a->destino));
		if( g->ponderado ) printf(" [peso=%ld]", a->peso);
		printf("\n");
	}
    printf("}\n");

    return g;
}

//------------------------------------------------------------------------------
// Devolve um grafo igual a g

grafo copia_grafo(grafo g) {

	struct grafo *ng = malloc(sizeof(struct grafo));
	if( !ng ) return NULL;

	vertice v;
	no nv, na;

	ng->nome = malloc(sizeof(char) * strlen(nome_grafo(g)+1));
	strcpy(ng->nome, g->nome);

	ng->vertices = constroi_lista();

	for( nv = primeiro_no(g->vertices); nv; nv = proximo_no(nv) ){

		vertice vt = malloc(sizeof(struct vertice));
		v = conteudo(nv);
		vt->nome = malloc(sizeof(char) * strlen(nome_vertice(v))+1);
		strcpy(vt->nome, nome_vertice(v));
		vt->arestas_saida   = constroi_lista();
		vt->arestas_entrada = constroi_lista();

		insere_lista(vt, ng->vertices);
	}

	for( nv = primeiro_no(ng->vertices); nv; nv = proximo_no(nv) ){

		vertice vd = conteudo(nv);
		vertice vo = busca_vertice(g->vertices, nome_vertice(vd));

		for( na = primeiro_no(vo->arestas_saida); na; na = proximo_no(na) ){
			aresta at = copia_aresta( conteudo(na), ng );
			insere_lista(at, vd->arestas_saida);
		}

		if( g->direcionado ){
			 for( na = primeiro_no(vo->arestas_entrada); na; na = proximo_no(na) ){
				aresta at = copia_aresta( conteudo(na), ng );
				insere_lista(at, vd->arestas_entrada);
			 }
		}
	}

	ng->direcionado = g->direcionado;
	ng->ponderado   = g->ponderado;
	ng->n_vertices  = g->n_vertices;
	ng->n_arestas   = g->n_arestas;

	return ng;
}

//------------------------------------------------------------------------------
// devolve a vizinhança do vértice v no grafo g
//
// se direcao ==  0, v é um vértice de um grafo não direcionado
//                   e a função devolve sua vizinhanca
//
// se direcao == -1, v é um vértice de um grafo direcionado e a função
//                   devolve sua vizinhanca de entrada
//
// se direcao ==  1, v é um vértice de um grafo direcionado e a função
//                   devolve sua vizinhanca de saída

lista vizinhanca(vertice v, int direcao, grafo g) {

	lista la, lv = constroi_lista();
	if( !v || !g ) return lv;

	switch( direcao ){
		case  0 : la = v->arestas_saida;   break;
		case  1 : la = v->arestas_saida;   break;
		case -1 : la = v->arestas_entrada; break;
	}

	for( no n = primeiro_no(la); n; n = proximo_no(n) ){
		aresta  a = conteudo(n);

		if( v == a->origem ){
			insere_lista( a->destino, lv );
		}
		if( v == a->destino ){
			insere_lista( a->origem, lv );
		}
	}
	return lv;
}

//------------------------------------------------------------------------------
// devolve o grau do vértice v no grafo g
//
// se direcao ==  0, v é um vértice de um grafo não direcionado
//                   e a função devolve seu grau
//
// se direcao == -1, v é um vértice de um grafo direcionado
//                   e a função devolve seu grau de entrada
//
// se direcao ==  1, v é um vértice de um grafo direcionado
//                   e a função devolve seu grau de saída

unsigned int grau(vertice v, int direcao, grafo g) {

	if( !g ) return 0;

	switch( direcao ){
		case  0 : return tamanho_lista(v->arestas_saida);
		case  1 : return tamanho_lista(v->arestas_saida);
		case -1 : return tamanho_lista(v->arestas_entrada);
	}
	return 0;	// evita warning
}

//------------------------------------------------------------------------------
// devolve 1, se o conjunto dos vertices em l é uma clique em g, ou
//         0, caso contrário
//
// um conjunto C de vértices de um grafo é uma clique em g
// se todo vértice em C é vizinho de todos os outros vértices de C em g

int clique(lista l, grafo g) {

	if( g->direcionado ) return 0;

	for( no n = primeiro_no(l); n; n = proximo_no(n) ){

		vertice vi = conteudo(n);
		lista   lv = vizinhanca(vi,0,g);

		for( no m = primeiro_no(l); m; m = proximo_no(m) ){
			if( n == m ) continue;
			vertice vx = conteudo(m);
			vertice vy = busca_vertice(lv, nome_vertice(vx));
			if( !vy ) return 0;
		}
	}
	return 1;
}


//------------------------------------------------------------------------------
// devolve 1, se v é um vértice simplicial em g, ou
//         0, caso contrário
//
// um vértice é simplicial no grafo se sua vizinhança é uma clique

int simplicial(vertice v, grafo g) {

	if( !v || !g || g->direcionado )
		return 0;

	lista l = vizinhanca(v,0,g);
	if( tamanho_lista(l) == 0 )
		return 1;

	return clique(l,g);
}

//------------------------------------------------------------------------------
// devolve 1, se g é um grafo cordal ou
//         0, caso contrário
//
// um grafo (não direcionado) G é cordal
// se e somente se
// existe uma permutação
//     (v_1, ..., v_n) de V(G)
// tal que
//     v_i é simplicial em G - v_1 - ... - v_{i-1}

int cordal(grafo g) {

	if( !g || g->direcionado )
		return 0;

	grafo ng = copia_grafo(g);

	lista lvg   = ng->vertices;
	int removeu = 1;

	while( tamanho_lista(lvg) > 0 && removeu ){
		removeu = 0 ;
		for( no n = primeiro_no(lvg); n; n = proximo_no(n) ){
			vertice vs = conteudo(n);
			if( simplicial(vs, ng) ){
				lista lvv = vizinhanca(vs, 0, ng);
				for( no m = primeiro_no(lvv); m; m = proximo_no(m) ){
					vertice vv = conteudo(m);
					for( no r = primeiro_no(vv->arestas_saida); r; r = proximo_no(r) ){
						aresta a = conteudo(r);
						if( (a->origem == vs) || (a->destino == vs) )	
							remove_no( vv->arestas_saida , r, destroi_aresta );
					}
				}
				removeu = remove_no( lvg , n, destroi_vertice );
				break;
			}
		}
	}

	destroi_grafo( ng );
	return !tamanho_lista(ng->vertices);
}

//------------------------------------------------------------------------------
// devolve uma lista de vertices com a ordem dos vértices dada por uma 
// busca em largura lexicográfica

lista busca_largura_lexicografica(grafo g){
	lista retorno = constroi_lista();
	lista l = g->vertices;
	for( no n = primeiro_no(l) ; n ; n = proximo_no(n)){
		vertice v= conteudo(n);
		v->label = malloc(sizeof(int) * tamanho_lista(l));
		for( int i = 0; i< (int)tamanho_lista(l);++i){
			v->label[i]=0;
			
		}	
		v-> visitado=0;
	}
	
	int label_atual = (int)tamanho_lista(g->vertices);
	no aux= primeiro_no(l);
	vertice primeiro= conteudo(aux);
	primeiro->label[0]= label_atual;
	label_atual--;
	vertice n;
	while( (n=vertice_maior_label(l)) != NULL){
		n->visitado=1;
		lista vizinhos= vizinhanca(n,0,g);
		for( no m =  primeiro_no(vizinhos); m ; m= proximo_no(m)){
			vertice x = conteudo(m);
			adiciona_label(x,label_atual);
		}
		label_atual--;
		escreve_vertice(n,(int)tamanho_lista(l));
		insere_lista(n,retorno);
		
	}

	
	return retorno;
}

//busca o vertice com o maior label lexicografico
vertice vertice_maior_label(lista l){
	no aux = primeiro_no(l);
	vertice retorno = conteudo(aux);
	 for( no n = primeiro_no(l); n ; n = proximo_no(n)){
			vertice v = conteudo(n);
			if(v->visitado == 0){			
			retorno = v;
			break;
			}
	}
		
	for( no n = primeiro_no(l); n ; n = proximo_no(n)){
		vertice v = conteudo(n);
		if(v->visitado == 0){
			if(label_maior(retorno,v,(int)tamanho_lista(l))){
			retorno = v;
			
			}		
		}
	}	
	return retorno->visitado==1? NULL:retorno;	
}

//retorna 0 se v é maior, e 1 se x é maior
int label_maior(vertice v, vertice x, int tamanho){
	for( int i = 0; i<tamanho; ++i){
		if(v->label[i] != x->label[i]){
			if( v->label[i] > x->label[i]){
				return 0;
			}else{
				return 1;
			}			
		}
	}
	return 0;	
}

void adiciona_label(vertice v, int valor){
	int i = 0;
	while (v->label[i] > 0){
		++i;		
	}
	v->label[i] = valor;
}

void escreve_vertice(vertice v,int tam){
	
	printf("nome vertice: %s    ", v->nome);
		for(int i = 0; i< tam; ++i){
			printf("%d,", v->label[i]);
		}
		printf("\n");
	
}


void escreve_especial(grafo g){
	int tamanho = (int)tamanho_lista(g->vertices);
	
	for(no n = primeiro_no(g->vertices); n; n= proximo_no(n)){
		vertice v= conteudo(n);
		printf("nome vertice: %s    ", v->nome);
		for(int i = 0; i< tamanho; ++i){
			printf("%d,", v->label[i]);
		}
		printf("\n");
	}
	
	
}

//------------------------------------------------------------------------------
// devolve 1, se a lista l representa uma 
//            ordem perfeita de eliminação para o grafo g ou
//         0, caso contrário
//
// o tempo de execução é O(|V(G)|+|E(G)|)

int ordem_perfeita_eliminacao(lista l, grafo g);
	// monta lista de vizinhos a direita
	//
	
	
	



void monta_vizinhos_a_direita(lista l, grafo g){
	printf("passei\n");
	for (no n= primeiro_no(l);n; n = proximo_no(n)){
		vertice v = conteudo(n);
		v->visitado=0;
	}
	//percorre a lista de vertices
	for (no n= primeiro_no(l);n; n = proximo_no(n)){
		vertice v = conteudo(n);
		printf("entrei\n");
		for( no m = primeiro_no(v->arestas_saida); m ; m= proximo_no(m)){
			printf("entrei2\n");
			aresta e = conteudo(m);
			lista vizinhos_direita=v->vizinhos_direita;
			vertice destino= e->destino;
			if(destino->visitado==0){
				insere_lista(e->destino,vizinhos_direita);
				
			}
		}
		
		lista viz=v->vizinhos_direita;
		for(no x = primeiro_no(viz);x;x = proximo_no(x)){
			vertice ver= conteudo(x);
			printf("nome vertice: %s \n   ",ver->nome );
				
		}
		
		v->visitado=1;
	}	
}



