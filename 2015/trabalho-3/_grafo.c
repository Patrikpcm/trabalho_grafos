#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <graphviz/cgraph.h>
#include "grafo.h"

//------------------------------------------------------------------------------
/* Funções auxiliares que não estão em 'grafo.h' */



//------------------------------------------------------------------------------

struct grafo {
    char	 	 *gnome;	 // nome do grafo
    int		 	 eh_dir;	 // 1 -> se é direcionado;  0 -> caso contrário
    int		 	 conexo;	 // 1 -> conexo;            0 -> caso contrário
    int		 	 fconexo;	 // 1 -> fortemente conexo; 0 -> caso contrário
    int		 	 c_peso;	 // 1 -> arestas tem peso;  0 -> caso contrário
    int 		 n_vert;	 // número de vertices do grafo
    int			 n_ares;	 // número de arestas do grafo	
    long int 	 diam;	 	 // diâmetro do grafo
    char	 	 **vlst;	 // lista dos vértices da matriz 
    double	 	 **mat;   	 // matriz de adjacência com pesos
    unsigned int **mbool;	 // matriz de adjacências 'booleana'
    unsigned int **mdist;	 // matriz de adjacências com distâncias
};

//------------------------------------------------------------------------------

const long int infinito = LONG_MAX;

/* Funçoes auxiliares */

/* Multiplica matrizes: M3 = M1 X M2 */
void mult_mat(int, int**, int**, int**);

/* Compara duas matrizes: 1 => iguais; 0 cc */
int compara_mat(int, int**, int**);

/* Copia conteúdo da matriz M2 para matriz M1 */
void copia_mat(int, int**, int**);

/* Preenche matriz de adjacência booleana do struct grafo */
void get_bool_mat(grafo);

/* Obtém a matriz de adjacência com distâncias do struct grafo */
void get_dist_mat(grafo);

/* Retorna a posição de um string num vetor de strings */
int is_nodeinlist(grafo, char*, int);

//==============================================================================

void mult_mat(int n, int **mat1, int **mat2, int **res){
	
	int i, j, k, r;
	for(i=0; i < n; i++){
		for(j=0; j < n; j++){
			r = 0;
			for(k=0; k < n; k++)
				r += mat1[i][k]+mat2[k][j];
			res[i][j] = (r > 0 ? 1 : 0);
		}
	}
	return;
}

//------------------------------------------------------------------------------

int compara_mat(int n, int **mat1, int **mat2){
	
	int i, j;
	for( i=0; i < n; i++ ){
		for( j=0; j < n; j++ ){
			if( mat1[i][j] != mat2[i][j] )
				return 0;
		}
	}
	return 1;
}

//------------------------------------------------------------------------------

void copia_mat(int n, int **mat1, int **mat2){
	
	int i, j;
	for( i=0; i < n; i++ )
		for( j=0; j < n; j++ )
			mat1[i][j] = mat2[i][j];
	return;
}

//------------------------------------------------------------------------------

void get_bool_mat(grafo g){
	
	if( !g->mat )  return;
	if( g->mbool ) return;
	
	int i, j;
	
	g->mbool = malloc(g->n_vert*sizeof(int *));
	for(i=0; i < g->n_vert; i++)
		g->mbool[i] = malloc(g->n_vert*sizeof(int));
	
	for(i=0; i < g->n_vert; i++)
		for(j=0; j < g->n_vert; j++)
			g->mbool[i][j] = g->mat[i][j] > 0.0 ? 1 : 0;

	return;
}

//------------------------------------------------------------------------------

