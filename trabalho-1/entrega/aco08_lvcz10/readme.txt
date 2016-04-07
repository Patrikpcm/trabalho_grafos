Trabalho 1 de algoritmos e teoria dos grafos:


	Participantes:
		Alexandre Calerio de Oliveira 		GRR20080568
		Leonardo Vincius Carvalho Zanella	GRR20103241


Estruturas implementada:

	Estrutura do grafo:

			char*	nome:  			Nome do grafo atribuido pela função agnameof().
			int		direcionado 	Indica se o grafo é direcionado atribuido pela função agisdirected().
			int		ponderado		Indica se o grafo é ponderado, atribuido caso uma aresta tenha a propriedade peso.
			int 	n_vertices		Número de vértices do grafo, atribuido pela função agnnodes() da cgraph.
			int 	n_arestas		Número de arestas do grafo, atribuido pela função agedges() da cgraph.
			lista 	vertices		estutura do tipo lista, onde os nós apontam para a estrutura do tipo vértice.

	Estrutura do vértice:

			char*	nome			Nome do vértice, atribuido pela função agnameof().
			lista	arestas_entrada	estrutura do tipo lista que possui apontadores para estrutura do tipo aresta.
			lista	arestas_saida	estrutura do tipo lista que possui apontadores para estrutura do tipo aresta.
			
	Estutura da aresta:

			long int peso:			Inteiro que recebe valor do peso da aresta.
			vertice	origem:			ponteiro do tipo vertice para o vértice de origem da aresta.
			vertice	destino:		ponteiro do tipo vertice para o vértice de destino da aresta.
			
Funções Extras implementadas:

	aresta	cria_aresta( lista lv, Agedge_t *e );

		Recebe o ponteiro do tipo Agedge_t e a lista de vertices do grafo.
		Procura os vértices(entrada,saida) utilizando a função busca_vertice para encontrar o apontador para os 
		vértices de origem e destino, verifica se existe peso na aresta.
		Retorna a aresta criada.
	
	aresta copia_aresta( aresta a, grafo ng );

		Recebe uma aresta a e um grafo.
		Aloca memoria para a estrutura atribui o valor de peso, busca os vertices de origem e destino e atribui 
		nos respectivos apontadores.
		Retorna uma mesma estrutura de aresta com os atributos da recebida.
	
	aresta busca_aresta( lista l, vertice origem, vertice destino );

		Recebe a lista de arestas e dois vertices, origem e destino.
		Procura na lista de aresta se o existe uma aresta que aponta para o destino e para a origem.
		Retorna ponteiro da aresta encontrada ou NULL.
	
	lista arestas( grafo g );

		Recebe um grafo g
		Devolve uma lista com todas as arestas do grafo g.
		
	vertice busca_vertice( lista l, char *nome );

		Recebe uma lista de vertices de um grafo e o nome do vértice.
		Retorna o ponteiro para o vertice encontrado ou NULL caso contrário.
		
	int destroi_aresta( void *ptr );

		Recebe o ponteiro para um aresta.
		Desaloca a estrutura da aresta.
		
	int destroi_vertice( void *ptr );

		Recebe o ponteiro para um vertice 
		Desaloca o vertice as listas de arestas 
		
		
	
Funções implementadas do trabalho:

	lista vizinhanca(vertice v, int direcao, grafo g);
	
		Varre os vértices do grafo g procurando por arestas nas listas de saída e entrada do vértice v 
		conforme critério de entrada e saída definido pelo parâmetro 'direção'.
		Devolve uma lista com os vértices encontrados na vizinhança de v.

	int clique(lista l, grafo g);
	
		Para cada vertice da lista l testa se seus vizinhos são vizinhos entre si.
		Caso afirmativo retorna 1 ou 0 caso contrario.

	int simplicial(vertice v, grafo g)

		Testa se a vizinhança do vertice v no grafo g é uma clique.
		Retorna 1 se afirmativo ou 0 caso contrário.
				
	int cordal(grafo g)
	
		A partir de uma cópia do grafo g:
		Remove um a um os vértices simpliciais juntamente com suas arestas.
		Ao se remover todos os vértices simpliciais a lista de vértices estiver vazia
		a função retorna 1, ou retorna 0 caso contrário.
		
