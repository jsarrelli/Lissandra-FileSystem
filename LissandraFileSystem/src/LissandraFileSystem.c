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



int main(void) {
	t_configuracion_LFS* configuracion;
	inicializarArchivoDeLogs("/home/utnso/tp-2019-1c-Los-Sisoperadores/LissandraFileSystem/error.log");
	inicializarArchivoDeLogs("/home/utnso/tp-2019-1c-Los-Sisoperadores/LissandraFileSystem/info.log");
	loggerInfo = log_create("/home/utnso/tp-2019-1c-Los-Sisoperadores/LissandraFileSystem/info.log", "LFS Logs", 1, LOG_LEVEL_INFO);
	loggerError = log_create("/home/utnso/tp-2019-1c-Los-Sisoperadores/LissandraFileSystem/error.log", "LFS Error Logs", 1, LOG_LEVEL_ERROR);
	log_info(loggerInfo, "Inicializando proceso LISSANDRA FILE SYSTEM. \n");
	configuracion = cargarConfig();

}

t_configuracion_LFS cargarConfig (){
		t_configuracion_LFS *config = malloc(sizeof(t_configuracion_LFS));
		t_config *fsConfig = config_create("./fsConfig.cfg");

		config->PUERTO_ESCUCHA = config_get_int_value(fsConfig, "PUERTO_ESCUCHA");
		config->PUNTO_MONTAJE = config_get_string_value(fsConfig, "PUNTO_MONTAJE");
		config->RETARDO = config_get_int_value(fsConfig, "RETARDO");
		config->TAMANIO_VALUE = config_get_int_value(fsConfig, "TAMANIO_VALUE");
		config->TIEMPO_DUMP = config_get_int_value(fsConfig, "TIEMPO_DUMP");

		config_destroy(fsConfig);

		return config;
	}


