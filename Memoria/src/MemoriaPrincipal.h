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
#include "SocketClienteMemoria.h"
#include "EstructurasMemoria.h"



int valueMaximo;
t_log* logger;
void* memoria;

t_list* memoriaStatus; // suerte de bitmap que guarda los frames disponibles de memoria
int socketFileSystem;
int tamanioRegistro;

void inicializarMemoria(int valueMaximoRecibido, int tamanioMemoriaRecibido, int socketFileSystem);

Segmento* buscarSegmento(char* nombreSegmento);
Segmento* buscarSegmentoEnMemoria(char* nombreSegmento);

Pagina* buscarPaginaEnMemoria(Segmento* segmento, int key);
Pagina* buscarPagina(Segmento* segmento, int key);

Segmento* insertarSegmentoEnMemoria(char* nombreSegmento, t_metadata_tabla* metaData);
Pagina* insertarPaginaEnMemoria(int key, char* value, double timeStamp, Segmento* segmento);

bool memoriaLlena();
void* darMarcoVacio();
bool todosModificados();

void* liberarUltimoUsado();
t_list* obtenerSegmentosDeFileSystem();
EstadoFrame* getEstadoFrame(Pagina* pagina);

void eliminarSegmentoDeMemoria(Segmento* segmentoAEliminar);
void eliminarPaginaDeMemoria(Pagina* paginaAEliminar, Segmento* segmento);
void journalMemoria();

void freeSegmento(Segmento* segmentoAEliminar);

bool isModificada(Pagina* pagina);
bool existeSegmentoFS (Segmento* segmento);
bool validarValueMaximo(char* value);

#endif /*MEMORIAPRINCIPAL_H_*/
