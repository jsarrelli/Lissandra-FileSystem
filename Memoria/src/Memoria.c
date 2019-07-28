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
	loggerInfo = log_create("MEM_logs.log", "MEMORIA Logs", true, LOG_LEVEL_INFO);
	loggerTrace = log_create("MEM_results.log", "MEMORIA Logs", true, LOG_LEVEL_TRACE);
	loggerError = log_create("MEM_resuslts.log", "MEMORIA Logs", true, LOG_LEVEL_ERROR);

	log_info(loggerInfo, "--Inicializando proceso MEMORIA--");
	seleccionarArchivoConfig();
	cargarConfiguracion();
	cargarEstructurasGossiping();

//	//HANDSHAKE INICIAL CON FILESYSTEM
	int success = HandshakeInicial();
	if (success == -1) {
		log_info(loggerInfo, "--Memoria finalizada--");
		liberarVariables();
		return EXIT_SUCCESS;
	}
//	//INICIALIZACION DE MEMORIA PRINCIPAL
	inicializarMemoria(configuracion->TAM_MEMORIA);
	log_info(loggerInfo, "--Memoria inicializada--");

//INICIAR SERVIDOR
	pthread_create(&serverThread, NULL, (void*) iniciarSocketServidor, NULL);
	pthread_detach(serverThread);

	//HILO DE JOURNAL
	pthread_create(&intTemporalJournal, NULL, (void*) procesoTemporalJournal, NULL);
	pthread_detach(intTemporalJournal);

//HILO DE GOSSIPING
	pthread_create(&intTemporalGossiping, NULL, (void*) procesoTemporalGossiping, NULL);
	pthread_detach(intTemporalGossiping);

	//CONSOLA
	leerConsola();

	close(listenningSocket);
	liberarVariables();
	finalizarMemoria();
	return EXIT_SUCCESS;
}

void* leerConsola() {
	char * consulta;
	while (1) {

		puts("\nIngrese comandos a ejecutar. Para salir presione enter");
		consulta = readline(">");

		if (strlen(consulta) != 0) {

			procesarConsulta(consulta);
			add_history(consulta);
		} else {
			free(consulta);
			log_info(loggerInfo, "Fin de leectura por consola");
			return NULL;
		}

	}

}

void cargarConfiguracion() {

	char* pathMEMConfig = string_new();
	string_append_with_format(&pathMEMConfig, "/home/utnso/tp-2019-1c-Los-Sisoperadores/Memoria/Config/config-%s.cfg", archivoConfig);
	log_info(loggerInfo, "Levantando configuracion de: %s", pathMEMConfig);

	if (configuracion != NULL) {
		liberarDatosConfiguracion();
	}
	configuracion = (MEMORIA_configuracion*) malloc(sizeof(MEMORIA_configuracion));
	if (configuracion == NULL) {
		exit(0);
	}
	archivo_configuracion = config_create(pathMEMConfig);
	configuracion->PUERTO_ESCUCHA = get_campo_config_string(archivo_configuracion, "PUERTO_ESCUCHA");
	configuracion->IP_FS = get_campo_config_string(archivo_configuracion, "IP_FS");
	configuracion->PUERTO_FS = get_campo_config_int(archivo_configuracion, "PUERTO_FS");
	configuracion->IP_SEEDS = get_campo_config_array(archivo_configuracion, "IP_SEEDS");
	configuracion->PUERTOS_SEEDS = get_campo_config_array(archivo_configuracion, "PUERTOS_SEEDS");
	configuracion->RETARDO_MEMORIA = get_campo_config_int(archivo_configuracion, "RETARDO_MEMORIA");
	configuracion->RETARDO_FS = get_campo_config_int(archivo_configuracion, "RETARDO_FS");
	configuracion->TAM_MEMORIA = get_campo_config_int(archivo_configuracion, "TAM_MEMORIA");
	configuracion->TIEMPO_JOURNAL = get_campo_config_int(archivo_configuracion, "TIEMPO_JOURNAL");
	configuracion->TIEMPO_GOSSIPING = get_campo_config_int(archivo_configuracion, "TIEMPO_GOSSIPING");
	configuracion->MEMORY_NUMBER = get_campo_config_int(archivo_configuracion, "MEMORY_NUMBER");
	configuracion->IP_ESCUCHA = get_campo_config_string(archivo_configuracion, "IP_ESCUCHA");
	log_info(loggerInfo, "Archivo de configuracion levantado");
	free(pathMEMConfig);
	listenArchivo("/home/utnso/tp-2019-1c-Los-Sisoperadores/Memoria/Config", cargarConfiguracion);

}

void iniciarSocketServidor() {
	log_info(loggerInfo, "Configurando Listening Socket...");
	listenningSocket = configurarSocketServidor(configuracion->PUERTO_ESCUCHA);
	if (listenningSocket != 0) {
		escuchar(listenningSocket);
	}

}

void procesoTemporalJournal() {
	while (1) {
		usleep(configuracion->TIEMPO_JOURNAL * 1000);
		log_info(loggerInfo, "Realizando proceso temporal Journal");
		JOURNAL_MEMORIA();
	}
}

void procesoTemporalGossiping() {
	while (1) {
		usleep(configuracion->TIEMPO_GOSSIPING * 1000);
		log_info(loggerInfo, "Descubriendo memorias..");
		gossiping();
	}

}

void liberarDatosConfiguracion() {
	freePunteroAPunteros(configuracion->PUERTOS_SEEDS);
	freePunteroAPunteros(configuracion->IP_SEEDS);
	free(configuracion);
	config_destroy(archivo_configuracion);
}

void liberarVariables() {
	log_destroy(loggerInfo);
	liberarDatosConfiguracion();
	list_destroy_and_destroy_elements(seeds, (void*) freeMemoria);
	list_destroy_and_destroy_elements(tablaGossiping, (void*) freeMemoria);
	pthread_mutex_destroy(&mutexJournal);
}

void seleccionarArchivoConfig() {
	puts("Ingrese el numero de Archivo de Configuracion:");
	archivoConfig = readline(">");
}

