/*
 * LissandraFileSystem.h
 *
 *  Created on: 17 abr. 2019
 *      Author: utnso
 */

#ifndef LISSANDRAFILESYSTEM_H_
#define LISSANDRAFILESYSTEM_H_

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>

#include <sys/socket.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>

#include <commons/config.h>
#include <commons/string.h>
#include <commons/bitarray.h>
#include <commons/log.h>

typedef struct{
	int PUERTO_ESCUCHA;
	char* PUNTO_MONTAJE;
	int RETARDO;
	int TAMANIO_VALUE;
	int TIEMPO_DUMP;
}t_configuracion;

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
t_configuracion config;
t_metadata metadata;
t_bitarray *bitmap;

#endif /* LISSANDRAFILESYSTEM_H_ */
