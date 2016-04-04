#include <stdio.h>
#include "grafo.h"

//------------------------------------------------------------------------------

int main(void) {

	grafo g = le_grafo(stdin);

	if ( !g ) return 1;

	grafo ng = copia_grafo(g);
	escreve_grafo(stdout, ng);

//	if( destroi_grafo(ng) )
//		 printf("Destruido!!!\n");
//	else printf("Falhou!!!\n");

//	printf("nome: %s\n", nome_grafo(g));
//	printf("%sdirecionado\n", direcionado(g) ? "" : "não ");
//	printf("%sponderado\n", ponderado(g) ? "" : "não ");
//	printf("%d vértices\n", n_vertices(g));
//	printf("%d arestas\n", n_arestas(g));
//	printf("%s é cordal\n", cordal(g) ? "" : "não ");

//	escreve_grafo(stdout, g);

//	return ! destroi_grafo(g);
	return 0;
}
