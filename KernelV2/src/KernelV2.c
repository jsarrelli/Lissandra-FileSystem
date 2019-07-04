/*
 ============================================================================
 Name        : tp_operativos_kernel.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include "Kernel.h"

void iniciarVariablesKernel() {
	log_master = malloc(sizeof(logStruct));
	inicializarLogStruct();

	sem_init(&ejecutarHilos, 0, 0); // Recordar cambiar el 0 a 1
	sem_init(&mutex_colaReady, 0, 1);

	cantRequestsEjecutadas = 0;
	haySC = false;
	idMemoria = 1;

	srand(time(NULL));

	config = cargarConfig((char*) RUTA_CONFIG_KERNEL);

	colaReady = queue_create();
	listaHilos = list_create();
	listaMemorias = list_create();
	hardcodearInfoMemorias();
	log_trace(log_master->logInfo, "El id de la primera memoria es: %d\n",
			((infoMemoria*) list_get(listaMemorias, 1))->id);
	listaMetadataTabla = list_create();
	hardcodearListaMetadataTabla();

	quantum = config->QUANTUM;
	multiprocesamiento = config->MULTIPROCESAMIENTO;
	multiprocesamientoUsado = 0;
	retardoEjecucion = config->SLEEP_EJECUCION;

	hilosActivos = 0;

	// Inicializo array de semaforos para determinar la cant de hilos a ejecutar en base a la cantidad
	// 		de requests que haya en la cola de ready

//	if(multiprocesamiento !=0){
//		arraySemaforos = malloc(sizeof(sem_t) * multiprocesamiento);
//
//		for(int i=0; i < multiprocesamiento;i++){
//			sem_init(&arraySemaforos[i], 0, 0);
//		}
//	}



}

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
 * 		- Resto de las funciones (con codigo harcodeado) -> HECHO + pequeñas modificaciones del comando RUN
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

void hiloConsola(){
	char* operacion = readline(">");

	while(1){
		if(!instruccionSeaSalir(operacion)){
			break;
			// Añadir semaforo para continuar y terminar el hilo principal
		}
		else{
			// Acordarse de descomentar una cosa de: crearProcesoYMandarloAReady(operacion); ---->>>> agregarRequestAlProceso(proceso, operacion);
			// Es muy importante!!!
			crearProcesoYMandarloAReady(operacion);
			// free(operacion); // Para esto es importante
		}
	}
//	free(operacion);
}

int main(void) {
	iniciarVariablesKernel();
	char*operacion;
//	agregarHiloAListaHilosEInicializo(listaHilos);

	operacion = readline(">");
	while(!instruccionSeaSalir(operacion)){
		crearProcesoYMandarloAReady(operacion);
//		deReadyAExec();

		// Por ahora lo hago con un solo proceso y lo hago manual
		ejecutarProcesos();
		funcionThread(NULL);

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



