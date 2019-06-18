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

/*
 * Estado del Kernel:
 *
 * Actualmente, el Kernel funciona con codigo hardcodeado ya que todavia no usa las funciones sockets
 *
 * A pesar de eso, se pueden acceder a todos los comandos del Kernel e indicar si hay algun comando invalido (API Kernel)
 *
 * Se puede asignar un criterio a una memoria (comando ADD)
 *
 * Se pueden loggear los errores correctamente y administrativamente (Muy importante!!)
 *
 * TODO:
 * 		- Manejo de errores
 * 		- Resto de las funciones (con codigo harcodeado) + pequeñas modificaciones del comando RUN
 * 		- Unir los modulos con las funciones sockets
 * 		- Round Robin
 *		- Multiprocesamiento (Brian)
 *
 * 	Aclaracion:
 * 		Muchas de las decisiones fueron hechas para ver los puntos que faltan
 *
 *
 * 	Pero lo mas importante es que ahora tenemos una muy buena base para avanzar!!!
 */

int main(void) {
	log_master = malloc(sizeof(logStruct));
	inicializarLogStruct();
//	log_info(log_master->logInfo, "Hola!!!");
	sem_init(&ejecutarHilos, 0, 0); // Recordar cambiar el 0 a 1
	sem_init(&mutex_colaReady, 0, 1);
	char*operacion;
	cantRequestsEjecutadas =0;
	haySC = false;
	idMemoria=1;

	config = cargarConfig((char*) RUTA_CONFIG_KERNEL);






	colaReady = queue_create();
	listaHilos = list_create();
	listaMemorias = list_create();
	hardcodearInfoMemorias();
	log_trace(log_master->logInfo, "El id de la primera memoria es: %d\n", ((infoMemoria*)list_get(listaMemorias, 1))->id);
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
	log_info(log_master->logInfo, "Finalizando consola\nLiberando memoria");
	free(operacion);
//	free(hilos);
	destruirElementosMain(listaHilos, colaReady);
	destruirListaMemorias();
	log_info(log_master->logInfo, "Consola terminada");
	destruirLogStruct(log_master);
//	config_destroy(config);
	return EXIT_SUCCESS;
}
