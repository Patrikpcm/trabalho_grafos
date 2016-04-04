#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <graphviz/cgraph.h>
#include "grafo.h"

typedef struct aresta *aresta;

aresta cria_aresta( Agedge_t *e );

aresta copia_aresta( aresta a );

vertice busca_vertice(lista l, char *nome);


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
};

//------------------------------------------------------------------------------

struct aresta {
	
	long int peso;
	char    *origem;
	char    *destino;
};

//------------------------------------------------------------------------------
// Cria e devolve uma estrutura de aresta 
// a partir de uma aresta da biblioteca cgraph

aresta cria_aresta( Agedge_t *e ){

	char *peso;
	char *findme = malloc(sizeof(char) * strlen("peso\0")+1);
	strcpy( findme, "peso\0" );
	aresta a   = malloc(sizeof(struct aresta));
	a->origem  = agnameof( agtail(e) );
	a->destino = agnameof( aghead(e) );
	peso       = agget( e, findme );
	a->peso    = peso ? strtol(peso,NULL,10) : 0;
	free(findme);
	return a;
}


//------------------------------------------------------------------------------
// Cria e devolve uma estrutura de aresta 
// a partir de uma outra aresta 

aresta copia_aresta( aresta a ){

	aresta at = malloc(sizeof(struct aresta));
	at->peso = a->peso;
	at->origem = malloc(sizeof(char) * strlen(a->origem)+1);
	strcpy(at->origem, a->origem);
	at->destino = malloc(sizeof(char) * strlen(a->destino)+1);
	strcpy(at->destino, a->destino);	
	return at;
}


//------------------------------------------------------------------------------
// Retorna apontador para o vértice com o nome dado como argumento
// ou NULL caso o vértice for encontrado.

vertice busca_vertice(lista l, char *nome) {

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

    struct grafo *g = malloc(sizeof(struct grafo *));
    if( !g ) return NULL;

	g->vertices = constroi_lista();
    g->nome     = malloc(sizeof(char) * strlen(agnameof(ag)+1));
    strcpy(g->nome, agnameof(ag));
    g->direcionado = agisdirected(ag);
    g->n_vertices  = (unsigned int)agnnodes(ag);
    g->n_arestas   = (unsigned int)agnedges(ag);
    g->ponderado   = 0;

	for( Agnode_t *v=agfstnode(ag); v; v=agnxtnode(ag,v) ){

		vertice vt = malloc(sizeof(struct vertice));
		vt->nome   = agnameof(v);
		vt->arestas_saida = constroi_lista();

		if( g->direcionado ) {

			vt->arestas_entrada = constroi_lista();
			
			for( Agedge_t *e = agfstout(ag,v); e; e = agnxtout(ag,e) ){
				aresta at = cria_aresta(e);
				if( at->peso != 0 ) g->ponderado = 1;
				insere_lista(at, vt->arestas_saida);
			}
			for( Agedge_t *e = agfstin(ag,v); e; e = agnxtin(ag,e) ){
				aresta at = cria_aresta(e);
				if( at->peso != 0 ) g->ponderado = 1;
				insere_lista(at, vt->arestas_entrada);
			}
		}
		else {

			vt->arestas_entrada = NULL;
			
			for( Agedge_t *e = agfstedge(ag,v); e; e = agnxtedge(ag,e,v) ){
				aresta at = cria_aresta(e);
				if( at->peso != 0 ) g->ponderado = 1;
				if( !strcmp( vt->nome, at->origem ) )
					 insere_lista(at, vt->arestas_saida);
				else free(at);
			}
		}

		insere_lista(vt, g->vertices);
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
/*
	struct grafo *r = g;
	
	if( r ) {
		//printf("%s\n", h->nome);
		free( r->nome );
		//if( h->nome ) return 0;
		//r->nome = realloc(r->nome,1);
	}
	else return 0;
*/
/*
struct grafo {

	char        *nome;
	lista		 vertices;
	int          direcionado;
	int          ponderado;
	unsigned int n_vertices;
	unsigned int n_arestas;
};
*/
	
	return (g) ? 0 : 1;
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

	const char *dir = g->direcionado ? "->" : "--";
	for( nv = primeiro_no(g->vertices); nv; nv = proximo_no(nv) ){
		v = conteudo(nv);
		for( na = primeiro_no(v->arestas_saida); na; na = proximo_no(na) ){
			a = conteudo(na);
			printf("    \"%s\" %s \"%s\"", a->origem, dir, a->destino);
			if( g->ponderado ) printf(" [peso=%ld]", a->peso);
			printf("\n");
		}
	}
    printf("}\n");

    return g;
}

//------------------------------------------------------------------------------
// Devolve um grafo igual a g

grafo copia_grafo(grafo g) {
	
	struct grafo *ng = malloc(sizeof(struct grafo *));
	if( !ng ) return NULL;
	
	vertice v;
	no nv, na;
	
	ng->nome = malloc(sizeof(char) * strlen(g->nome)+1);
	strcpy(ng->nome, g->nome);
	
	ng->vertices = constroi_lista();
	
	for( nv = primeiro_no(g->vertices); nv; nv = proximo_no(nv) ){
		
		vertice vt = malloc(sizeof(struct vertice));
		v = conteudo(nv);
		vt->nome = malloc(sizeof(char) * strlen(v->nome)+1);
		strcpy(vt->nome, v->nome);

		vt->arestas_saida = constroi_lista();
		for( na = primeiro_no(v->arestas_saida); na; na = proximo_no(na) ){
			aresta at = copia_aresta(conteudo(na));
			insere_lista(at, vt->arestas_saida);
		}
		
		if( g->direcionado ){
			 vt->arestas_entrada = constroi_lista();
			 for( na = primeiro_no(v->arestas_entrada); na; na = proximo_no(na) ){
				aresta at = copia_aresta(conteudo(na));
				insere_lista(at, vt->arestas_entrada);
			 }
		}
		else vt->arestas_entrada = NULL;
		
		insere_lista(vt, ng->vertices);
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
	if( !g ) return lv;

	switch( direcao ){
		case  0 : la = v->arestas_saida;   break;
		case  1 : la = v->arestas_saida;   break;
		case -1 : la = v->arestas_entrada; break;
	}
	
	for( no n = primeiro_no(la); n; n = proximo_no(n) ){
		aresta  a = conteudo(n);
		vertice x = busca_vertice(g->vertices, a->destino);
		if( x ) insere_lista(x,lv);
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
	
	return ( g && l ) ? 1 : 0;
}


//------------------------------------------------------------------------------
// devolve 1, se v é um vértice simplicial em g, ou
//         0, caso contrário
//
// um vértice é simplicial no grafo se sua vizinhança é uma clique

int simplicial(vertice v, grafo g) {
	
	if( g->direcionado ) return 0;
	
	return ( g && v ) ? 1 : 0;
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
	
	if( g->direcionado ) return 0;

	return ( g ) ? 1 : 0;
}

