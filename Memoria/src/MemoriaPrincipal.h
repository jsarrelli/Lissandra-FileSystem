#ifndef MEMORIAPRINCIPAL_H_
#define MEMORIAPRINCIPAL_H_

#include <Libraries.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include <commons/string.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include "Parser.h"
#include "Sockets/Conexiones.h"
#include "Sockets/Serializacion.h"

int valueMaximo;
t_log* logger;
void* memoria;
t_list* segmentos;
t_list* memoriaStatus; // suerte de bitmap que guarda los frames disponibles de memoria
int socketFileSystem;
int tamanioRegistro;

typedef struct Segmento {
	char* nombreTabla;
	char* nombreViejo;
	t_list* paginas;
	int nombreModificado;
	t_metadata_tabla* metaData;
} Segmento;

typedef enum{
	NO_MODIFICADO = 0,
	MODIFICADO = 1
}t_modificado;

typedef struct Pagina {
	t_modificado modificado;
	t_registro* registro;
} Pagina;

typedef enum {
	LIBRE = 0,
	OCUPADO
} t_estado;


typedef struct EstadoFrame {
	t_estado estado;
	double fechaObtencion;
} EstadoFrame;

void inicializarMemoria(int valueMaximoRecibido, int tamanioMemoriaRecibido, int socketFileSystem);

Segmento* buscarSegmento(char* nombreSegmento);
Segmento* buscarSegmentoEnMemoria(char* nombreSegmento);

Pagina* buscarPaginaEnMemoria(Segmento* segmento, int key);
Pagina* buscarPagina(Segmento* segmento, int key);

Segmento* insertarSegmentoEnMemoria(char* nombreSegmento, t_metadata_tabla* metaData);
Pagina* insertarPaginaEnMemoria(int key, char value[112], double timeStamp, Segmento* segmento);

bool memoriaLlena();
void* darMarcoVacio();
bool todosModificados();

void* liberarUltimoUsado();
t_list* obtenerSegmentosDeFileSystem();
EstadoFrame* getEstadoFrame(Pagina* pagina);

void eliminarSegmentoDeMemoria(Segmento* segmentoAEliminar);
void eliminarSegmentoFileSystem(char* nombreTabla);
void eliminarPaginaDeMemoria(Pagina* paginaAEliminar, Segmento* segmento);
void journalMemoria();

t_list* obtenerPaginasModificadas();
#endif /*MEMORIAPRINCIPAL_H_*/
