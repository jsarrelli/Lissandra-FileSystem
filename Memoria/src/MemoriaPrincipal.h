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
	char nombreTabla[20];
	char nombreViejo[20];
	t_list* paginas;
	int nombreModificado;
	t_metadata_tabla* metaData;
}Segmento;

typedef struct Pagina{
	int modificado;
	t_registro* registro;
}Pagina;

typedef struct EstadoFrame{
	int estado;
	double fechaObtencion;
}EstadoFrame;

void inicializarMemoria(int valueMaximoRecibido, int tamanioMemoriaRecibido);

bool existePaginaEnMemoria(Segmento* segmento,int key);
bool existeSegmentoEnMemoria(char* nombreSegmento);

Segmento* buscarSegmento(char nombreSegmento[20]);
Segmento* buscarSegmentoEnMemoria(char nombreSegmento[20]);

Pagina* buscarPaginaEnMemoria(Segmento* segmento,int key);
Pagina* buscarPagina(Segmento* segmento,int key);

Segmento* insertarSegmentoEnMemoria(char nombreSegmento[20], t_metadata_tabla* metaData);
Pagina* insertarPaginaEnMemoria(int key, char value[112], Segmento* segmento);
void eliminarSegmento(char nombreTabla[20]);

Pagina* crearPagina(int key, char value[100]);
bool memoriaLlena();
void* darMarcoVacio();
bool estaLibre(EstadoFrame* estado);
bool todosModificados();

void* liberarUltimoUsado();
t_list* obtenerSegmentosDeFileSystem();
EstadoFrame* getEstadoFrame(Pagina* pagina);

void eliminarSegmento(char nombreSegmento[20]);
void eliminarPaginaDeMemoria(Pagina* pagina);

#endif /*MEMORIAPRINCIPAL_H_*/
