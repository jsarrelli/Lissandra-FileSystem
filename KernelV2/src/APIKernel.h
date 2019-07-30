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
int enviarInfoMemoria(int socketMemoria, char* request, t_protocolo protocolo, Paquete* paquete);
int procesarAdd(int id, t_consistencia consistencia);
int consolaAdd(char*argumento);
int consolaInsert(char*request);
int consolaSelect(char*argumentos);
int consolaCreate(char*argumentos);
int consolaDescribe(char*nombreTabla);
int consolaDrop(char*nombreTabla);
int consolaRun(char*path);
int consolaJournal();
void consolaSalir(char*nada);

void enviarJournalMemoria(int socketMemoria);
infoTabla* newInfoTabla(char* nombreTabla, t_consistencia consistencia);
infoTabla* deserealizarInfoTabla(char* tablaSerializada);
void agregarTabla(infoTabla* tabla);

int procesarDescribe(int socketMemoria, char* nombreTabla);

void freeInfoTabla(infoTabla* tabla);

#endif /* APIKERNEL_H_ */
