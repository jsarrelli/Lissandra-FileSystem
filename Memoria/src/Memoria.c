/*
 ============================================================================
 Name        : Memoria.c
 Author      : Matias Erratchu y Julian Sarrelli
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */
#include "Memoria.h"
#include "MemoriaPrincipal.h"

int main(int argc, char** argv) {
	logger = log_create("MEM_logs.txt", "MEMORIA Logs", 1, 1);
	log_info(logger, "Inicializando proceso MEMORIA. \n");
	configuracion = get_configuracion();

	puts("--Memoria inicializada--"); /* prints !!!Hello World!!! */
	if (argc > 1) {
			procesarConsulta(argc,argv);
		}

	log_info(logger, "Configurando Listening Socket. \n");

	insertarSegmentoEnMemoria("TABLA1");

//	if(configurarSocketServidor()){
//		escuchar();
//	}

//	close(listenningSocket);



	return EXIT_SUCCESS;
}


MEMORIA_configuracion get_configuracion() {
	log_info(logger,"Levantando archivo de configuracion del proceso MEMORIA \n");
	MEMORIA_configuracion configuracion;
	t_config*  archivo_configuracion = config_create(pathMEMConfig);
	configuracion.PUERTO_ESCUCHA = get_campo_config_string(archivo_configuracion, "PUERTO_ESCUCHA");
	configuracion.IP_FS = get_campo_config_string(archivo_configuracion, "IP_FS");
	configuracion.PUERTO_FS = get_campo_config_int(archivo_configuracion, "PUERTO_FS");
	configuracion.IP_SEEDS = get_campo_config_string(archivo_configuracion, "IP_SEEDS");
	configuracion.PUERTOS_SEEDS = get_campo_config_string(archivo_configuracion, "PUERTOS_SEEDS");
	configuracion.RETARDO_MEMORIA = get_campo_config_int(archivo_configuracion, "RETARDO_MEMORIA");
	configuracion.RETARDO_FS = get_campo_config_int(archivo_configuracion, "RETARDO_FS");
	configuracion.TAM_MEMORIA = get_campo_config_int(archivo_configuracion, "TAM_MEMORIA");
	configuracion.TIEMPO_JOURNAL = get_campo_config_int(archivo_configuracion, "TIEMPO_JOURNAL");
	configuracion.TIEMPO_GOSSIPING = get_campo_config_int(archivo_configuracion, "TIEMPO_GOSSIPING");
	configuracion.MEMORY_NUMBER = get_campo_config_int(archivo_configuracion, "MEMORY_NUMBER");
	config_destroy(archivo_configuracion);
	return configuracion;
}

