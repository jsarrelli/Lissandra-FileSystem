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
	logger = log_create("MEM_logs.txt", "MEMORIA Logs", true, LOG_LEVEL_INFO);
	log_info(logger, "--Inicializando proceso MEMORIA--");
	cargarConfiguracion();

	//HANDSHAKE INICIAL CON FILESYSTEM
	int socketFileSystem = HandshakeInicial();

	//INICIALIZACION DE MEMORIA PRINCIPAL
	inicializarMemoria(valueMaximoPaginas, configuracion->TAM_MEMORIA,socketFileSystem);
	log_info(logger, "--Memoria inicializada--");

	//HILO DE CONSOLA
	pthread_create(&consoleThread, NULL, leerConsola, NULL);
	pthread_detach(consoleThread);

	//HILO DE JOURNAL
//	pthread_create(&intTemporalJournal, NULL, (void*)procesoTemporalJournal, NULL);
//	pthread_detach(intTemporalJournal);

	//HILO DE GOSSIPING
//	pthread_create(&intTemporalGossiping, NULL, procesoTemporalGossiping, NULL);
//	pthread_detach(intTemporalGossiping);

	//PUERTO DE ESCUCHA
	log_info(logger, "Configurando Listening Socket...");
	int listenningSocket = configurarSocketServidor(configuracion->PUERTO_ESCUCHA);
	if (listenningSocket != -1) {
		escuchar(listenningSocket);
	}


	close(listenningSocket);
	log_destroy(logger);
	free(configuracion);
	config_destroy(archivo_configuracion);

	return EXIT_SUCCESS;
}

void* leerConsola()
{
	char * consulta;
	while (1) {

		puts("\nIngrese comandos a ejecutar. Para salir presione enter");
		consulta = readline(">");
		if (consulta) {
			add_history(consulta);
		}
		if (consulta == NULL) {
			return NULL;
		}
		procesarConsulta(consulta);

	}
	log_info(logger, "Fin de leectura por consola");
	return NULL;
}


void cargarConfiguracion() {
	pathMEMConfig = "/home/utnso/tp-2019-1c-Los-Sisoperadores/Memoria/configMEM.cfg";
	log_info(logger, "Levantando archivo de configuracion del proceso MEMORIA");
	configuracion = (MEMORIA_configuracion*) malloc(sizeof(MEMORIA_configuracion));
	if (configuracion == NULL) {
		exit(0);
	}
	archivo_configuracion = config_create(pathMEMConfig);
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

}

void procesoTemporalJournal(){
	while(1){
		usleep(configuracion->TIEMPO_JOURNAL*1000);
		journalMemoria();
	}
}

void procesoTemporalGossiping(){
	while(1){
		usleep(configuracion->TIEMPO_GOSSIPING*1000);
		//falta funcion gossiping
	}

}

