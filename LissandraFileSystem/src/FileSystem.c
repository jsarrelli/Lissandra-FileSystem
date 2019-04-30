/*
 * FileSystem.c
 *
 *  Created on: 23 abr. 2019
 *      Author: utnso
 */


/* Aqui desarrollaremos el sub modulo FileSystem del TP*/

#include "FileSystem.h"

int cargarMetadata(t_configuracion_LFS* config){
	char*path;
	t_config* configFs = config_create("/home/utnso/tp-2019-1c-Los-Sisoperadores/LissandraFileSystem/fsConfig.cfg");

		config->PUNTO_MONTAJE = config_get_string_value(configFs, "PUNTO_MONTAJE");
		mkdir(config->PUNTO_MONTAJE, 0777);
		path = string_new();
			string_append(&path,config->PUNTO_MONTAJE);
			string_append(&path,"/Metadata");
			mkdir(path, 0777);
			string_append(&path,"/Metadata.bin");
			rutas.Metadata = path;
			FILE*arch1 = fopen(rutas.Metadata, "w+");
			fprintf(arch1, "BLOCK_SIZE=64\n");
			fprintf(arch1, "BLOCKS=5500\n");
			fprintf(arch1, "MAGIC_NUMBER=LISSANDRA\n");
			fclose(arch1);

			path = string_new();
			string_append(&path,config->PUNTO_MONTAJE);
			string_append(&path,"/Metadata/Bitmap.bin");
			rutas.Bitmap = path;
			FILE*arch2 = fopen(rutas.Bitmap, "w+");

			path = string_new();
			string_append(&path,config->PUNTO_MONTAJE);
			string_append(&path,"/Tablas/");
			rutas.Tablas = path;
			mkdir(rutas.Tablas, 0777);

			path = string_new();
			string_append(&path,config->PUNTO_MONTAJE);
			string_append(&path,"/Bloques/");
			rutas.Bloques = path;
			mkdir(rutas.Bloques, 0777);

		free(config);

		return 1;
}
int leerMetadata(){
	t_config* config = config_create(rutas.Metadata);
	if(config==NULL){
		printf("No se encontró el archivo Metadata en :%s\n", rutas.Metadata);
		return -1;
	}

	if (config_has_property(config, "BLOCKS")){
		metadata.BLOCKS = config_get_int_value(config,"BLOCKS");
		printf("CANTIDAD DE BLOQUES: %i\n", metadata.BLOCKS);
	}else{
		printf("No se encontró el parámetro BLOCKS dentro del archivo Metadata\n");
		return -1;
	}
	if (config_has_property(config, "BLOCK_SIZE")){
		metadata.BLOCK_SIZE = config_get_int_value(config,"BLOCK_SIZE");
		printf("TAMANIO DE BLOQUES: %i\n", metadata.BLOCK_SIZE);
	}else{
		printf("No se encontró el parámetro BLOCK_SIZE dentro del archivo Metadata\n");
		return -1;
	}
	if (config_has_property(config, "MAGIC_NUMBER")){
		metadata.MAGIC_NUMBER = config_get_string_value(config,"MAGIC_NUMBER");
		printf("MAGIC_NUMBER: %s\n", metadata.MAGIC_NUMBER);
	}else{
		printf("No se encontró el parámetro MAGIC_NUMBER dentro del archivo Metadata\n");
		return -1;
	}

		return 1;
}
