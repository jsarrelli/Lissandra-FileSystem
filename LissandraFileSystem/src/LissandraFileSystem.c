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

void cargarConfig() {
	char* configPath = "/home/utnso/tp-2019-1c-Los-Sisoperadores/LissandraFileSystem/Config/fsConfig.cfg";
	log_info(logger, "Levantando archivo de configuracion del proceso LFS \n");
	if (config != NULL) {
		freeConfig();
	}
	config = malloc(sizeof(t_configuracion_LFS));
	t_config *fsConfig = config_create(configPath);

	config->PUERTO_ESCUCHA = string_duplicate(get_campo_config_string(fsConfig, "PUERTO_ESCUCHA"));
	config->PUNTO_MONTAJE = string_duplicate(get_campo_config_string(fsConfig, "PUNTO_MONTAJE"));
	config->RETARDO = get_campo_config_int(fsConfig, "RETARDO");
	config->TAMANIO_VALUE = get_campo_config_int(fsConfig, "TAMANIO_VALUE");
	config->TIEMPO_DUMP = get_campo_config_int(fsConfig, "TIEMPO_DUMP");
	log_info(logger, "Archivo de configuracion del proceso LFS levantado \n");

	config_destroy(fsConfig);
	listenArchivo("/home/utnso/tp-2019-1c-Los-Sisoperadores/LissandraFileSystem/Config", cargarConfig);

}

void iniciarSocketServidor() {
	int listenningSocket = configurarSocketServidor(config->PUERTO_ESCUCHA);
	if (listenningSocket != 0) {
		escuchar(listenningSocket);
	}

}

void freeConfig() {
	free(config->PUERTO_ESCUCHA);
	free(config->PUNTO_MONTAJE);
	free(config);
}

void inicializarLoggers() {
	inicializarArchivoDeLogs("/home/utnso/tp-2019-1c-Los-Sisoperadores/LissandraFileSystem/erroresLFS.log");
	inicializarArchivoDeLogs("/home/utnso/tp-2019-1c-Los-Sisoperadores/LissandraFileSystem/infoLFS.log");
	logger = log_create("/home/utnso/tp-2019-1c-Los-Sisoperadores/LissandraFileSystem/infoLFS.log", "LFS Logs", 1, LOG_LEVEL_INFO);
	loggerError = log_create("/home/utnso/tp-2019-1c-Los-Sisoperadores/LissandraFileSystem/erroresLFS.log", "LFS Error Logs", 1,
			LOG_LEVEL_ERROR);
}

int main(void) {
	inicializarLoggers();
	log_info(logger, "Inicializando proceso LISSANDRA FILE SYSTEM. \n");

	cargarConfig();

	cargarMetadata(config);
	printf("Metadata cargada \n ");

	cargarMemtable();

	//INICIALIZAMOS LOS SEMAFOROS

	sem_init(&mutexEscrituraBloques, 0, 1);
	sem_init(&mutexCompactacion, 0,1);

	//HILO ESCUCHA SERVIDOR
	pthread_create(&serverThread, NULL, (void*) iniciarSocketServidor, NULL);
	pthread_detach(serverThread);

	//HILO DUMP
//	pthread_create(&dumpThread, NULL, (void*) procesoDump, NULL);
//	pthread_detach(dumpThread);

	consolaLFS();
	pthread_kill(serverThread,SIGUSR1);
	procesoDump();
	vaciarMemtable();
	freeConfig();
	bitarray_destroy(bitmap);
	return EXIT_SUCCESS;
}

