#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <graphviz/cgraph.h>
#include "grafo.h"
#include "lista.h"

typedef struct aresta *aresta;

aresta cria_aresta( Agedge_t *e );

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
// Devolve uma estrutura de aresta a partir de uma aresta da biblioteca cgraph

aresta cria_aresta( Agedge_t *e ){

	char *peso;
	aresta a   = malloc(sizeof(struct aresta));
	a->origem  = agnameof(agtail(e));
	a->destino = agnameof(aghead(e));
	peso        = agget(e,(char*)"peso");
	a->peso    = peso ? strtol(peso,NULL,10) : 0;
	return a;
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

	g->vertices    = constroi_lista();
    g->nome        = agnameof(ag);
    g->direcionado = agisdirected(ag);
    g->n_vertices  = (unsigned int)agnnodes(ag);
    g->n_arestas   = (unsigned int)agnedges(ag);

	for( Agnode_t *v=agfstnode(ag); v; v=agnxtnode(ag,v) ){

		vertice vt = malloc(sizeof(struct vertice));
		vt->nome   = agnameof(v);
		vt->arestas_saida = constroi_lista();

		if( g->direcionado ) {
			//vt->grau_saida      = agdegree(ag,v,FALSE,TRUE);
			//vt->grau_entrada    = agdegree(ag,v,TRUE,FALSE);
			vt->arestas_entrada = constroi_lista();
			
			for( Agedge_t *e = agfstout(ag,v); e; e = agnxtout(ag,e) ){
				aresta at = cria_aresta(e);
				insere_lista(at, vt->arestas_saida);
			}
			for( Agedge_t *e = agfstin(ag,v); e; e = agnxtin(ag,e) ){
				aresta at = cria_aresta(e);
				insere_lista(at, vt->arestas_entrada);
			}
		}
		else {
			//vt->grau_saida      = agcountuniqedges(ag,v,TRUE,TRUE);
			//vt->grau_entrada    = -1;
			vt->arestas_entrada = NULL;
			
			for( Agedge_t *e = agfstedge(ag,v); e; e = agnxtedge(ag,e,v) ){
				aresta at = cria_aresta(e); 
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

	return (g) ? 1 : 0;
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
	
	nv = primeiro_no(g->vertices);
	while( nv != NULL ){
		v = conteudo(nv);
		printf("    \"%s\"\n", v->nome);
		nv = proximo_no(nv);
	}
	printf("\n");

    // Imprime arestas

	const char *dir = g->direcionado ? "->" : "--";
	nv = primeiro_no(g->vertices);
	while( nv != NULL ){
		v = conteudo(nv);
		na = primeiro_no(v->arestas_saida);
		while( na != NULL ){
			a = conteudo(na);
			printf("    \"%s\" %s \"%s\"", a->origem, dir, a->destino);
			if(a->peso > 0) printf(" [peso=%ld]", a->peso);
			printf("\n");
			na = proximo_no(na);
		}
		nv = proximo_no(nv);
	}
    printf("}\n");

    return g;
}

//------------------------------------------------------------------------------
// devolve um grafo igual a g

grafo copia_grafo(grafo g) {
	
	return ( g ) ? g : NULL;
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
	//if(direcao==0){
		//return tamanho_lista(v->arestas_saida);		
	//}
	//if(direcao ==-1){
		//return tamanho_lista(v->arestas_entrada);
	//}
	//if(direcao=1){
		//return tamanho_lista(v->arestas_saida);
	//}
	return ( g && v && direcao ) ? 1 : 0;
}

//retorna o vertice do grafo

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


void escreveAresta(vertice v){
	
	for (no k=primeiro_no(v->arestas_saida);k;k=proximo_no(k)){
		aresta a= conteudo(k);
		printf("nome aresta (dest):%s \n",a->destino);
		printf("nome aresta (origem):%s \n \n",a->origem);
	}
	printf("imprimido \n");
	
}


//------------------------------------------------------------------------------
// devolve 1, se o conjunto dos vertices em l é uma clique em g, ou
//         0, caso contrário
//
// um conjunto C de vértices de um grafo é uma clique em g 
// se todo vértice em C é vizinho de todos os outros vértices de C em g

int clique(lista l, grafo g) {
	if(g->direcionado) return 0;
	no aux;
	lista isClique = constroi_lista();
	for (no n=primeiro_no(l);n;n=proximo_no(n)){
		
		lista vizinhos= constroi_lista();
		vertice test= conteudo(n);
		//vertice vertAtual=buscaVertice(nome_vertice(test),g);
		//printf("nome vertice prin: %s \n",nome_vertice(vertAtual));
		vizinhos = vizinhanca(vertAtual,0,g);
		
		for(no j=primeiro_no(l);j;j=proximo_no(j)){
			for(no k=primeiro_no(vizinhos);k;k=proximo_no(k)){
				vertice a= conteudo(k);
				vertice v = conteudo(j);
				printf("nome vertice: %s \n",nome_vertice(v));
				printf("nome aresta (dest):%s \n",a->destino);
				printf("nome aresta (origem):%s \n",a->origem);
				if(!strcmp(nome_vertice(v),a->destino)){
				aux=insere_lista(conteudo(j),isClique);
				printf("ok  valor: %s\n",nome_vertice(v));
				
				break;
				}
			}
		}
	}
			if(tamanho_lista(l)==tamanho_lista(isClique)){
				printf("ok são iguais\n ");
			for(no x=primeiro_no(l);x;x=proximo_no(x)){
				for(no y=primeiro_no(isClique);y;y=proximo_no(y)){
					vertice a1= conteudo(x);
					vertice a2= conteudo(y);
					printf("nome verticea1: %s \n",nome_vertice(a1));
					printf("nome verticea2: %s \n",nome_vertice(a2));
					if(!strcmp(nome_vertice(a1),nome_vertice(a2))){
					printf("ok são iguais2\n ");
					}
				}
			}
			//printf("ok");
		}
		else{ printf("deuruim");return 0;}
		return 1;
}


//------------------------------------------------------------------------------
// devolve 1, se v é um vértice simplicial em g, ou
//         0, caso contrário
//
// um vértice é simplicial no grafo se sua vizinhança é uma clique

int simplicial(vertice v, grafo g) {
	
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

	return ( g ) ? 1 : 0;
}

