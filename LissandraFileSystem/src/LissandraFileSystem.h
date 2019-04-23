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
	int TAMANIO_BLOQUES;
	int CANTIDAD_BLOQUES;
	char* MAGIC_NUMBER;
}t_metadata;

typedef struct{
	char* Tablas;
	char* Bloques;
	char* Metadata;
	char* Bitmap;

}t_rutas;

t_rutas rutas;
t_metadata metadata;
t_bitarray *bitmap;

t_log* logger;
t_log* loggerError;

#endif /* LISSANDRAFILESYSTEM_H_ */
