/*
 * KernelMainFunctions.c
 *
 *  Created on: 17 jun. 2019
 *      Author: utnso
 */

#include "Kernel.h"
#include "APIKernel.h"

void destruirElementosMain(t_list* lista, t_queue* cola) {
	list_destroy_and_destroy_elements(lista, (void*) destruirProceso);
	queue_destroy_and_destroy_elements(cola, (void*) destruirProceso);
}

void destruirLogStruct(logStruct* log_master) {
	log_destroy(log_master->logInfo);
	log_destroy(log_master->logError);
	log_destroy(log_master->logTrace);
	free(log_master);
}

procExec* newProceso() {
	procExec* proceso = malloc(sizeof(procExec));
	proceso->script = list_create();
	proceso->contadorRequests = 0;
//	list_add(proceso->script, instruccion);
	return proceso;
}

void destruirProceso(procExec* proceso) {
	list_destroy_and_destroy_elements(proceso->script, (void*) free);
	free(proceso);
}

void deNewAReady(procExec* proceso) {
	sem_wait(&mutex_colaReadyPUSH);
	queue_push(colaReady, proceso);
	sem_post(&cantProcesosColaReady);
	sem_post(&mutex_colaReadyPUSH);
}

void deReadyAExec() {
	procExec* procesoAEjecutar = queue_pop(colaReady);
	list_add(listaHilos, procesoAEjecutar);
}

int get_campo_config_int(t_config* archivo_configuracion, char* nombre_campo) {
	int valor;
	if (config_has_property(archivo_configuracion, nombre_campo)) {
		valor = config_get_int_value(archivo_configuracion, nombre_campo);
		log_info(log_master->logInfo, "El %s es: %i", nombre_campo, valor);
		return valor;
	}
	return (int) NULL;
}

char* get_campo_config_string(t_config* archivo_configuracion, char* nombre_campo) {
	char* valor;
	if (config_has_property(archivo_configuracion, nombre_campo)) {
		valor = config_get_string_value(archivo_configuracion, nombre_campo);
		log_info(log_master->logInfo, "El %s es: %s", nombre_campo, valor);
		return valor;
	}
	return NULL;
}

void freeConfigKernel() {
	if (config->IP_MEMORIA != NULL) {
		free(config->IP_MEMORIA);
	}

	free(config);
}

void cargarConfigKernel() {
	log_info(log_master->logInfo, "Levantando archivo de configuracion del proceso Kernel ");

	if (config != NULL) {
		freeConfigKernel();
	}
	config = malloc(sizeof(t_config_kernel));
	kernelConfig = config_create(RUTA_CONFIG_KERNEL);

	if (kernelConfig == NULL) {
		perror("Error ");
		log_error(log_master->logError, "Problema al abrir el archivo");
	}

	config->IP_MEMORIA = get_campo_config_string(kernelConfig, "IP_MEMORIA");
	config->PUERTO_MEMORIA = get_campo_config_int(kernelConfig, "PUERTO_MEMORIA");
	config->QUANTUM = get_campo_config_int(kernelConfig, "QUANTUM");
	config->MULTIPROCESAMIENTO = get_campo_config_int(kernelConfig, "MULTIPROCESAMIENTO");
	config->METADATA_REFRESH = get_campo_config_int(kernelConfig, "METADATA_REFRESH");
	config->SLEEP_EJECUCION = get_campo_config_int(kernelConfig, "SLEEP_EJECUCION");

	log_info(log_master->logInfo, "Archivo de configuracion del proceso Kernel levantado ");

//<<<<<<< HEAD
//	config_destroy(kernelConfig);  // Si lo ponemos, se pierden los datos
//	listenArchivo(RUTA_CONFIG_KERNEL, cargarConfigKernel);
//=======
//	config_destroy(kernelConfig);
	//config_destroy(kernelConfig);  // Si lo ponemos, se pierden los datos
	listenArchivo("/home/utnso/tp-2019-1c-Los-Sisoperadores/KernelV2/config/", cargarConfigKernel);
//>>>>>>> d47fbace383f93ecea3aa5815e4b57dce5ca6366
}

