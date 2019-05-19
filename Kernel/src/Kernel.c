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

//#include "../LissandraFileSystem/src/FileSystem.h" //para traer el t_metadata
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
t_dictionary *describeGlobal(char* IP_MEMORIA){
	log_info(logger, "Pidiendo metadata global");
	printf("Memoria request %s",IP_MEMORIA);
	t_dictionary *diccionario = dictionary_create();
	//hardcodeo datos
	//tabla1
		t_metadata_tabla * metaTabla1 = malloc(sizeof(t_metadata_tabla));
		metaTabla1->CANT_PARTICIONES = 5;
		metaTabla1->T_COMPACTACION = 5000;
		metaTabla1->CONSISTENCIA = "SC";
		dictionary_put(diccionario,"tabla1",metaTabla1);
	//tabla2
		t_metadata_tabla * metaTabla2 = malloc(sizeof(t_metadata_tabla));
		metaTabla2->CANT_PARTICIONES = 10;
		metaTabla2->T_COMPACTACION = 4000;
		metaTabla2->CONSISTENCIA = "HC";
		dictionary_put(diccionario,"tabla2",metaTabla2);
	//tabla3
		t_metadata_tabla * metaTabla3 = malloc(sizeof(t_metadata_tabla));
		metaTabla3->CANT_PARTICIONES = 40;
		metaTabla3->T_COMPACTACION = 3000;
		metaTabla3->CONSISTENCIA = "EC";
		dictionary_put(diccionario,"tabla3",metaTabla3);


	return diccionario;
}
int main(void) {
	t_config_kernel *config;
	t_dictionary *metadataTablas;
	char *rutaConfig = "/home/utnso/tp-2019-1c-Los-Sisoperadores/Kernel/src/config_kernel.cfg";

	inicializarArchivoDeLogs("/home/utnso/tp-2019-1c-Los-Sisoperadores/Kernel/erroresKernel.log");
	inicializarArchivoDeLogs("/home/utnso/tp-2019-1c-Los-Sisoperadores/Kernel/infoKernel.log");

	logger = log_create("/home/utnso/tp-2019-1c-Los-Sisoperadores/Kernel/infoKernel.log", "Kernel Info Logs", 1, LOG_LEVEL_INFO);
	loggerError = log_create("/home/utnso/tp-2019-1c-Los-Sisoperadores/Kernel/erroresKernel.log", "Kernel Error Logs", 1, LOG_LEVEL_ERROR);

	config = cargarConfig(rutaConfig);
	//cada METADATA_REFRESH hacer
		metadataTablas = describeGlobal(config->IP_MEMORIA);
		t_metadata_tabla * metaTabla1 = dictionary_get(metadataTablas,"tabla1");
		//puts(metaTabla1->CONSISTENCIA);


	//cuando llega un request
		//INSERT TABLA1 1 “Mi nombre es Lissandra”
		//fijarme a que memoria mandarlo segun la consistencia de TABLA1
		//mandar el request

		//setearCriterios(infoTablas);

	//Cuando llega un request
		//memDestino = criterios(nombreTabla);
		//resultado = ejecutarRequest(memDestino,request);
	/*
	if(configurarSocketCliente()){
		enviarConsulta();
	}
	close(serverSocket);
	*/
	return EXIT_SUCCESS;
}
