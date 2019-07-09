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
	sem_init(&mutex_colaReadyPOP, 0, 1);
	sem_init(&mutex_colaReadyPUSH, 0, 1);
	sem_init(&mutex_id_proceso, 0, 1);
	sem_init(&bin_main, 0, 0);
	sem_init(&fin, 0, 0);
	sem_init(&cantProcesosColaReady, 0, 0);

	cantRequestsEjecutadas = 0;
	haySC = false;
	puedeHaberRequests = true;
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
	idHilo=0;

	// Inicializo array de semaforos para determinar la cant de hilos a ejecutar en base a la cantidad
	// 		de requests que haya en la cola de ready

	if(multiprocesamiento !=0){
		arraySemaforos = malloc(sizeof(sem_t) * multiprocesamiento);

		for(int i=0; i < multiprocesamiento;i++){
			sem_init(&arraySemaforos[i], 0, 0);
		}
	}



}

/*
 * Estado del Kernel:
 *
 * Se pueden acceder a todos los comandos del Kernel e indicar si hay algun comando invalido (API Kernel)
 *
 * Se puede asignar un criterio a una memoria (comando ADD)
 *
 * Se pueden loggear los errores correctamente y administrativamente (Muy importante!!)
 *
 * TODO:
 * 		- Manejo de errores
 * 		- Round Robin (Creo que  ya esta resuelto)
 *		- Multiprocesamiento
 *
 */

void iniciarConsolaKernel(){
	char* operacion;
	operacion = readline(">");

	while(1){
//		if(instruccionSeaSalir(operacion)){
////			puedeHaberRequests = false;
////			sem_post(&cantProcesosColaReady);
////			sem_post(&fin);
//			break;
//			// Añadir semaforo para continuar y terminar el hilo principal
//		}
//		else{
//			// Acordarse de descomentar una cosa de: crearProcesoYMandarloAReady(operacion); ---->>>> agregarRequestAlProceso(proceso, operacion);
//			// Es muy importante!!!
//			crearProcesoYMandarloAReady(operacion);
//			desbloquearHilos();
//			sem_post(&bin_main);
//			// free(operacion); // Para esto es importante
//		}



		crearProcesoYMandarloAReady(operacion);
//		desbloquearHilos();
		sem_post(&bin_main);
		operacion = readline(">");
	}
//	free(operacion);
}

void inicioKernelUnProcesador() {
	char* operacion;
	//	agregarHiloAListaHilosEInicializo(listaHilos);
	operacion = readline(">");
	while (!instruccionSeaSalir(operacion)) {
		crearProcesoYMandarloAReady(operacion);
		//		deReadyAExec();
		// Por ahora lo hago con un solo proceso y lo hago manual
//		ejecutarProcesos();
		// Prueba multiprocesamiento
//		desbloquearHilos();
		nuevaFuncionThread(NULL);
		//		funcionThread(NULL);
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
}

int main(void) {
	pthread_t hiloConsola;
	pthread_t hiloMultiprocesamiento;
	iniciarVariablesKernel();
//	inicioKernelUnProcesador();

	// Hilo de consola

	pthread_create(&hiloConsola, NULL, (void*)iniciarConsolaKernel, NULL);
	pthread_detach(hiloConsola);

//	iniciarConsolaKernel();

	// Este semaforo es muy importante
	sem_wait(&bin_main);

	// Hilo de multiprocesamiento
	pthread_create(&hiloMultiprocesamiento, NULL, (void*)nuevaFuncionThread, NULL);
	pthread_detach(hiloMultiprocesamiento);

	sem_wait(&fin);

	// ELiminar memoria (Esto solo se puede llegar una vez que el usuario haya escrito SALIR en consola)

	destruirArraySemaforos();

//	for(int i = multiprocesamiento; i > 0; i--)
//		free(&arraySemaforos[i]);

//	config_destroy(config);
	return EXIT_SUCCESS;
}



