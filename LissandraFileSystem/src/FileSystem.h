/*
 * FileSystem.h
 *
 *  Created on: 23 abr. 2019
 *      Author: utnso
 */

#ifndef FILESYSTEM_H_
#define FILESYSTEM_H_
#include <math.h>
#include <stdio.h>
#include <Libraries.h>
#include "LissandraFileSystem.h"
#include <string.h>

typedef struct {
	int BLOCK_SIZE;
	int BLOCKS;
	char* MAGIC_NUMBER;
} t_metadata;

typedef struct {
	char* Tablas;
	char* Bloques;
	char* Metadata;
	char* Bitmap;

} t_rutas;

t_rutas rutas;
t_metadata metadata;
t_bitarray *bitmap;

int cargarMetadata();
int leerMetadata();
void crearBloques();
void crearBitmap();
void leerBitmap();
void escribirBitmap();
void destruirBitmap();
t_list* buscarBloquesLibres(int cant);
void reservarBloque(int index);
void liberarBloque(int index);
void cargarMemtable();
t_semaforos_tabla* getSemaforoByTabla(char* nombreTabla);
void cargarSemaforosTabla(char* nombreTabla);
void freeSemaforoTabla(t_semaforos_tabla* semaforoTabla);
int getSizeOfFile(char* rutaArchivo);

#endif /* FILESYSTEM_H_ */
