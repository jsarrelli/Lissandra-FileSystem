/*
 * FileSystem.c
 *
 *  Created on: 23 abr. 2019
 *      Author: utnso
 */

/* Aqui desarrollaremos el sub modulo FileSystem del TP*/

#include "FileSystem.h"

void crearBloques() {
	int i;
	FILE *archivo;
	for (i = 0; i < metadata.BLOCKS; ++i) {
		char* pat = string_new();
		char* numeroBloque = string_itoa(i);
		string_append(&pat, rutas.Bloques);
		string_append(&pat, numeroBloque);
		string_append(&pat, ".bin");
		archivo = fopen(pat, "rb");
		if (archivo == NULL) {
			archivo = fopen(pat, "wb");
		}
		free(pat);
		free(numeroBloque);
		fclose(archivo);
	}
}

t_list* buscarBloquesLibres(int cant) {

	t_list* bloquesLibres = list_create();
	for (int i = 0; i < metadata.BLOCKS && list_size(bloquesLibres) < cant; ++i) {
		if (bitarray_test_bit(bitmap, i) == 0) {
			list_add(bloquesLibres, (void*) i);
		}
	}

	if (list_is_empty(bloquesLibres)) {
		list_destroy(bloquesLibres);
		return NULL;
	}
	return bloquesLibres;
}

void liberarBloque(int index) {
	bitarray_clean_bit(bitmap, index);
	log_info(loggerInfo, "Bloque %d liberado", index);
	escribirBitmap();

}

void reservarBloque(int index) {
	bitarray_set_bit(bitmap, index);
	log_info(loggerInfo, "Bloque %d reservado", index);
	escribirBitmap();
}

