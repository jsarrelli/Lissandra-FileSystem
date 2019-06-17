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

int main(void) {
	log_master = malloc(sizeof(logStruct));
	inicializarLogStruct();
	log_info(log_master->logInfo, "Hola!!!");
	sem_init(&ejecutarHilos, 0, 0); // Recordar cambiar el 0 a 1
	sem_init(&mutex_colaReady, 0, 1);
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
//	agregarHiloAListaHilosEInicializo(listaHilos);


	operacion = readline(">");
	while(!instruccionSeaSalir(operacion)){
		procExec* proceso = newProceso();
		agregarRequestAlProceso(proceso, operacion);

		deNewAReady(proceso);

//		deReadyAExec();

		// Por ahora lo hago con un solo proceso y lo hago manual
		ejecutarProcesos();
		funcionThread((int*)2); // El 2 es un ejemplo porque no me importa lo que reciba pero si que reciba algo









//		free(operacion);
//		destruirProcesoExec(proceso);
		operacion = readline(">");
	}
//	free(operacion);
//	free(hilos);
	destruirElementosMain(listaHilos, colaReady);
	destruirLogStruct(log_master);
//	config_destroy(config);
	return EXIT_SUCCESS;
}