int cantidadParametros(char ** palabras) {
	int i = 1;
	while (palabras[i] != NULL) {
		i++;
	}
	return i - 1;
}

int obtenerMemoriaSegunTablaYKey(int key, char* nombreTabla, t_protocolo protocolo, infoMemoria* memoriaAEnviar) {
	if (memoriaAEnviar != NULL) {

		if (protocolo == SELECT) {
			(memoriaAEnviar->cantSelectsEjecutados)++;
		}
		if (protocolo == INSERT) {
			(memoriaAEnviar->cantInsertEjecutados)++;
		}

		log_trace(log_master->logTrace, "Los datos obtenidos son:");
		imprimirCriterio(memoriaAEnviar->criterios);
		log_trace(log_master->logTrace, "Id de la memoria: %d", memoriaAEnviar->id);
	} else {
		log_error(log_master->logError, "Error: no existe memoria con ese criterio o todavia no hay memorias con el criterio de la tabla");
		return SUPER_ERROR;
	}
	return TODO_OK;
}

void agregarRequestAlProceso(procExec* proceso, char* operacion) {
	list_add(proceso->script, operacion);
}

//void* funcionThread(void* args) {
//	sem_wait(&ejecutarHilos);
//
//	sem_wait(&mutex_colaReadyPOP);
//	procExec* proceso = NULL;
//	proceso = queue_pop(colaReady);
//	sem_post(&mutex_colaReadyPOP);
//
//	void _correrProceso(char*request) {
//		procesarInputKernel(request);
//		cantRequestsEjecutadas++;
//	}
//
//	list_iterate(proceso->script, (void*) _correrProceso);
//	destruirProceso(proceso);
//
//	// Creo que aca se liberan los recursos del proceso
//	return NULL;
//}

//void otorgarId(bool* tieneID) {
//	if (!(*tieneID)) {
//		sem_wait(&mutex_id_proceso);
//		idHilo++;
//		sem_post(&mutex_id_proceso);
//
//		*tieneID = true;
//
//		if (idHilo == multiprocesamientoUsado)
//			sem_post(&bin_main);
//	}
//
//}

//void desbloquearHilos() {
//	int tamCola = queue_size(colaReady);
//	if (tamCola <= multiprocesamiento)
//		for (int i = 0; i < tamCola; i++)
//			sem_post(&arraySemaforos[i]);
//}

bool interrupcionPorEstado(estadoProceso estado) {
	return estado == ERROR;
}

procExec* obtenerProcesoDeColaReady() {
	sem_wait(&cantProcesosColaReady);

	sem_wait(&mutex_colaReadyPOP);
	procExec* proceso = NULL;
	proceso = queue_pop(colaReady);
	sem_post(&mutex_colaReadyPOP);

	return proceso;
}

void* iniciarMultiprocesamiento(void* args) {
	estadoProceso estado = OK;
	int cantRequestsProceso = 0;
	int cantRequestsEjecutadas = 0;
	procExec* proceso = NULL;
	int cantRequestsEjecutadasPorQuantum = 0;
	int quantum = 1;
	int retardoEjecucion = 0;

	/*
	 * Se que no se entiende mucho
	 *
	 * Intente hacer un refactor pero no se porque no me funcionaba correctamente despues
	 */

	do {
		quantum = config->QUANTUM;
		retardoEjecucion = config->SLEEP_EJECUCION;
		proceso = obtenerProcesoDeColaReady();

		if (proceso != NULL) {

			cantRequestsProceso = list_size(proceso->script);

			// Ahora evaluamos cada una de las requests del script
			// Lo hago por un for y list_get en vez de list_itearate porque necesito varias condiciones

			for (cantRequestsEjecutadas = proceso->contadorRequests;
					estado == OK && cantRequestsEjecutadas < cantRequestsProceso && cantRequestsEjecutadasPorQuantum < quantum;
					cantRequestsEjecutadas++) {
				usleep(retardoEjecucion * 1000);
				estado = procesarInputKernel(list_get(proceso->script, cantRequestsEjecutadas));
				cantRequestsEjecutadasPorQuantum++;
			}

			// Evaluo condiciones

			if (!interrupcionPorEstado(estado) && cantRequestsEjecutadas == cantRequestsProceso) {

				log_info(log_master->logInfo, "El script termino de ejecutarse correctamente");
				if (cantRequestsEjecutadasPorQuantum == quantum) {
//					usleep(retardoEjecucion * 1000);
					log_info(log_master->logInfo, "Llega a fin de quantum.\nDesalojando");
				}
				cantRequestsEjecutadasPorQuantum = 0;
				destruirProceso(proceso);
				printf("\n>");

			}
			if (!interrupcionPorEstado(estado) && cantRequestsEjecutadasPorQuantum == quantum
					&& cantRequestsEjecutadas < cantRequestsProceso) {

				log_info(log_master->logInfo, "Llega a fin de quantum.\nDesalojando");
				proceso->contadorRequests = cantRequestsEjecutadas;
				cantRequestsEjecutadasPorQuantum = 0;
//				usleep(retardoEjecucion * 1000);
				deNewAReady(proceso);
			}
			if (interrupcionPorEstado(estado)) {
				log_error(log_master->logError, "Error: Una request no se pudo cumplir");
				log_error(log_master->logError, "Destruyendo proceso");
				destruirProceso(proceso);
				log_error(log_master->logError, "Proceso destruido");
				printf("\n>");
			}

		}

		estado = OK;

	} while (puedeHaberRequests);

	return NULL;
}

