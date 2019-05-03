/*
 * LissandraFileSystem.h
 *
 *  Created on: 17 abr. 2019
 *      Author: utnso
 */

#ifndef LISSANDRAFILESYSTEM_H_
#define LISSANDRAFILESYSTEM_H_

#include "Libraries.h"

typedef struct{
	int PUERTO_ESCUCHA;
	char* PUNTO_MONTAJE;
	int RETARDO;
	int TAMANIO_VALUE;
	int TIEMPO_DUMP;
}t_configuracion_LFS;

typedef struct{
	char* CONSISTENCIA;
	int CANT_PARTICIONES;
	int T_COMPACTACION;
}t_metadata_tabla;

typedef struct{
	int TAMANIO;
	char **BLOQUES;
	int cantBloques;
}t_archivo;

t_log* logger;
t_log* loggerError;

t_configuracion_LFS* cargarConfig (char* ruta);

#endif /* LISSANDRAFILESYSTEM_H_ */