void leerBitmap() {
	int cantidadBloques = (metadata.BLOCKS / 8);

	int fd = open(rutas.Bitmap, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);

	ftruncate(fd, cantidadBloques);
	if (fd == -1) {
		log_error(loggerError, "No se pudo abrir el archivo de bitmap");
	}

	char * bitarray = mmap(NULL, cantidadBloques, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

	bitmap = bitarray_create_with_mode(bitarray, cantidadBloques, LSB_FIRST);

	if (strlen(bitarray) == 0) {
		if (string_is_empty(bitarray)) {
			for (int i = 0; i < metadata.BLOCKS; i++) {
				bitarray_clean_bit(bitmap, i);
			}
		}
	}

	msync(bitarray, cantidadBloques, MS_SYNC);

	log_info(loggerInfo, "El tamanio del bitmap es de %d bits", bitarray_get_max_bit(bitmap));

	close(fd);

}

void escribirBitmap() {
	msync(bitmap->bitarray, metadata.BLOCKS / 8, MS_SYNC);
}

void destruirBitmap(t_bitarray *bitmap) {
	free(bitmap->bitarray);
	bitarray_destroy(bitmap);
}

void cargarMemtable() {
	memtable = list_create();
	log_info(loggerInfo, "Levantando tablas ya existentes..");
	t_list* listaDirectorios = list_create();
	buscarDirectorios(rutas.Tablas, listaDirectorios);

	t_list* nombreTablas = list_map(listaDirectorios, (void*) obtenerNombreTablaByRuta);

	list_iterate(nombreTablas, (void*) insertarTablaEnMemtable);

	list_destroy_and_destroy_elements(nombreTablas, free);
	list_destroy_and_destroy_elements(listaDirectorios, free);

}

int cargarMetadata() {
	char*path;
	mkdir(config->PUNTO_MONTAJE, 0777);
	path = string_new();
	string_append(&path, config->PUNTO_MONTAJE);
	string_append(&path, "/Metadata");
	mkdir(path, 0777);
	string_append(&path, "/Metadata.bin");
	rutas.Metadata = path;
	FILE*arch1 = fopen(rutas.Metadata, "w+");
	fprintf(arch1, "BLOCK_SIZE=%d\n", config->BLOCK_SIZE);
	fprintf(arch1, "BLOCKS=%d\n", config->BLOCKS);
	fprintf(arch1, "MAGIC_NUMBER=%s\n", config->MAGIC_NUMBER);
	fclose(arch1);

	path = string_new();
	string_append(&path, config->PUNTO_MONTAJE);
	string_append(&path, "/Metadata/Bitmap.bin");
	rutas.Bitmap = path;

	path = string_new();
	string_append(&path, config->PUNTO_MONTAJE);
	string_append(&path, "Tablas/");
	rutas.Tablas = path;
	mkdir(rutas.Tablas, 0777);

	path = string_new();
	string_append(&path, config->PUNTO_MONTAJE);
	string_append(&path, "/Bloques/");
	rutas.Bloques = path;
	mkdir(rutas.Bloques, 0777);

	leerMetadata();
	printf("Metadata  leida \n");

	leerBitmap();
	printf("Bitmap creado\n\n");

	log_info(loggerInfo, "Archivo Metadata cargado");
	return 1;
}

int leerMetadata() {
	t_config* config = config_create(rutas.Metadata);
	if (config == NULL) {
		printf("No se encontró el archivo Metadata en :%s\n", rutas.Metadata);
		return -1;
	}

	if (config_has_property(config, "BLOCKS")) {
		metadata.BLOCKS = config_get_int_value(config, "BLOCKS");
		printf("CANTIDAD DE BLOQUES: %i\n", metadata.BLOCKS);
		crearBloques();
	} else {
		printf("No se encontró el parámetro BLOCKS dentro del archivo Metadata\n");
		return -1;
	}
	if (config_has_property(config, "BLOCK_SIZE")) {
		metadata.BLOCK_SIZE = config_get_int_value(config, "BLOCK_SIZE");
		printf("TAMANIO DE BLOQUES: %i\n", metadata.BLOCK_SIZE);
	} else {
		printf("No se encontró el parámetro BLOCK_SIZE dentro del archivo Metadata\n");
		return -1;
	}
	if (config_has_property(config, "MAGIC_NUMBER")) {
		char* magicNumber = string_duplicate(config_get_string_value(config, "MAGIC_NUMBER"));
		metadata.MAGIC_NUMBER = magicNumber;
		printf("MAGIC_NUMBER: %s\n", metadata.MAGIC_NUMBER);
	} else {
		printf("No se encontró el parámetro MAGIC_NUMBER dentro del archivo Metadata\n");
		return -1;
	}
	config_destroy(config);
	return 1;
}

t_semaforos_tabla* getSemaforoByTabla(char* nombreTabla) {

	bool isTablaBuscada(t_semaforos_tabla* semaforoTabla) {
		return strcmp(semaforoTabla->nombreTabla, nombreTabla) == 0;
	}
	return list_find(listaSemaforos, (void*) isTablaBuscada);
}

void cargarSemaforosTabla(char* nombreTabla) {
	t_semaforos_tabla* semaforoTabla = malloc(sizeof(t_semaforos_tabla));

	semaforoTabla->nombreTabla = string_duplicate(nombreTabla);

	pthread_mutex_t mutexCompactacion;
	pthread_mutex_init(&mutexCompactacion, NULL);

	pthread_mutex_t mutexMemtable;
	pthread_mutex_init(&mutexMemtable, NULL);

	pthread_mutex_t mutexDump;
	pthread_mutex_init(&mutexDump, NULL);

	semaforoTabla->mutexCompactacion = mutexCompactacion;
	semaforoTabla->mutexMemtable = mutexMemtable;
	semaforoTabla->mutexTmp = mutexDump;

	list_add(listaSemaforos, semaforoTabla);

}

void freeSemaforoTabla(t_semaforos_tabla* semaforoTabla) {
	free(semaforoTabla->nombreTabla);

	pthread_mutex_destroy(&semaforoTabla->mutexMemtable);
	pthread_mutex_destroy(&semaforoTabla->mutexCompactacion);
	free(semaforoTabla);
}

int getSizeOfFile(char* rutaArchivo){
	struct stat st;
	if(stat(rutaArchivo,&st)==-1){
		return -1;

	}
	return st.st_size;
}
