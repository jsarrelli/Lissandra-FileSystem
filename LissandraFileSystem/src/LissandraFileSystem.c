/*
 ============================================================================
 Name        : LissandraFileSystem.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include "EstructurasFileSystem.h"
#include "FileSystem.h"
#include "funcionesLFS.h"

t_configuracion_LFS* cargarConfig(char* ruta) {
	log_info(logger, "Levantando archivo de configuracion del proceso LFS \n");
	t_configuracion_LFS* config = malloc(sizeof(t_configuracion_LFS));
	fsConfig = config_create(ruta);

	config->PUERTO_ESCUCHA = get_campo_config_string(fsConfig, "PUERTO_ESCUCHA");
	config->PUNTO_MONTAJE = get_campo_config_string(fsConfig, "PUNTO_MONTAJE");
	config->RETARDO = get_campo_config_int(fsConfig, "RETARDO");
	config->TAMANIO_VALUE = get_campo_config_int(fsConfig, "TAMANIO_VALUE");
	config->TIEMPO_DUMP = get_campo_config_int(fsConfig, "TIEMPO_DUMP");
	log_info(logger, "Archivo de configuracion del proceso LFS levantado \n");


	return config;
}

void iniciarSocketServidor(t_configuracion_LFS* config) {
	//OJO lo que estas haciendo aca con la variable config
	int listenningSocket = configurarSocketServidor(config->PUERTO_ESCUCHA);
	if (listenningSocket != 0) {
		escuchar(listenningSocket);
	}

}

int main(void) {
	t_configuracion_LFS* config;
	pthread_t serverThread;
	//listaNombresTablas = list_create();
	memtable = list_create();
	inicializarArchivoDeLogs("/home/utnso/tp-2019-1c-Los-Sisoperadores/LissandraFileSystem/erroresLFS.log");
	inicializarArchivoDeLogs("/home/utnso/tp-2019-1c-Los-Sisoperadores/LissandraFileSystem/infoLFS.log");
	logger = log_create("/home/utnso/tp-2019-1c-Los-Sisoperadores/LissandraFileSystem/infoLFS.log", "LFS Logs", 1, LOG_LEVEL_INFO);
	loggerError = log_create("/home/utnso/tp-2019-1c-Los-Sisoperadores/LissandraFileSystem/erroresLFS.log", "LFS Error Logs", 1,
			LOG_LEVEL_ERROR);
	log_info(logger, "Inicializando proceso LISSANDRA FILE SYSTEM. \n");
	config = cargarConfig("/home/utnso/tp-2019-1c-Los-Sisoperadores/LissandraFileSystem/fsConfig.cfg");

	//crearStructRegistro(config->TAMANIO_VALUE);
	cargarMetadata(config);
	printf("Metadata cargada \n ");
	leerMetadata();
	printf("Metadata  leida \n");

	leerBitmap();
	printf("Bitmap creado\n\n");

	pthread_create(&serverThread, NULL, (void*) iniciarSocketServidor, config);
	pthread_detach(serverThread);

	consolaLFS();
	free(config);
	config_destroy(fsConfig);
	free(bitmap->bitarray);
	bitarray_destroy(bitmap);
	return EXIT_SUCCESS;
}

