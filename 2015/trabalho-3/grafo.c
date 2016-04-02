#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <graphviz/cgraph.h>
#include "grafo.h"

//------------------------------------------------------------------------------
// valor que representa "infinito"

const long int infinito = LONG_MAX;

//------------------------------------------------------------------------------
// (apontador para) estrutura de dados que representa um vértice do grafo
//  os vértices do grafo tem nome que são "string"s quaisquer

struct vertice {
	char	*nome;
};

//-----------------------------------------------------------------------------
// nó da lista encadeada cujo conteúdo é um void *

struct no {
	void 	*e;
};

//-----------------------------------------------------------------------------
// lista encadeada

struct lista {
	int		nver;
	no		stno;
};

//------------------------------------------------------------------------------
// grafo
// 
// o grafo pode ser
// - direcionado ou não
// - com pesos nas arestas ou não
// 
// o grafo tem um nome, que é uma "string" qualquer
// 
// num grafo com pesos nas arestas, todas as arestas tem peso
// 
// o peso de uma aresta é um long int e seu valor default é zero

struct grafo {
    char	 *nome;		 // nome do grafo
    int		 eh_dir;	 // 1 -> se é direcionado;  0 -> caso contrário
    int		 conexo;	 // 1 -> conexo;            0 -> caso contrário
    int		 fconexo;	 // 1 -> fortemente conexo; 0 -> caso contrário
    int		 c_peso;	 // 1 -> arestas tem peso;  0 -> caso contrário
    int 	 n_vert;	 // número de vertices do grafo
    int		 n_ares;	 // número de arestas do grafo
    long int diam;	 	 // diâmetro do grafo
};

//==============================================================================

//------------------------------------------------------------------------------
// devolve o primeiro nó da lista l, 
//			ou NULL se l é vazia

no primeiro_no(lista l) {
	
}


//------------------------------------------------------------------------------
// devolve o sucessor do nó n em l
//			ou NULL, se n for o último nó de l

no proximo_no(no n) {
	
	return n->prx;
}


//------------------------------------------------------------------------------
// devolve o conteúdo do nó n

void *conteudo(no n) {
	
	return n->e;
}


//------------------------------------------------------------------------------
// desaloca a lista l e todos os seus nós
// 
// se destroi != NULL invoca
//
//     destroi(conteudo(n)) 
//
// para cada nó da lista. 
//
// devolve 1 em caso de sucesso,
//      ou 0 em caso de falha

int destroi_lista(lista l, int destroi(void *)) {

}


//------------------------------------------------------------------------------
// devolve o nome do vertice v

char *nome_vertice(vertice v) {
	
	return v->nome;
}


//------------------------------------------------------------------------------
// lê um grafo no formato dot de input, usando as rotinas de libcgraph
// 
// desconsidera todos os atributos do grafo lido
// exceto o atributo "peso" nas arestas onde ocorra
// 
// devolve o grafo lido,
//      ou NULL, em caso de erro 
//
// desaloca a estrtura de dados devolvida pelas rotinas de libcgraph
// quando da leitura do grafo assim que ela não seja mais necessária

