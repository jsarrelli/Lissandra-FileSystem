/*
 * FileSystem.c
 *
 *  Created on: 23 abr. 2019
 *      Author: utnso
 */


/* Aqui desarrollaremos el sub modulo FileSystem del TP*/

/*#include "FileSystem.h"

int cargarMetadata(char*ruta){
	char*path;
	FILE* file = fopen(ruta, "wb+");
		t_configuracion_LFS* config = malloc(sizeof(t_configuracion_LFS));

		config->PUNTO_MONTAJE = config_get_string_value(file, "PUNTO_MONTAJE");

		path = string_new();
			string_append(&path,config->PUNTO_MONTAJE);
			string_append(&path,"/Metadata/Metadata.bin");
			rutas.Metadata = path;

			path = string_new();
			string_append(&path,config->PUNTO_MONTAJE);
			string_append(&path,"/Metadata/Bitmap.bin");
			rutas.Bitmap = path;

			path = string_new();
			string_append(&path,config->PUNTO_MONTAJE);
			string_append(&path,"/Tablas");
			rutas.Tablas = path;

			path = string_new();
			string_append(&path,config->PUNTO_MONTAJE);
			string_append(&path,"/Bloques/");
			rutas.Bloques = path;

		free(config);
		fclose(ruta);
		return 1;
}
int leerMetadata(){
	t_config* config = config_create(rutas.Metadata);
	if(config==NULL){
		printf("No se encontró el archivo Metadata en :%s\n", rutas.Metadata);
		return -1;
	}

	if (config_has_property(config, "CANTIDAD_BLOQUES")){
		metadata.BLOCKS = config_get_int_value(config,"BLOCKS");
		printf("CANTIDAD DE BLOQUES: %i\n", metadata.BLOCKS);
	}else{
		printf("No se encontró el parámetro CANTIDAD_BLOQUES dentro del archivo Metadata\n");
		return -1;
	}
	if (config_has_property(config, "TAMANIO_BLOQUES")){
		metadata.BLOCK_SIZE = config_get_int_value(config,"BLOCK_SIZE");
		printf("TAMANIO DE BLOQUES: %i\n", metadata.BLOCK_SIZE);
	}else{
		printf("No se encontró el parámetro TAMANIO_BLOQUES dentro del archivo Metadata\n");
		return -1;
	}
	if (config_has_property(config, "MAGIC_NUMBER")){
		metadata.MAGIC_NUMBER = config_get_string_value(config,"MAGIC_NUMBER");
		printf("MAGIC_NUMBER: %s\n", metadata.MAGIC_NUMBER);
	}else{
		printf("No se encontró el parámetro MAGIC_NUMBER dentro del archivo Metadata\n");
		return -1;
	}
	//Creo carpetas de Bloques y Tablas si es que no existen
	mkdir(rutas.Bloques, S_IRWXU);
	mkdir(rutas.Tablas, S_IRWXU);
	return 1;
}*/
