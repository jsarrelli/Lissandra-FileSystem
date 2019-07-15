/*
 * APIKernel.h
 *
 *  Created on: 20 jun. 2019
 *      Author: utnso
 */

#ifndef APIKERNEL_H_
#define APIKERNEL_H_

#include "Kernel.h"
#include <Sockets/Serializacion.h>
#include <Sockets/Conexiones.h>
#include <Libraries.h>
/*
 * Aca se almacenan las funciones de inicio del API Kernel
 */

int procesarInputKernel(char* linea);
int enviarInfoMemoria(int socketMemoria, char* request, t_protocolo protocolo, Paquete paquete);
int procesarAdd(int id, consistencia cons);
int consolaAdd(char*argumento);
int consolaInsert(char*request);
int consolaSelect(char*argumentos);
int consolaCreate(char*argumentos);
int consolaDescribe(char*nombreTabla);
int consolaDrop(char*nombreTabla);
int consolaRun(char*path);
int consolaJournal();
void consolaSalir(char*nada);

void imprimirMetrics();
void enviarJournalMemoria(int socketMemoria);
void mostrarMetadata(metadataTabla* metadataTabla);
t_metadata_tabla* deserealizarTabla(Paquete* paquete);
metadataTabla* deserealizarMetadata(char* metadataSerializada);
metadataTabla* newMetadata(char* nombreTabla, consistencia consistencia, int cantParticiones, int tiempoCompactacion);
consistencia getConsitenciaFromChar(char* consistenciaChar);
void agregarTabla(metadataTabla* tabla);

int procesarDescribe(int socketMemoria, char* nombreTabla);
consistencia getConsitenciaFromChar(char* consistenciaChar);
char* consistenciaToChar(consistencia consistencia);


void freeMetadata(metadataTabla* tabla);

#endif /* APIKERNEL_H_ */
