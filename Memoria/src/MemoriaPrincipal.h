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
#include <math.h>

void* memoria;

t_list* memoriaStatus; // suerte de bitmap que guarda los frames disponibles de memoria
int valueMaximo;
int socketFileSystem;
int tamanioRegistro;
pthread_mutex_t mutexJournal;
pthread_mutex_t mutexAsignacionMarco;
pthread_mutex_t mutexLRU;
pthread_mutex_t mutexSelect;
pthread_mutex_t mutexInsert;
void inicializarMemoria(int tamanioMemoriaRecibido);

Segmento* buscarSegmento(char* nombreSegmento);
Segmento* buscarSegmentoEnMemoria(char* nombreSegmento);

Pagina* buscarPaginaEnMemoria(Segmento* segmento, int key);
Pagina* buscarPagina(Segmento* segmento, int key);

Segmento* insertarSegmentoEnMemoria(char* nombreSegmento);
Pagina* insertarPaginaEnMemoria(int key, char* value, double timeStamp, Segmento* segmento, bool modificado);

bool memoriaLlena();
void* darMarcoVacio();
bool todosModificados();

void liberarUltimoUsado();
t_list* obtenerSegmentosDeFileSystem();
EstadoFrame* getEstadoFrame(Pagina* pagina);

void eliminarSegmentoDeMemoria(Segmento* segmentoAEliminar);
void eliminarPaginaDeMemoria(Pagina* paginaAEliminar);
void reemplazarPagina(Pagina* pagina, Segmento* segmento);
void clearFrameDePagina(Pagina* paginaAEliminar);
void journalMemoria();

void freeSegmento(Segmento* segmentoAEliminar);

bool isModificada(Pagina* pagina);
bool existeSegmentoFS(char* nombreSegmento, t_list* tablas);
bool validarValueMaximo(char* value);

Segmento* newSegmento(char* nombreSegmento);
void vaciarMemoria();
void finalizarMemoria();

t_list* obtenerTablasFileSystem();

#endif /*MEMORIAPRINCIPAL_H_*/