void get_dist_mat(grafo g){
	
	if( !g->mat ) return;
	if( g->mdist ) return;
	if( !g->mbool ) get_bool_mat(g);
	
	int i, j, k;
	
	g->mdist = malloc(g->n_vert*sizeof(int *));
	
	int **maux1 = malloc(g->n_vert*sizeof(int *));
	int **maux2 = malloc(g->n_vert*sizeof(int *));
	
	for(i=0; i < g->n_vert; i++){
		g->mdist[i] = malloc(g->n_vert*sizeof(int));
		maux1[i] = malloc(g->n_vert*sizeof(int));
		maux2[i] = malloc(g->n_vert*sizeof(int));
	}

	copia_mat(g->n_vert, maux2,    g->mbool);
	copia_mat(g->n_vert, g->mdist, g->mbool);

	i = 1;

	do{
		i += 1;
		
		copia_mat(g->n_vert, maux1, maux2);
		
		mult_mat(g->n_vert, g->mbool, maux1, maux2);

		if( g->eh_dir ){
			
			for(j=0; j < g->n_vert; j++)
				for(k=0; k < g->n_vert; k++)
					if((j != k) && (maux2[j][k] && !g->mdist[j][k]))
						g->mdist[j][k] = i;
		}
		else{
			for(j=0; j < g->n_vert-1; j++)
				for(k=j+1; k < g->n_vert; k++)
					if( maux2[j][k] && !g->mdist[j][k] )
						g->mdist[j][k] = i;
		}

	} while(!compara_mat(g->n_vert, maux1, maux2) && (i < (g->n_vert-1)));

	for(i=0; i < g->n_vert; i++){
		free(maux1[i]);
		free(maux2[i]);
	}
	free(maux1);
	free(maux2);
	
	return;
}

//------------------------------------------------------------------------------

int is_nodeinlist(grafo g, char *nm, int n){

	int x=0;
	while( x < n ){
		if(!strcmp(g->vlst[x++],nm))
			return x-1;
	}
	return -1;
}

//===============================================================================

grafo le_grafo(FILE *input) {
	
	if( !input ) return NULL;
	
	int    i, j, vn=0, an=0;
	char  *w;
	grafo  g;
	
    Agraph_t *ag = agread(input, 0);
    if(!(ag && agisstrict(ag))) 
        return NULL;
        
 	/* Aloca memória para a estrutura do grafo
 	*/
	g = malloc(sizeof(struct grafo));
	
	if( !g ) return NULL;
	
    /* Inicializa variáveis de g com dados do grafo
    */
    g->gnome  = agnameof(ag);
    g->eh_dir = agisdirected(ag);
    g->n_vert = agnnodes(ag);
    g->n_ares = agnedges(ag);
    
    /* Inicializa variáveis de g com 'valores padrão'
    */
    g->diam    = -1;
    g->conexo  = -1;
    g->fconexo = -1;
    g->c_peso  = -1;
    g->mbool   = NULL;
    g->mdist   = NULL;

    /* Aloca memória para lista de vértices 
    */
    g->vlst = malloc(g->n_vert*sizeof(char *));
    if( !g->vlst ) return NULL;
    
    /* Aloca memória para a matriz de pesos
    */
    g->mat  = malloc(g->n_vert*sizeof(double *));
    if( !g->mat ) return NULL;
    
    for(i=0; i < g->n_vert; i++){
		g->mat[i] = malloc(g->n_vert*sizeof(double));
		if( !g->mat[i] ) return NULL;
		
		for(j=0; j < g->n_vert; j++){
			g->mat[i][j] = 0.0;
		}
    }
    
	/* Lista de arestas ou arcos percorridos
	*/
	Agedge_t **ax = malloc(g->n_ares*sizeof(Agedge_t *));

    for(Agnode_t *v=agfstnode(ag); v; v=agnxtnode(ag,v)){
		
        /* Percorre os arcos do vértice 'v'
        */
         for(Agedge_t *a=agfstout(ag,v); a; a=agnxtout(ag,a)){
		
		/* Percorre as arestas do vértice 'v' 
		*/
		// for(Agedge_t *a=agfstedge(ag,v); a; a=agnxtedge(ag,a,v)){

			//if(!is_edgeinlist(ax, a, an)){
				ax[an] = a; an++;
				
				/* Processa o vértice 'v' da aresta {v,u} 
				*/
				if((i = is_nodeinlist(g,agnameof(agtail(a)),vn)) < 0 ){
					i = vn++;
					g->vlst[i]=agnameof(agtail(a));
				}
				/* Processa o vértice 'u' da aresta {v,u}
				*/
				if((j = is_nodeinlist(g,agnameof(aghead(a)),vn)) < 0 ){
					j = vn++;
					g->vlst[j]=agnameof(aghead(a));
				}
				/* Processa o peso das arestas, se existir
				*/
				if((int)(w = agget(a,(char*)"peso"))){
					g->mat[i][j] = strtod(w, NULL);
					g->c_peso    = 1;
				}
				else{
					g->mat[i][j] = 1.0;
					g->c_peso    = 0;
				}
			//}
        }
    }

    free(ag);
    free(ax);
 
 	return g;
}