//void ejecutarProcesos() {
//	sem_post(&ejecutarHilos);
//}

void inicializarLogStruct() {
	log_master->logInfo = log_create((char*) INFO_KERNEL, "Kernel Info Logs", 1, LOG_LEVEL_INFO);
	log_master->logError = log_create((char*) ERRORES_KERNEL, "Kernel Error Logs", 1, LOG_LEVEL_ERROR);
	log_master->logTrace = log_create((char*) TRACE_KERNEL, "Kernel Trace Logs", 1, LOG_LEVEL_TRACE);
}

infoMemoria* obtenerMemoriaAlAzar() {
//	srand(time(NULL));
	int numeroAleatorio = rand() % list_size(listaMemorias);
	return list_get(listaMemorias, numeroAleatorio);
}

infoMemoria* obtenerMemoriaAlAzarParaFunciones() {
	infoMemoria* memoriaAlAzar = NULL;

	memoriaAlAzar = obtenerMemoriaAlAzar();
	log_trace(log_master->logTrace, "El id de la memoria obtenida es: %d", memoriaAlAzar->id);

	return memoriaAlAzar;
}

infoMemoria* obtenerMemoria(char* nombreTabla, int key) {
	consistencia consistenciaDeTabla = obtenerConsistenciaDe(nombreTabla);

	if (consistenciaDeTabla == ERROR_CONSISTENCIA) {
		log_error(log_master->logError, "Error al obtener la consistencia: tabla no existe o error en la consistencia");
		return NULL;
	}

	return obtenerMemoriaSegunConsistencia(consistenciaDeTabla, key);
}

consistencia obtenerConsistenciaDe(char* nombreTabla) {
	bool findByName(metadataTabla* tabla) {
		return strcmp(tabla->nombreTabla, nombreTabla) == 0;
	}

	metadataTabla* tabla = list_find(listaMetadataTabla, (void*) findByName);

	if (tabla == NULL)
		return ERROR_CONSISTENCIA;

	return tabla->consistencia;
}

infoMemoria* obtenerMemoriaSegunConsistencia(consistencia consistenciaDeTabla, int key) {
	t_list* memoriasEncontradas = NULL;
	infoMemoria* memoriaPosta = NULL;

	bool _condicion(infoMemoria*memoria, consistencia cons) {
		return verificarCriterio(memoria->criterios, cons);
	}
	bool condicionParaEncontrarMemorias(void* memoria) {
		return _condicion(memoria, consistenciaDeTabla);
	}
	memoriasEncontradas = list_filter(listaMemorias, condicionParaEncontrarMemorias);

	switch (consistenciaDeTabla) {
	case SC:
		memoriaPosta = list_get(memoriasEncontradas, 0);
		break;
	case SHC:
		memoriaPosta = resolverUsandoFuncionHash(memoriasEncontradas, key);
		break;
	case EC:
		memoriaPosta = resolverAlAzar(memoriasEncontradas);
		break;
	default:
		log_error(log_master->logError, "Error: en obtenerMemoriaSegunConsistencia");
	}
	list_destroy(memoriasEncontradas);
	return memoriaPosta;

}

