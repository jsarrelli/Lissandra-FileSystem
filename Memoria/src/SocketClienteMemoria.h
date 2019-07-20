#ifndef SOCKETCLIENTEMEMORIA_H_
#define SOCKETCLIENTEMEMORIA_H_

#include <Libraries.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include <commons/string.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include "EstructurasMemoria.h"
#include <Sockets/Serializacion.h>
#include <Sockets/Conexiones.h>
#include "Memoria.h"

t_metadata_tabla* describeSegmento(char* nombreSegmento);
void enviarRegistroAFileSystem(Pagina* pagina, char* nombreSegmento);
int eliminarSegmentoFileSystem(char* nombreSegmento);
int enviarCreateAFileSystem(t_metadata_tabla* metadata, char* nombreTabla);
char* describeAllFileSystem();
int HandshakeInicial();
void intercambiarTablasGossiping(t_memoria* memoria);

void enviarTablaGossiping(int socketMemoriaDestino);
void procesarRequestTABLA_GOSSIPING(int socketKernel);
t_registro* selectFileSystem(Segmento* segmento, int key);
bool isMemoriaCaida(t_memoria* memoria);
#endif /*SOCKETCLIENTEMEMORIA_H_*/