//------------------------------------------------------------------------------

int destroi_grafo(grafo g) {

	if( !g->mat && !g->vlst )
		return 1;

	int i;
	
	/* Lista de vértices 
	*/
	free(g->vlst);
	g->vlst = NULL;
	if( g->vlst ) return 0;

	/* Matriz de adjacência com pesos 
	*/
	for(i=0; i < g->n_vert; i++){
		free(g->mat[i]);
		g->mat[i] = NULL;
		if(g->mat[i]) return 0;
	}
	free(g->mat);
	g->mat = NULL;
	if( g->mat ) return 0;
	
	/* Matriz de adjacência 'booleana'
	*/
	if( g->mbool ){
		for(i=0; i < g->n_vert; i++){
			free(g->mbool[i]);
			g->mbool[i] = NULL;
			if(g->mbool[i]) return 0;
		}
		free(g->mbool);
		g->mbool = NULL;
	}
	
	/* Matriz de adjacência com distâncias
	 * entre os vértices
	*/
	if( g->mdist ){
		for(i=0; i < g->n_vert; i++){
			free(g->mdist[i]);
			g->mdist[i] = NULL;
			if(g->mdist[i]) return 0;
		}
		free(g->mdist);
		g->mdist = NULL;
	}
	
	/* Grafo g 
	*/
	free(g);
	g = NULL;
	if(g) return 0;
		
	return 1;
}

//------------------------------------------------------------------------------

grafo escreve_grafo(FILE *output, grafo g) {
	
	if(!(g && output)) return NULL;

	int i, j;
	char dir = g->eh_dir ? '>' : '-';
	
	printf("strict %sgraph \"%s\" {\n\n",g->eh_dir ?"di":"",g->gnome);

    /* Imprime vertices 
    */
    for(i=0; i < g->n_vert; i++){
        printf("    \"%s\"\n", g->vlst[i]);
    }
    printf("\n");
    
	/* imprime arestas (arcos)
	*/
	for(i=0; i < g->n_vert; i++){
		for(j=0; j < g->n_vert; j++){
			if( g->mat[i][j] > 0.0 ){
				printf("    \"%s\" -%c \"%s\"",g->vlst[i],dir,g->vlst[j]);
				if( g->c_peso ) 
					printf(" [peso=%d]", (int)g->mat[i][j]);
				printf("\n");
			}
		}
	}
	printf("}\n");
	
	return g;
}

//------------------------------------------------------------------------------

char *nome(grafo g) {

	/* Nome do grafo lido em 'le_grafo()'
	 * e armazenano no 'struct grafo'.     
	*/
	return g ? g->gnome : "";
}

//------------------------------------------------------------------------------

unsigned int n_vertices(grafo g) {

	/* Número de vértices lido em 'le_grafo()'
	 * e armazenano no 'struct grafo'.         
	*/
	return g ? (unsigned int)g->n_vert : 0;
}

