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
	log_info(loggerInfo, "Levantando archivo de configuracion del proceso LFS \n");
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
	config->BLOCK_SIZE = config_get_int_value(fsConfig, "BLOCK_SIZE");
	config->BLOCKS = config_get_int_value(fsConfig, "BLOCKS");
	config->MAGIC_NUMBER = string_duplicate(config_get_string_value(fsConfig, "MAGIC_NUMBER"));
	log_info(loggerInfo, "Archivo de configuracion del proceso LFS levantado \n");

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
	loggerInfo = log_create("/home/utnso/tp-2019-1c-Los-Sisoperadores/LissandraFileSystem/infoLFS.log", "LFS Logs", 1, LOG_LEVEL_INFO);
	loggerError = log_create("/home/utnso/tp-2019-1c-Los-Sisoperadores/LissandraFileSystem/erroresLFS.log", "LFS Error Logs", 1,
			LOG_LEVEL_ERROR);
	loggerTrace = log_create("/home/utnso/tp-2019-1c-Los-Sisoperadores/LissandraFileSystem/resultados.log", "LFS Results Logs", 1,
			LOG_LEVEL_TRACE);
}

int main(void) {
	inicializarLoggers();
	log_info(loggerInfo, "Inicializando LISSANDRA FILE SYSTEM.");

	//INICIALIZAMOS LOS SEMAFOROS
	listaSemaforos = list_create();
	pthread_mutex_init(&mutexBitarray, NULL);
	pthread_mutex_init(&mutexBuscarDirectorios,NULL);
	pthread_mutex_init(&mutexCompactacion,NULL);
	pthread_mutex_init(&mutexObtenerMetadata,NULL);
	pthread_mutex_init(&mutexDrop,NULL);


	//CARGAMOS ARCHIVOS DE CONFIGURACION
	cargarConfig();
	cargarMetadata(config);

	//CARGA MEMTABLE
	cargarMemtable();

	//HILO ESCUCHA SERVIDOR
	pthread_create(&serverThread, NULL, (void*) iniciarSocketServidor, NULL);
	pthread_detach(serverThread);

	//HILO DUMP
	pthread_create(&dumpThread, NULL, (void*) iniciarProcesoDump, NULL);
	pthread_detach(dumpThread);

	//API
	consolaLFS();

	//FINALIZAR FILESYSTEM
	pthread_kill(serverThread, SIGUSR1);
	vaciarMemtable();
	freeConfig();
	bitarray_destroy(bitmap);
	list_destroy_and_destroy_elements(listaSemaforos, (void*) freeSemaforoTabla);
	pthread_mutex_destroy(&mutexBitarray);

	return EXIT_SUCCESS;
}

