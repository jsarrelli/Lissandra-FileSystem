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



int main() {
	logger = log_create("MEM_logs.txt", "MEMORIA Logs", true, LOG_LEVEL_DEBUG);
	log_info(logger, "--Inicializando proceso MEMORIA--");
	cargarConfiguracion();

	inicializarMemoria(100,configuracion->TAM_MEMORIA);
	log_info(logger, "--Memoria inicializada--");



	pthread_create(&threadId, NULL, leerConsola(), NULL);

	//pruebas varias
	insertarSegmentoEnMemoria("TABLA1",NULL);
	insertarSegmentoEnMemoria("TABLA2",NULL);
	Segmento* tabla1= buscarSegmentoEnMemoria("TABLA1");
	insertarPaginaEnMemoria(3,"juli" ,tabla1);
	//SELECT_MEMORIA("TABLA1" , 3);

	log_info(logger, "Configurando Listening Socket.");
	if(configurarSocketServidor()){
		escuchar();
	}

	close(listenningSocket);
	log_destroy(logger);
	free(configuracion);
	return EXIT_SUCCESS;
}

void* leerConsola()
{
	char comando[100]="";
	while (strcmp(comando,"salir" )) {

		puts("Ingrese comandos a ejecutar. Escriba 'salir' para finalizar");
		fgets(comando, sizeof(comando), stdin);
		procesarConsulta(comando);

	}
	log_info(logger, "Fin de leectura por consola");
	return NULL;
}

void cargarConfiguracion() {
	log_info(logger,"Levantando archivo de configuracion del proceso MEMORIA");
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
	log_info(logger, "Archivo de configuracion levantado");
	config_destroy(archivo_configuracion);
}

