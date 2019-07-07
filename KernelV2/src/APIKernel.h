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

void procesarInput(char* linea);
void enviarInfoMemoria(int socketMemoria, char request[], t_protocolo protocolo);
void enviarJournalMemoria(int socketMemoria);
void procesarAdd(int id, consistencia cons);
void consolaAdd(char*argumento);
void consolaInsert(char*request);
void consolaSelect(char*argumentos);
void consolaCreate(char*argumentos);
void consolaDescribe(char*nombreTabla);
void consolaDrop(char*nombreTabla);
void consolaRun(char*path);
void mostrarMetadata(char* nombreSegmento, t_metadata_tabla* metadata);
t_metadata_tabla* deserealizarTabla(Paquete* paquete);


void consolaExecute();

#endif /* APIKERNEL_H_ */
