/*
 ============================================================================
 Name        : Kernel.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include "SocketCliente.h"
#include "kernel.h"
t_config_kernel *cargarConfig(char *ruta){
	puts("!!!Hello World!!!");
	log_info(logger,"Levantando archivo de configuracion del proceso Kernel \n");
	t_config_kernel* config = malloc(sizeof(t_config_kernel));
	t_config *kernelConfig = config_create(ruta);
	if(kernelConfig == NULL){
		perror("Error ");

		log_error(loggerError,"Problema al abrir el archivo");
	}
	config->IP_MEMORIA = get_campo_config_string(kernelConfig, "IP_MEMORIA");
	config->PUERTO_MEMORIA = get_campo_config_int(kernelConfig, "PUERTO_MEMORIA");
	config->QUANTUM = get_campo_config_int(kernelConfig, "QUANTUM");
	config->MULTIPROCESAMIENTO = get_campo_config_int(kernelConfig, "MULTIPROCESAMIENTO");
	config->METADATA_REFRESH = get_campo_config_int(kernelConfig, "METADATA_REFRESH");
	config->SLEEP_EJECUCION = get_campo_config_int(kernelConfig, "SLEEP_EJECUCION");

	log_info(logger,"Archivo de configuracion del proceso Kernel levantado \n");

	return config;
}

int main(void) {
	t_config_kernel *config;
	char *rutaConfig = "/home/utnso/tp-2019-1c-Los-Sisoperadores/Kernel/src/config_kernel.cfg";

	inicializarArchivoDeLogs("/home/utnso/tp-2019-1c-Los-Sisoperadores/Kernel/erroresKernel.log");
	inicializarArchivoDeLogs("/home/utnso/tp-2019-1c-Los-Sisoperadores/Kernel/infoKernel.log");

	logger = log_create("/home/utnso/tp-2019-1c-Los-Sisoperadores/Kernel/infoKernel.log", "Kernel Info Logs", 1, LOG_LEVEL_INFO);
	loggerError = log_create("/home/utnso/tp-2019-1c-Los-Sisoperadores/Kernel/erroresKernel.log", "Kernel Error Logs", 1, LOG_LEVEL_ERROR);

	config = cargarConfig(rutaConfig);
	/*
	if(configurarSocketCliente()){
		enviarConsulta();
	}
	close(serverSocket);
	*/
	return EXIT_SUCCESS;
}
