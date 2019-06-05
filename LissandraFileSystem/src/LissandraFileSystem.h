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
#include "Funciones/Conexiones.h"
#include "Funciones/Serializacion.h"

typedef struct{
	char* PUERTO_ESCUCHA;
	char* PUNTO_MONTAJE;
	int RETARDO;
	int TAMANIO_VALUE;
	int TIEMPO_DUMP;
}t_configuracion_LFS;

typedef struct{
	char tabla[100];
	t_list* registros;
}t_tabla_memtable;

typedef struct{
	int TAMANIO;
	char **BLOQUES;
	int cantBloques;
}t_archivo;

t_list* memtable;
t_log* logger;
t_log* loggerError;
t_configuracion_LFS* config;
t_configuracion_LFS* cargarConfig (char* ruta);

#endif /* LISSANDRAFILESYSTEM_H_ */
