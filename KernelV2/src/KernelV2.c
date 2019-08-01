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
#include "APIKernel.h"

/*
 * Estado del Kernel:
 *
 * El Kernel anda, en general, bastante bien
 *
 * TODO: (Cosas boludas)
 * 		- Mejorar el manejo de errores (agregarle cosas) + VALIDACIONES
 *		- Solucionar algunas condiciones de carrera (creo)
 *
 */

void iniciarVariablesKernel() {
	log_master = malloc(sizeof(logStruct));
	inicializarLogStruct();
	cargarConfigKernel();

	crearMetrics(&metricas);
	crearMetrics(&copiaMetricas);

	sem_init(&mutex_colaReadyPOP, 0, 1);
	sem_init(&mutex_colaReadyPUSH, 0, 1);
	sem_init(&mutex_id_proceso, 0, 1);
	sem_init(&bin_main, 0, 0);
	sem_init(&fin, 0, 0);
	sem_init(&cantProcesosColaReady, 0, 0);
	sem_init(&semMetricas, 0, 1);
	pthread_mutex_init(&mutexListaMemorias, NULL);

	hayMetricas = false;
	puedeHaberRequests = true;
	idMemoria = 1;

	srand(time(NULL));

	colaReady = queue_create();
	listaHilos = list_create();
	listaMemorias = list_create();

	//hardcodearInfoMemorias();
	listaInfoTablas = list_create();
	//hardcodearListaMetadataTabla();
	multiprocesamiento = config->MULTIPROCESAMIENTO;
	multiprocesamientoUsado = 0;

	cantSelects = 0;
	cantInserts = 0;
	timestampSelectAlIniciar = 0;
	timestampSelectAlFinalizar = 0;
	timestampInsertAlIniciar = 0;
	timestampInsertAlFinalizar = 0;

	hilosActivos = 0;
	idHilo = 0;

	arrayDeHilos = malloc(sizeof(pthread_t) * multiprocesamiento);
}

void* iniciarhiloMetrics(void* args) {

	while (puedeHaberRequests) {
		usleep(30000 * 1000); // algo asi...
		if (puedeHaberRequests) {
			sem_wait(&semMetricas);
			calcularMetrics();
			sem_post(&semMetricas);
			sem_wait(&semMetricas);
			copiarMetrics();
			sem_post(&semMetricas);
			sem_wait(&semMetricas);
			imprimirMetrics(metricas);
			sem_post(&semMetricas);
			sem_wait(&semMetricas);
			reiniciarMetrics(&metricas);
			sem_post(&semMetricas);
		}
	}
	return NULL;
}

void iniciarConsolaKernel() {

	char* operacion;

	while (puedeHaberRequests) {
		operacion = readline(">");
		// Para salir, escriba SALIR -> esta operacion se acumula en la cola de Ready y se encarga de destruir los hilos, etc
		if (!instruccionSeaMetrics(operacion)) {
			if (strlen(operacion) != 0) {
				crearProcesoYMandarloAReady(operacion);
				sem_post(&bin_main);
			}
		} else {
			imprimirMetrics(copiaMetricas);
			free(operacion);
		}
	}
}
void iniciarHiloMetadataRefresh() {
	while (true) {
		usleep(config->METADATA_REFRESH * 1000);
		log_trace(log_master->logTrace, "Iniciar Describe global");
		if (consolaDescribe(NULL) == SUPER_ERROR)
			log_error(log_master->logError, "Fallo el describe global automatico");

	}
}

void iniciarHiloGossiping() {
	log_info(log_master->logInfo, "Iniciando hilo de gossiping");

	while (true) {
		usleep(tiempoGossiping * 1000);
		log_info(log_master->logInfo, "Descubriendo memorias..");
		if (conocerMemorias() == SUPER_ERROR) {

			log_error(log_master->logError, "Fallo conocer memorias");
		}

	}

}

void cerrarKernel() {

	// ELiminar memoria (Esto solo se puede llegar una vez que el usuario haya escrito SALIR en consola)
	destruirElementosMain(listaHilos, colaReady);
	destruirListaMemorias();

	sem_wait(&semMetricas);
	destruirMetrics(&metricas);
	sem_post(&semMetricas);
	sem_wait(&semMetricas);
//	destruirMetrics(&copiaMetricas);
//	list_destroy(copiaMetricas.diferenciaDeTiempoReadLatency);
//	list_destroy(copiaMetricas.diferenciaDeTiempoWriteLatency);
	sem_post(&semMetricas);

	free(config);
	config_destroy(kernelConfig);
	free(arrayDeHilos);
	log_info(log_master->logInfo, "Consola terminada");
	destruirLogStruct(log_master);
	printf("Llego al final ok\n");
}

int main(void) {
	iniciarVariablesKernel();

	conocerMemorias();

	// Hilo de Gossiping
	pthread_create(&hiloGossiping, NULL, (void*) iniciarHiloGossiping, NULL);
	pthread_detach(hiloGossiping);

	// Hilo de metrics
	pthread_create(&hiloMetrics, NULL, (void*) iniciarhiloMetrics, NULL);
	pthread_detach(hiloMetrics);

	// Hilo de Describe global
	pthread_create(&hiloMetadataRefresh, NULL, (void*) iniciarHiloMetadataRefresh, NULL);
	pthread_detach(hiloMetadataRefresh);

	// Hilo de consola
	pthread_create(&hiloConsola, NULL, (void*) iniciarConsolaKernel, NULL);
	pthread_detach(hiloConsola);

	// Este semaforo es muy importante
	sem_wait(&bin_main);

	// Hilo de multiprocesamiento

	for (int i = 0; i < multiprocesamiento; i++) {
		arrayDeHilosPuntero = arrayDeHilos;
		pthread_create(&arrayDeHilosPuntero[i], NULL, (void*) iniciarMultiprocesamiento, NULL);
	}

	for (int i = 0; i < multiprocesamiento; i++) {
		pthread_detach(arrayDeHilosPuntero[i]);
	}

	sem_wait(&fin);

	usleep(5 * 1000);
	cerrarKernel();

	return EXIT_SUCCESS;
}

