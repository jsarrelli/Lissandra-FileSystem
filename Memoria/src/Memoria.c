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
#include "AdministradorDeConsultas.h"

int main(int argc, char** argv) {
	logger = log_create("MEM_logs.txt", "MEMORIA Logs", true, LOG_LEVEL_DEBUG);
	log_info(logger, "Inicializando proceso MEMORIA. \n");
	cargarConfiguracion();

	puts("--Memoria inicializada--");
	if (argc > 1) {
			procesarConsulta(argc,argv);
		}

	log_info(logger, "Configurando Listening Socket.");

	inicializarMemoria(100,configuracion->TAM_MEMORIA);
	insertarSegmentoEnMemoria("TABLA1");
	insertarSegmentoEnMemoria("TABLA2");
	insertarPaginaEnMemoria(3,"juli" ,"TABLA1");
	SELECT("TABLA1",3);
	printf("salio");

//	if(configurarSocketServidor()){
//		escuchar();
//	}

//	close(listenningSocket);


	log_destroy(logger);
	free(configuracion);
	return EXIT_SUCCESS;
}


void cargarConfiguracion() {
	log_info(logger,"Levantando archivo de configuracion del proceso MEMORIA \n");
	configuracion=(MEMORIA_configuracion*)malloc(sizeof(MEMORIA_configuracion));
	if(configuracion==NULL){
		exit(0);
	}
	t_config*  archivo_configuracion = config_create(pathMEMConfig);
	configuracion->PUERTO_ESCUCHA = get_campo_config_string(archivo_configuracion, "PUERTO_ESCUCHA");
	configuracion->IP_FS = get_campo_config_string(archivo_configuracion, "IP_FS");
	configuracion->PUERTO_FS = get_campo_config_int(archivo_configuracion, "PUERTO_FS");
	configuracion->IP_SEEDS = get_campo_config_string(archivo_configuracion, "IP_SEEDS");
	configuracion->PUERTOS_SEEDS = get_campo_config_string(archivo_configuracion, "PUERTOS_SEEDS");
	configuracion->RETARDO_MEMORIA = get_campo_config_int(archivo_configuracion, "RETARDO_MEMORIA");
	configuracion->RETARDO_FS = get_campo_config_int(archivo_configuracion, "RETARDO_FS");
	configuracion->TAM_MEMORIA = get_campo_config_int(archivo_configuracion, "TAM_MEMORIA");
	configuracion->TIEMPO_JOURNAL = get_campo_config_int(archivo_configuracion, "TIEMPO_JOURNAL");
	configuracion->TIEMPO_GOSSIPING = get_campo_config_int(archivo_configuracion, "TIEMPO_GOSSIPING");
	configuracion->MEMORY_NUMBER = get_campo_config_int(archivo_configuracion, "MEMORY_NUMBER");
	config_destroy(archivo_configuracion);
}