grafo le_grafo(FILE *input) {
	
	if( !input ) return NULL;
	
	int    i, j, vn=0, an=0;
	char  *w;
	grafo  g;
	
    Agraph_t *ag = agread(input, 0);
    if(!(ag && agisstrict(ag))) 
        return NULL;
        
 	/* Aloca memória para a estrutura do grafo */
	g = malloc(sizeof(struct grafo));
	
	if( !g ) return NULL;
	
    /* Inicializa variáveis de g com dados do grafo */
    g->nome   = agnameof(ag);
    g->eh_dir = agisdirected(ag);
    g->n_vert = agnnodes(ag);
    g->n_ares = agnedges(ag);
    
    /* Inicializa variáveis de g com 'valores padrão' */
    g->diam    = -1;
    g->conexo  = -1;
    g->fconexo = -1;
    g->c_peso  = -1;


    /* Aloca memória para lista de vértices */
//    g->vlst = malloc(g->n_vert*sizeof(char *));
//    if( !g->vlst ) return NULL;
    
    /* Aloca memória para a matriz de pesos */
//    g->mat  = malloc(g->n_vert*sizeof(double *));
//    if( !g->mat ) return NULL;
    
//    for(i=0; i < g->n_vert; i++){
		
//		g->mat[i] = malloc(g->n_vert*sizeof(double));
//		if( !g->mat[i] ) return NULL;
		
//		for(j=0; j < g->n_vert; j++){
//			g->mat[i][j] = 0.0;
//		}
//    }
    
	/* Lista de arestas ou arcos percorridos */
	Agedge_t **ax = malloc(g->n_ares*sizeof(Agedge_t *));

    for(Agnode_t *v=agfstnode(ag); v; v=agnxtnode(ag,v)){
		
        /* Percorre os arcos do vértice 'v' */
         for(Agedge_t *a=agfstout(ag,v); a; a=agnxtout(ag,a)){
		
		/* Percorre as arestas do vértice 'v' */
		// for(Agedge_t *a=agfstedge(ag,v); a; a=agnxtedge(ag,a,v)){

			//if(!is_edgeinlist(ax, a, an)){
				ax[an] = a; an++;
				
				/* Processa o vértice 'v' da aresta {v,u} */
				if((i = is_nodeinlist(g,agnameof(agtail(a)),vn)) < 0 ){
					i = vn++;
					g->vlst[i]=agnameof(agtail(a));
				}
				/* Processa o vértice 'u' da aresta {v,u} */
				if((j = is_nodeinlist(g,agnameof(aghead(a)),vn)) < 0 ){
					j = vn++;
					g->vlst[j]=agnameof(aghead(a));
				}
				/* Processa o peso das arestas, se existir */
				if((int)(w = agget(a,(char*)"peso"))){
					g->mat[i][j] = strtod(w, NULL);
					g->c_peso    = 1;
				}
				else{
					g->mat[i][j] = 1.0;
					g->c_peso    = 0;
				}
			//}
        }
    }

    free(ag);
    free(ax);
 
 	return g;
}


//------------------------------------------------------------------------------
// desaloca toda a memória usada em g
// 
// devolve 1 em caso de sucesso,
//      ou 0, caso contrário

int destroi_grafo(void *g) {

}


//------------------------------------------------------------------------------
// escreve o g em output usando o formato dot, de forma que
// 
// 1. todos os vértices são escritos antes de todas as arestas (arcos)
// 2. se uma aresta (arco) tem peso, este deve ser escrito como um atributo
//
// devolve o grafo escrito,
//      ou NULL, em caso de erro 

grafo escreve_grafo(FILE *output, grafo g) {

}


//------------------------------------------------------------------------------
// devolve o nome do grafo g

char *nome(grafo g) {

	return g->nome;
}


//------------------------------------------------------------------------------
// devolve o número de vértices do grafo g

unsigned int n_vertices(grafo g) {

	return (unsigned int)g->n_vert;
}


//------------------------------------------------------------------------------
// devolve 1, se g é direcionado,
//      ou 0, caso contrário

int direcionado(grafo g) {

	return g->eh_dir;
}


//------------------------------------------------------------------------------
// devolve 1, se g é não direcionado e conexo,
//      ou 0, caso contrário

int conexo(grafo g) {

	return direcionado(g) ? 0 : g->conexo;
}


//------------------------------------------------------------------------------
// devolve uma árvore geradora mínima do grafo g,
//      ou NULL, se g não for um grafo não direcionado conexo

grafo arvore_geradora_minima(grafo g) {
	
}


//------------------------------------------------------------------------------
// devolve uma lista de grafos onde cada grafo é um componente de g

lista componentes(grafo g) {
	
}


//------------------------------------------------------------------------------
// devolve uma lista de grafos onde cada grafo é um bloco de g
//      ou NULL, se g é um grafo direcionado

lista blocos(grafo g) {
	
}


//------------------------------------------------------------------------------
// devolve uma lista dos vértices de g ordenados topologicamente,
//      ou NULL se g não é um grafo direcionado

lista ordena(grafo g) {
	
}


//------------------------------------------------------------------------------
// devolve uma arborescência de caminhos mínimos de g de raiz r

grafo arborescencia_caminhos_minimos(grafo g, vertice r) {
	
}


//------------------------------------------------------------------------------
// devolve um grafo com pesos, onde
//
//     - os vértices tem os mesmos nomes que os de g
//
//     - a aresta {u,v} (arco (u,v)) ocorre se v é alcançável a partir
//       de u em g
//
//     - o peso da aresta {u,v} (arco (u,v)) é a distância de u a v em g
//
// o grafo é computado usando a função arborescencia_caminhos_minimos()

grafo distancias(grafo g) {
	
}


//------------------------------------------------------------------------------
// devolve 1, se g é fortemente conexo,
//      ou 0, caso contrário

int fortemente_conexo(grafo g) {

	return g->fconexo;
}


//------------------------------------------------------------------------------
// devolve o diâmetro de g

long int diametro(grafo g) {

	return g->diam;
}
