#include <stdlib.h>
#include <stdio.h>
#include <graphviz/cgraph.h>
#include "grafo.h"

typedef struct aresta *aresta;

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

struct vertice {

	char *nome;
	lista vizinhos_saida;
	lista vizinhos_entrada;
};

struct aresta {
	
	vertice  origem;
	vertice  destino;
	long int peso;
};

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

    g->nome        = agnameof(ag);
    g->direcionado = agisdirected(ag);
    g->n_vertices  = (unsigned int)agnnodes(ag);
    g->n_arestas   = (unsigned int)agnedges(ag);


	int    i = 0;
	int    j = 0;
	double peso;


    for(Agnode_t *v=agfstnode(ag); v; v=agnxtnode(ag,v)){
//        g->l_vert[i] = agnameof(v); i+=1;
		  printf("%s\n", agnameof(v));

        // Grafos direcionados //
        if( g->direcionado ){
            for(Agedge_t *a=agfstout(ag,v); a; a=agnxtout(ag,a)){

				printf("%s", agnameof(agtail(a)));
				printf("%s", " ---> ");
				printf("%s\n", agnameof(aghead(a)));

/*
                g->l_tail[j] = agnameof(agtail(a));
                g->l_head[j] = agnameof(aghead(a));
                w = agget(a,(char*)"peso");
                g->l_peso[j] = w ? strtod(w,NULL) : 0.0;
                j+=1;
*/
            }
        }
        // Grafos nao direcionados //
        else{
            for(Agedge_t *a=agfstedge(ag,v); a; a=agnxtedge(ag,a,v)){


				printf("%s", agnameof(agtail(a)));
				printf("%s", " ---> ");
				printf("%s\n", agnameof(aghead(a)));


/*
                // Evita que uma aresta ja visitada seja guardada
                inlst=0;                
                for(k=0; k < avn; k++){
                    if(ageqedge(a, avl[k])){
                        inlst=1;                        
                        break;
                    }
                }
                
                if( !inlst ){
                    g->l_tail[j] = agnameof(agtail(a));
                    g->l_head[j] = agnameof(aghead(a));
                    w = agget(a,(char*)"peso");
                    g->l_peso[j] = w ? strtod(w,NULL) : 0.0;
                    // Guarda aresta visitada numa lista
                    avl[avn]=a;
                    avn+=1;   
                    j+=1;
                }
*/
                
            }
        }
    }




	return g;
	
//	if (!input)
//		 return NULL;
//	else return g;
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


	if (g) 
		 return 1;
	else return 0;

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
	
	
//    int i,j;

    if(!(g && output)) return NULL;
/*    
    printf("strict %sgraph \"%s\" {\n\n", g->direcionado ? "di" : "", g->nome);
    
    // Imprime vertices
    for(i=0; i < g->n_vertices; i++){
        printf("    \"%s\"\n", g->l_vert[i]);
    }
    printf("\n");


    // Imprime arestas
    const char *dir = g->eh_dir ? "->" : "--";
    for(j=0; j < g->n_ares; j++){
        printf("    \"%s\" %s \"%s\"", g->l_tail[j], dir, g->l_head[j]);
        if(g->l_peso[j] > 0.0) printf(" [peso=%f]", g->l_peso[j]);
        printf("\n");
    }

    printf("}\n");
*/
    return g;
}

//------------------------------------------------------------------------------
// devolve um grafo igual a g

grafo copia_grafo(grafo g) {
	
	if( g ) 
		 return g;
	else return NULL;

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

	lista l = NULL;
	
	if ( g && v && direcao )
		 return NULL;
	else return l;

}

//------------------------------------------------------------------------------
// devolve o grau do vértice v no grafo g
// 
// se direcao == 0, v é um vértice de um grafo não direcionado
//                  e a função devolve seu grau
//
// se direcao == -1, v é um vértice de um grafo direcionado
//                   e a função devolve seu grau de entrada
//
// se direcao == 1, v é um vértice de um grafo direcionado
//                  e a função devolve seu grau de saída

unsigned int grau(vertice v, int direcao, grafo g) {
	
	if ( g && v && direcao )
		 return 1;
	else return 0;
}

//------------------------------------------------------------------------------
// devolve 1, se o conjunto dos vertices em l é uma clique em g, ou
//         0, caso contrário
//
// um conjunto C de vértices de um grafo é uma clique em g 
// se todo vértice em C é vizinho de todos os outros vértices de C em g

int clique(lista l, grafo g) {
	
	if( g && l )
		 return 1;
	else return 0;

}


//------------------------------------------------------------------------------
// devolve 1, se v é um vértice simplicial em g, ou
//         0, caso contrário
//
// um vértice é simplicial no grafo se sua vizinhança é uma clique

int simplicial(vertice v, grafo g) {
	
	if( g && v ) 
		 return 1;
	else return 0;
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

	if( g )
		 return 1;
	else return 0;
}

