
	Trabalho 2 de algoritmos e teoria dos grafos:


	Participantes:

		Alexandre Calerio de Oliveira 		GRR20080568
		Leonardo Vincius Carvalho Zanella	GRR20103241

	====================================================================
	==  Estruturas                                                    ==
	====================================================================

	Grafo:

			char*	nome:  			Nome do grafo atribuido pela função agnameof().
			int		direcionado 	Indica se o grafo é direcionado atribuido pela função agisdirected().
			int		ponderado		Indica se o grafo é ponderado, atribuido caso uma aresta tenha a propriedade peso.
			int 	n_vertices		Número de vértices do grafo, atribuido pela função agnnodes() da cgraph.
			int 	n_arestas		Número de arestas do grafo, atribuido pela função agedges() da cgraph.
			lista 	vertices		Estutura do tipo lista, onde os nós apontam para a estrutura do tipo vértice.

	Vértice:

			char*	nome			 Nome do vértice, atribuido pela função agnameof().
			lista	arestas_entrada	 Estrutura do tipo lista que possui apontadores para estrutura do tipo aresta.
			lista	arestas_saida	 Estrutura do tipo lista que possui apontadores para estrutura do tipo aresta.
			lista   vizinhos_direita Estrutura do tipo lista que possui apontadores para estrutura do tipo vértice.
			int*    label			 Rótulos dos vértice.
			int     visitado		 Flag usada na busca em largura.
			int     padding          Somente pra evitar warning na compilação.
			
	Aresta:

			long int peso:			Inteiro que recebe valor do peso da aresta.
			vertice	origem:			ponteiro do tipo vertice para o vértice de origem da aresta.
			vertice	destino:		ponteiro do tipo vertice para o vértice de destino da aresta.


	====================================================================
	==  Funções extras implementadas para o Trabalho 2:               ==
	====================================================================


	static void monta_vizinhos_a_direita(lista l);

		Retorna uma lista com os vizinhos do próximo vértice da lista
		que não possuem o status de 'visitado'.


	static vertice vertice_maior_label(lista l);
	
		Busca na lista 'l' o vertice com o maior label lexicografico.


	static int label_maior(vertice v, vertice x, int tamanho);
	
		Compara os rótulos de dois vértices, 'v' e 'x'.
		Retorna 0 se o label de 'v' é maior que o label de 'x' ou 
		retorna 1 caso contrário.


	static void adiciona_label(vertice v, int valor);
	
		Insere um rótulo 'valor' à lista de rótulos do vértice 'v'.


	static int vizinhos_v2_contem_v1(vertice v1, vertice v2);

		Compara a vizinhança de dois vértices. Se a vizinhança do vértice
		'v2' está contida na vizinhança do vértice 'v1' retorna 1 ou 
		retorna 0 caso contrário.


	static void desvisita_vertices(lista l);
		
		Atribui o status de 'não visitado' para todos os vértices da lista l.


	static int v_esta_na_lista_vizinhos(vertice v, lista l);

		Verifica se o vértice 'v' está contido na lista 'l'.
		Retorna 1 caso afirmativo e 0 caso contrário.
		
	
	====================================================================		
	==  Funções requeridas para o Trabalho 2:                         ==
	====================================================================

	lista busca_largura_lexicografica(grafo g);

		Aloca estutura para os rótulos dos vértices, escreve 0 em cada posição
		da estrutura e seta estado de 'não visitado' nos flags dos vértices.
		Realiza uma busca em largura no grafo g. 
		A partir do vértice corrente 'v' e o grafo g possuindo 'n' vértices 
		ainda não visitados (v incluso) atribui o rótulo 'n' a 'v' e 'n-1' 
		aos vizinhos de 'v' mantendo a lista de retorno ordenada de tal forma
		que o vértice com maior rótulo fique no início da lista e o de menor
		fique no final.
		
	
	int ordem_perfeita_eliminacao(lista l, grafo g);
			
		Para todo elemento da lista 'l' verifica se a vizinhança do vértice
		corrente 'u' está contida na vizinhança do vértice 'v', próximo de
		'u' na lista. Se verdadeiro para todo vértice 'u' da lista, retorna 1
		ou retorna 0 caso contrário.
	
	
	int cordal(grafo g);
	
		Se o grafo g não é direcionado e a busca lexicográfica nos vértices
		de g representa uma ordem perfeita de eliminação, retorna 1 ou
		retorna 0 caso contrário.
