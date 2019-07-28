/*
 * LissandraFileSystem.h
 *
 *  Created on: 17 abr. 2019
 *      Author: utnso
 */

#ifndef LISSANDRAFILESYSTEM_H_
#define LISSANDRAFILESYSTEM_H_

#include <Libraries.h>
#include "SocketServidor.h"
#include <Sockets/Serializacion.h>
#include <Sockets/Conexiones.h>
#include <commons/config.h>
#include <semaphore.h>
#include "EstructurasFileSystem.h"
#include "FileSystem.h"
#include "funcionesLFS.h"
#include "ApiLFS.h"
#include "AdministradorConsultasLFS.h"
#include "Inotify.h"


pthread_t dumpThread;
pthread_t serverThread;

t_configuracion_LFS* config;
bool finProceso;


void cargarConfig();
void freeConfig();
void iniciarSocketServidor();


#endif /* LISSANDRAFILESYSTEM_H_ */