infoMemoria* resolverUsandoFuncionHash(t_list* memoriasEncontradas, int key) {
	int posicionMemoria = funcionHash(memoriasEncontradas, key);
	return list_get(memoriasEncontradas, posicionMemoria);
}

int funcionHash(t_list* memoriasEncontradas, int key) {
	int size = list_size(memoriasEncontradas);
	return key % size;
}

infoMemoria* resolverAlAzar(t_list* memoriasEncontradas) {
//	srand(time(NULL));
	int randomNumber = rand() % list_size(memoriasEncontradas);

	return list_get(memoriasEncontradas, randomNumber);
}

infoMemoria* newInfoMemoria(char* ip, int puerto, int id) {
	infoMemoria* memoria = malloc(sizeof(infoMemoria));
	memoria->id = idMemoria;
	idMemoria++;
	for (int i = 0; i < 4; i++) {
		(memoria->criterios)[i] = false;
	}
	memoria->ip = string_duplicate(ip);
	memoria->cantInsertEjecutados = 0;
	memoria->cantSelectsEjecutados = 0;
	memoria->puerto = puerto;
	memoria->memoryLoadUnaMemoria = 0;
	memoria->id = id;
	return memoria;
}

void asignarCriterioMemoria(infoMemoria* memoria, consistencia cons) {
	if (!haySC && cons == SC) {
		(memoria->criterios)[0] = true;
		haySC = true;
	} else if (cons == SHC)
		(memoria->criterios)[1] = true;
	else if (cons == EC)
		(memoria->criterios)[2] = true;
	else if ((haySC && cons == SC) || cons == ERROR_CONSISTENCIA)
		(memoria->criterios)[3] = true;
}

void destruirInfoMemoria(infoMemoria* memoria) {
	free(memoria->ip);
	free(memoria);
}

void destruirListaMemorias() {
	list_destroy_and_destroy_elements(listaMemorias, (void*) destruirInfoMemoria);
}

void crearProcesoYMandarloAReady(char* operacion) {
	procExec* proceso = newProceso();
	agregarRequestAlProceso(proceso, operacion);
	deNewAReady(proceso);
}

int conocerMemorias() {
	log_info(log_master->logInfo, "Descubriendo memorias..");
	int socketMemoria = ConectarAServidor(config->PUERTO_MEMORIA, config->IP_MEMORIA);
	if (socketMemoria == -1) {
		log_error(log_master->logError, "Nuestra memoria seed no esta conectada");
		return SUPER_ERROR;
	}

	EnviarDatosTipo(socketMemoria, KERNEL, NULL, 0, TABLA_GOSSIPING);
	Paquete paquete;
	int codRecibir = 0;

	RecibirPaquete(socketMemoria, &paquete);
	tiempoGossiping = atoi(paquete.mensaje);

	free(paquete.mensaje);

	while ((codRecibir = RecibirPaquete(socketMemoria, &paquete)) > 0) {

		char** response = string_split(paquete.mensaje, " ");
		infoMemoria* memoriaConocida = newInfoMemoria(response[0], atoi(response[1]), atoi(response[2]));
		agregarMemoriaConocida(memoriaConocida);
		//log_info(log_master->logInfo, "Memoria Descubierta IP:%s PUERTO:%d MEMORY_NUMBER:%d", memoriaConocida->ip, memoriaConocida->puerto,
		//		memoriaConocida->id);
		free(paquete.mensaje);
		freePunteroAPunteros(response);
	}
	if (codRecibir < 0)
		return SUPER_ERROR;
	return TODO_OK;
}

void agregarMemoriaConocida(infoMemoria* memoria) {
	bool findByNumber(infoMemoria* memoriaActual) {
		return memoria->id == memoriaActual->id;
	}
	if (!list_any_satisfy(listaMemorias, (void*) findByNumber)) {
		list_add(listaMemorias, memoria);
	}
}

