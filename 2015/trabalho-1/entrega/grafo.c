#include <stdlib.h>
#include <stdio.h>
#include <graphviz/cgraph.h>
#include "grafo.h"


//------------------------------------------------------------------------------
// estrutura de dados para representar um grafo simples
// 
// o grafo pode ser
// - direcionado ou não
// - com pesos nas arestas ou não
// 
// além dos vértices e arestas, o grafo tem um nome
// 
// os vértices do grafo também tem nome
// 
// os nomes do grafo e dos vértices são strings quaisquer
// 
// num grafo com pesos nas arestas todas as arestas tem peso, que é um double
// 
// o peso default de uma aresta é 0.0


struct grafo {
    char    *gnome;     // nome do grafo
    int      eh_dir;    // se eh direcionado
    int      n_vert;    // numero de vertices do grafo
    int      n_ares;    // numero de arestas do grado
    char   **l_tail;    // lista indexada de "tails" das arestas
    char   **l_head;    // lista indexada de "heads" das arestas
    double  *l_peso;    // lista indexada dos pesos das arestas
    char   **l_vert;    // lista dos vertices do grafo
};


//------------------------------------------------------------------------------
// lê um grafo no formato dot de input, usando as rotinas de libcgraph
// 
// desconsidera todos os atributos do grafo lido
// exceto o atributo "peso" nas arestas onde ocorra
// 
// num grafo com pesos nas arestas todas as arestas tem peso, que é um double
// 
// o peso default de uma aresta num grafo com pesos é 0.0
// 
// devolve o grafo lido ou
//         NULL em caso de erro 

grafo le_grafo(FILE *input) {

    Agraph_t *ag = agread(input, 0);
    if(!(ag && agisstrict(ag))) 
        return NULL;

    grafo g = malloc(sizeof(grafo));
    if( !g ) return NULL;
    
    g->gnome  = agnameof(ag);
    g->eh_dir = agisdirected(ag);
    g->n_vert = agnnodes(ag);
    g->n_ares = agnedges(ag);
    // ptr = malloc(0) prove um ponteiro valido e evita diversos
    // problemas como corrupcao de dados e problemas com free()
    g->l_tail = malloc(0);
    g->l_head = malloc(0);
    g->l_peso = malloc(0);
    g->l_vert = malloc(0);

    int i=0, j=0, k=0; 
    int avn=0, inlst=0;
    char *w;
    // Lista de arestas visitadas
    Agedge_t **avl;
    avl = malloc(((unsigned int)g->n_ares)*sizeof(Agedge_t *));

    g->l_tail = malloc(((unsigned int)g->n_ares)*sizeof(char *));
    if(!g->l_tail) return NULL;

    g->l_head = malloc(((unsigned int)g->n_ares)*sizeof(char *));
    if(!g->l_head) return NULL;

    g->l_peso = malloc(((unsigned int)g->n_ares)*sizeof(double));
    if(!g->l_peso) return NULL;

    g->l_vert = malloc(((unsigned int)g->n_vert)*sizeof(char *));
    if(!g->l_vert) return NULL;

    for(Agnode_t *v=agfstnode(ag); v; v=agnxtnode(ag,v)){
        g->l_vert[i] = agnameof(v); i+=1;
        // Grafos direcionados
        if( g->eh_dir ){
            for(Agedge_t *a=agfstout(ag,v); a; a=agnxtout(ag,a)){
                g->l_tail[j] = agnameof(agtail(a));
                g->l_head[j] = agnameof(aghead(a));
                w = agget(a,(char*)"peso");
                g->l_peso[j] = w ? strtod(w,NULL) : 0.0;
                j+=1;
            }
        }
        // Grafos nao direcionados
        else{
            for(Agedge_t *a=agfstedge(ag,v); a; a=agnxtedge(ag,a,v)){
                // Evita que uma aresta ja visitada seja guardada
                inlst=0;                
                for(k=0; k < avn; k++){
                    if(ageqedge(a, avl[k])){
                        inlst=1;                        
                        break;
                    }
                }
                if( !inlst ){
                    g->l_tail[j] = agnameof(agtail(a));
                    g->l_head[j] = agnameof(aghead(a));
                    w = agget(a,(char*)"peso");
                    g->l_peso[j] = w ? strtod(w,NULL) : 0.0;
                    // Guarda aresta visitada numa lista
                    avl[avn]=a;
                    avn+=1;   
                    j+=1;
                }
            }
        }
    }

    return g;
}



//------------------------------------------------------------------------------
// desaloca toda a memória utilizada em g
// 
// devolve 1 em caso de sucesso ou
//         0 em caso de erro

int destroi_grafo(grafo g) {

    if(!g) return 0;

    if(g->l_tail) free(g->l_tail);
    if(g->l_tail) return 0;
    if(g->l_head) free(g->l_head);
    if(g->l_head) return 0;
    if(g->l_peso) free(g->l_peso);
    if(g->l_peso) return 0;
    if(g->l_vert) free(g->l_vert);
    if(g->l_vert) return 0;
    if(g) free(g); 
    if(g) return 0;

    return 1;
}



//------------------------------------------------------------------------------
// escreve o grafo g em output usando o formato dot, de forma que
// 
// 1. todos os vértices são escritos antes de todas as arestas/arcos 
// 2. se uma aresta tem peso, este deve ser escrito como um atributo
//
// devolve o grafo escrito ou
//         NULL em caso de erro 

grafo escreve_grafo(FILE *output, grafo g) {

    int i,j;

    if(!(g && output)) return NULL;
    
    printf("strict %sgraph \"%s\" {\n\n", g->eh_dir ? "di" : "", g->gnome);
    
    // Imprime vertices
    for(i=0; i < g->n_vert; i++){
        printf("    \"%s\"\n", g->l_vert[i]);
    }
    printf("\n");


    // Imprime arestas
    const char *dir = g->eh_dir ? "->" : "--";
    for(j=0; j < g->n_ares; j++){
        printf("    \"%s\" %s \"%s\"", g->l_tail[j], dir, g->l_head[j]);
        if(g->l_peso[j] > 0.0) printf(" [peso=%f]", g->l_peso[j]);
        printf("\n");
    }

    printf("}\n");

    return g;
}

