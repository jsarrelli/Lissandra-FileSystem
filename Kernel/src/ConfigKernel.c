/*
 * ConfigKernel.c
 *
 *  Created on: 3 jun. 2019
 *      Author: utnso
 */


#include "ConfigKernel.h"

t_config_kernel *cargarConfig(char *ruta){
	puts("!!!Hello World!!!");
	log_info(logger,
			"Levantando archivo de configuracion del proceso Kernel \n");
	t_config_kernel* config = malloc(sizeof(t_config_kernel));
	t_config *kernelConfig = config_create(ruta);
	if (kernelConfig == NULL) {
		perror("Error ");

		log_error(loggerError, "Problema al abrir el archivo");
	}
	config->IP_MEMORIA = get_campo_config_string(kernelConfig, "IP_MEMORIA");
	config->PUERTO_MEMORIA = get_campo_config_int(kernelConfig,
			"PUERTO_MEMORIA");
	config->QUANTUM = get_campo_config_int(kernelConfig, "QUANTUM");
	config->MULTIPROCESAMIENTO = get_campo_config_int(kernelConfig,
			"MULTIPROCESAMIENTO");
	config->METADATA_REFRESH = get_campo_config_int(kernelConfig,
			"METADATA_REFRESH");
	config->SLEEP_EJECUCION = get_campo_config_int(kernelConfig,
			"SLEEP_EJECUCION");

	log_info(logger,
			"Archivo de configuracion del proceso Kernel levantado \n");

	return config;
}
