#ifndef MEMORIAPRINCIPAL_H_
#define MEMORIAPRINCIPAL_H_

#include <Libraries.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include <commons/string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>


int valueMaximo;

void* memoria;
t_list* segmentos;
t_list* memoriaStatus;// suerte de bitmap que guarda los frames disponibles de memoria

typedef struct Segmento{
	char* nombreTabla;
	char* nombreViejo;
	t_list* paginas;
	int nombreModificado;
}Segmento;

typedef struct Pagina{
	int modificado;
	t_registro* registro;
}Pagina;

typedef struct EstadoMemoria{
	int estado;
	int fechaObtencion;
}EstadoMemoria;

bool existePaginaEnMemoria(Segmento* segmento,int key);
bool existeSegmentoEnMemoria(char* nombreSegmento);

Segmento* buscarSegmento(char* nombreSegmento);
Segmento* buscarSegmentoEnMemoria(char* nombreSegmento);

Pagina* buscarPaginaEnMemoria(Segmento* segmento,int key);
Pagina* buscarPagina(Segmento* segmento,int key);

void insertarSegmentoEnMemoria(char* nombreSegmento);
Pagina* insertarPaginaEnMemoria(int key, char* value, char* nombreSegmento);

Pagina* crearPagina(int key, char*value);
bool memoriaLlena();
void* darMarcoVacio();
bool estaLibre(EstadoMemoria estado);
bool todosModificados();

void* liberarUltimoUsado();

#endif /*MEMORIAPRINCIPAL_H_*/