//------------------------------------------------------------------------------

int direcionado(grafo g) {

	/* Informação lida em 'le_grafo()'
	 * e armazenano no 'struct grafo'.
	*/
	return g ? g->eh_dir : 0;
}

//------------------------------------------------------------------------------

int conexo(grafo g) {
	
	if( g->eh_dir ) return 0;
	
	/* Retorna se a conectividade do grafo já
	 * foi definida.
	*/
	if( g->conexo >= 0 )
		return g->conexo;

	int i, j;

	/* Utiliza-se da matriz de adjacência boolena 
	*/
	if( !g->mbool ) get_bool_mat(g);

	/* Varre a parte superior em relação a diagonal
	 * principal da matriz 'booleana'. Se alguma linha
	 * apresente somente '0's significa um corte e
	 * portanto o grafo é desconexo. 
	*/ 
	for(i=0; i < g->n_vert-1; i++){
		g->conexo = 0;
		for(j=i+1; j < g->n_vert; j++)
			g->conexo = g->conexo || g->mbool[i][j];
		if(!g->conexo) 
			break;
	}
	
	return g->conexo;
}

//------------------------------------------------------------------------------

int fortemente_conexo(grafo g)  {

	if( !g->eh_dir ) return 0;
	
	/* Retorna se a conectividade do grafo já
	 * foi definida.
	*/
	if( g->fconexo >= 0 )
		return g->fconexo;
	
	int i, j;

	/* Utiliza-se da matriz de adjacência contendo
	 * as distâncias entre os vértices. 
	*/
	if( !g->mdist ) get_dist_mat(g);
	
	/* Se existir caminho (u, v) e (v, u) 
	 * na matriz de distâncias, o grafo é
	 * fortemente conexo.
	*/
	for(i=0; i < g->n_vert; i++){
		for(j=0; j < g->n_vert; j++){
			if((i != j) && g->mdist[i][j]){
				if( !g->mdist[j][i] ){
					g->fconexo = 0;
					return g->fconexo;
				}
			}
		}
	}
	g->fconexo = 1;	
	return g->fconexo;
}

//------------------------------------------------------------------------------

long int diametro(grafo g) {

	if( !g ) return infinito;
	
	/* O diâmetro do grafo é a maior distância
	 * entre dois de seus vértices. Se o grafo 
	 * é desconexo ou no caso de direcionado,
	 * não fortemente conexo, o diâmetro é infinito.
	*/
	if( g->eh_dir ){
		if( g->fconexo < 0 ) fortemente_conexo(g);
		if( !g->fconexo ) return infinito;
	}
	else{
		if( g->conexo < 0 ) conexo(g);
		if( !g->conexo ) return infinito;
	}
	 
	/* Retorna o diâmetro se este já foi calculado.
	*/
	if( g->diam >= 0 ) return g->diam;
	
	int i, j;
	
	/* Matriz com distâncias entre vértices
	*/
	if( !g->mdist ) get_dist_mat(g);

	/* Varre a matriz de distâncias procurando 
	 * pela maior distância entre dois vértices.
	*/
	for(i=0; i < g->n_vert; i++)
		for(j=0; j < g->n_vert; j++)
			if(((long int)g->mdist[i][j]) > g->diam)
				g->diam = (long int)g->mdist[i][j];
	
	return g->diam;
}

//------------------------------------------------------------------------------

grafo distancias(grafo g) {
	
	/* Matriz com distâncias entre vértices
	*/
	if( !g->mdist ) get_dist_mat(g);
	
	int i, j;
	
	/* Substitui o peso das aresta pelas distância
	 * entre os vértices da aresta.
	*/
	for(i=0; i < g->n_vert; i++)
		for(j=0; j < g->n_vert; j++){
			g->mat[i][j] = (double)g->mdist[i][j];
		}
		
	g->c_peso = 1;

	return g;
}

//------------------------------------------------------------------------------
