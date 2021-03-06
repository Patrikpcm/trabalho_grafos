#include <stdio.h>
#include "lista.h"
#include "grafo.h"

//------------------------------------------------------------------------------

int main(void) {

	grafo g = le_grafo(stdin);

	if ( !g ) return 1;

	printf("nome: %s\n", nome_grafo(g));

	int d = direcionado(g);

	printf("%sdirecionado\n", d ? "" : "não ");
	
	printf("%sponderado\n", ponderado(g) ? "" : "não ");

	unsigned n = n_vertices(g);

	printf("%d vértices\n", n);
	
	printf("%d arestas\n",  n_arestas(g));

	escreve_grafo(stdout, g);

	if (!d)  printf("\n%s %s é cordal\n", nome_grafo(g), cordal(g) ? "" : "não");

	int destroi = destroi_grafo(g);

	printf("%s\n", (destroi ? "Grafo destuido" : "Falha ao destruir grafo") );

	return !destroi;
}
