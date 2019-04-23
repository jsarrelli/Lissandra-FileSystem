/*
 ============================================================================
 Name        : LissandraFileSystem.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */


#include "LissandraFileSystem.h"



t_configuracion_LFS* cargarConfig (){
	log_info(logger,"Levantando archivo de configuracion del proceso LFS \n");
		t_configuracion_LFS* config = malloc(sizeof(t_configuracion_LFS));
		t_config *fsConfig = config_create("./fsConfig.cfg");

		config->PUERTO_ESCUCHA = get_campo_config_int(fsConfig, "PUERTO_ESCUCHA");
		config->PUNTO_MONTAJE = get_campo_config_string(fsConfig, "PUNTO_MONTAJE");
		config->RETARDO = get_campo_config_int(fsConfig, "RETARDO");
		config->TAMANIO_VALUE = get_campo_config_int(fsConfig, "TAMANIO_VALUE");
		config->TIEMPO_DUMP = get_campo_config_int(fsConfig, "TIEMPO_DUMP");

		config_destroy(fsConfig);

		return config;
	}


int main(int argc, char* argv[]) {
		t_configuracion_LFS* config = cargarConfig();
		inicializarArchivoDeLogs("/home/utnso/tp-2019-1c-Los-Sisoperadores/LissandraFileSystem/error.log");
		inicializarArchivoDeLogs("/home/utnso/tp-2019-1c-Los-Sisoperadores/LissandraFileSystem/info.log");
		logger = log_create("/home/utnso/tp-2019-1c-Los-Sisoperadores/LissandraFileSystem/info.log", "LFS Logs", 1, LOG_LEVEL_INFO);
		loggerError = log_create("/home/utnso/tp-2019-1c-Los-Sisoperadores/LissandraFileSystem/error.log", "LFS Error Logs", 1, LOG_LEVEL_ERROR);
		log_info(logger, "Inicializando proceso LISSANDRA FILE SYSTEM. \n");


		return EXIT_SUCCESS;
}







