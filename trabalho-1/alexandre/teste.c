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



	if( destroi_grafo(g) )
		 printf("Destruido!!!\n");
	else printf("Falhou!!!\n");

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
