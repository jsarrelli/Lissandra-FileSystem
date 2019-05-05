#include <Libraries.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include <commons/string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

int LIBRE = 1;
int OCUPADO = 0;

int tamanioMemoria=1024;
int tamanioPagina=124;
int cantPaginas;

int valueMaximo;

void* memoria;
t_list segmentos;
t_list estadoMemoria;

typedef struct Segmento{
	char* nombreTabla;
	char* nombreViejo;
	t_list paginas;
	int nombreModificado;
}Segmento;

typedef struct Pagina{
	int modificado;
	t_registro *registro;
}Pagina;
//
//bool existePagina(Segmento segmento,int key);
//bool existeSegmento(Segmento segmento);
//Segmento buscarSegmento(char* nombreSegmento);
//Pagina buscarPagina(Segmento segmento,int key);
//Segmento insertarSegmento(char* nombreSegmento);
//Pagina insertarPagina(int key, char* value);
