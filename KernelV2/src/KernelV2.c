/*
 ============================================================================
 Name        : tp_operativos_kernel.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include "KernelHeader.h"

static const char* INFO_KERNEL =
		"/home/utnso/tp-2019-1c-Los-Sisoperadores/KernelV2/infoKernel.log";
static const char* ERRORES_KERNEL =
		"/home/utnso/tp-2019-1c-Los-Sisoperadores/KernelV2/erroresKernel.log";
static const char* RUTA_CONFIG_KERNEL =
		"/home/utnso/tp-2019-1c-Los-Sisoperadores/KernelV2/configKernel.cfg";

int main(void) {
	logger = log_create((char*) INFO_KERNEL, "Kernel Info Logs", 1, LOG_LEVEL_INFO);
	loggerError = log_create((char*) ERRORES_KERNEL, "Kernel Error Logs", 1,
				LOG_LEVEL_ERROR);
	log_info(logger, "Hola!!!");
	sem_init(&ejecutarHilos, 0, 0);
	sem_init(&mutex_colaReady, 0, 1); // Recordar cambiar el 0 a 1
	char*operacion;

	config = cargarConfig((char*) RUTA_CONFIG_KERNEL);






	colaReady = queue_create();
	listaHilos = list_create();
	listaMemorias = list_create();
	hardcodearInfoMemorias();
	printf("El id de la primera memoria es: %d\n", ((infoMemoria*)list_get(listaMemorias, 1))->id);
	listaMetadataTabla = list_create();
	hardcodearListaMetadataTabla();
	quantum = config->QUANTUM;
//	pthread_t* hilos = malloc(sizeof(pthread_t)* config->MULTIPROCESAMIENTO);
	agregarHiloAListaHilosEInicializo(listaHilos);


	operacion = readline(">");
	while(!instruccionSeaSalir(operacion)){
		procExec* proceso = newProceso();
		agregarRequestAlProceso(proceso, operacion);

		deNewAReady(proceso);

//		deReadyAExec();

		// Por ahora lo hago con un solo proceso y lo hago manual
//		list_get(listaHilos, 0);
		ejecutarProcesos();
		funcionThread((int*)2);


//		if(queue_size(colaReady) >=3){
//			// execute();
//		}







//		free(operacion);
		destruirProcesoExec(proceso);
		operacion = readline(">");
	}
	free(operacion);
//	free(hilos);
	destruirElementosMain(listaHilos, colaReady, logger);
//	config_destroy(config);
	return EXIT_SUCCESS;
}
