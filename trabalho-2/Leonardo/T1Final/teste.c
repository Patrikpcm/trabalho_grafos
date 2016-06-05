#include <stdio.h>
#include "lista.h"
#include "grafo.h"

//------------------------------------------------------------------------------

int main(void) {

	grafo g = le_grafo(stdin);

	if ( !g ) return 1;

//	escreve_grafo(stdout, g);

//	grafo ng = copia_grafo(g);
//	escreve_grafo(stdout, ng);


//	lista lv = constroi_lista();
//	if( clique(lv, g) )
//		 printf("Eh Clique!!!\n");
//	else printf("Não é clique...\n");


//	if( destroi_grafo(ng) )
//		 printf("Destruido!!!\n");
//	else printf("Falhou!!!\n");


//	if( simplicial(v,g) )
//		 printf("Eh Simplicial!!!\n");
//	else printf("Não é simplicial...\n");


	printf("nome: %s\n",      nome_grafo(g));
	printf("%sdirecionado\n", direcionado(g) ? "" : "não ");
	printf("%sponderado\n",   ponderado(g) ? "" : "não ");
	printf("%d vértices\n",   n_vertices(g));
	printf("%d arestas\n",    n_arestas(g));
	//printf("%s é cordal\n",   cordal(g) ? "" : "não ");

	lista l = busca_largura_lexicografica(g);
	
	monta_vizinhos_a_direita(l,g);


	


	//escreve_grafo(stdout, g);
	
	

//	int r = destroi_grafo(g);
//	if(r) printf("Grafo destruido! \n");
//	else  printf("Falha ao destruir grafo \n");
//	return !r;

	return ! destroi_grafo(g);

//	return 0;
}
