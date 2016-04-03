#include <stdio.h>
#include "grafo.h"
#include "lista.h"

//------------------------------------------------------------------------------

int main(void) {

  grafo g = le_grafo(stdin);

  if ( !g )

    return 1;
	

		


  printf("nome: %s\n", nome_grafo(g));
  printf("%sdirecionado\n", direcionado(g) ? "" : "não ");
  printf("%sponderado\n", ponderado(g) ? "" : "não ");
  printf("%d vértices\n", n_vertices(g));
  printf("%d arestas\n", n_arestas(g));
  printf("%s é cordal\n", cordal(g) ? "" : "não ");
		
		
		
		
		escrevenos(g);
  
  void *teste="LIMEIRA";
  vertice aux =buscaVertice(teste,g);
  
  printf("achei esse: %s",nome_vertice(aux));

  return ! destroi_grafo(g);
}
