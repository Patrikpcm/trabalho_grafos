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
		
		
	lista l= constroi_lista();
  
  void *teste="a";
  vertice aux =buscaVertice(teste,g);
  
  no d1=insere_lista(aux,l);
  
  teste="c";
  vertice aux2 =buscaVertice(teste,g);
  no d2=insere_lista(aux2,l);
  
  teste="d";
  vertice aux3 =buscaVertice(teste,g);
   no d3=insere_lista(aux3,l);
   teste="e";
  vertice aux4 =buscaVertice(teste,g);
   no d4=insere_lista(aux4,l);
  
   
  int isClique =clique(l,g);
  
  

  
  
 

  return ! destroi_grafo(g);
}
