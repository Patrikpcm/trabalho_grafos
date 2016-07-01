#include <stdio.h>
#include "grafo.h"

//------------------------------------------------------------------------------

int main(void) {

	grafo g = le_grafo(stdin);

	if ( !g ) return 1;

	printf("\nGrafo lido:\n");
	printf("nome: %s\n",       nome_grafo(g));
	printf("%s direcionado\n", direcionado(g) ? "" : "não");
	printf("%s ponderado\n",   ponderado(g)   ? "" : "não");
	printf("%d vértices\n",    n_vertices(g));
	printf("%d arestas\n",     n_arestas(g));
	escreve_grafo(stdout, g);
	
	grafo e = emparelhamento_maximo(g);
	printf("\nEmparelhamento Máximo:\n");
	
	printf("nome: %s\n",       nome_grafo(e));
	printf("%s direcionado\n", direcionado(e) ? "" : "não");
	printf("%s ponderado\n",   ponderado(e)   ? "" : "não");
	printf("%d vértices\n",    n_vertices(e));
	printf("%d arestas\n",     n_arestas(e));
	printf("\n");
	
	escreve_grafo(stdout, e);

	return ! (destroi_grafo(e) && destroi_grafo(g));
	
	return !destroi_grafo(g);
}
