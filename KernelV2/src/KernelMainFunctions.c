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

	listenArchivo("/home/utnso/tp-2019-1c-Los-Sisoperadores/KernelV2/config/", cargarConfigKernel);
}

int cantidadParametros(char ** palabras) {
	int i = 1;
	while (palabras[i] != NULL) {
		i++;
	}
	return i - 1;
}

int actualizarMetricasDeMemoria(int key, char* nombreTabla, t_protocolo protocolo, infoMemoria* memoriaAEnviar) {

	if (protocolo == SELECT) {
		(memoriaAEnviar->cantSelectsEjecutados)++;
	}
	if (protocolo == INSERT) {
		(memoriaAEnviar->cantInsertEjecutados)++;
	}

	imprimirCriterio(memoriaAEnviar);

	return TODO_OK;
}

void agregarRequestAlProceso(procExec* proceso, char* operacion) {
	list_add(proceso->script, operacion);
}

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

				log_trace(log_master->logTrace, "El script termino de ejecutarse correctamente");
				if (cantRequestsEjecutadasPorQuantum == quantum) {
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

void inicializarLogStruct() {
	log_master->logInfo = log_create((char*) INFO_KERNEL, "Kernel Info Logs", 1, LOG_LEVEL_INFO);
	log_master->logError = log_create((char*) ERRORES_KERNEL, "Kernel Error Logs", 1, LOG_LEVEL_ERROR);
	log_master->logTrace = log_create((char*) TRACE_KERNEL, "Kernel Trace Logs", 1, LOG_LEVEL_TRACE);
}

infoMemoria* obtenerMemoriaAlAzar(t_list* memorias) {
	int numeroAleatorio = rand() % list_size(memorias);
	return list_get(memorias, numeroAleatorio);
}

infoMemoria* obtenerMemoria(char* nombreTabla, int key) {
	pthread_mutex_lock(&mutexListaMemorias);
	log_info(log_master->logInfo, "Obteniendo memoria para tabla %s con key: %d", nombreTabla, key);
	infoTabla* tabla = obtenerTablaByNombretabla(nombreTabla);

	if (tabla == NULL) {
		log_error(log_master->logError, "Error al obtener la consistencia: tabla no existe");
		pthread_mutex_unlock(&mutexListaMemorias);
		return NULL;
	}

	t_consistencia criterio = tabla->consitencia;
	infoMemoria* memoriaObtenida = obtenerMemoriaSegunCriterio(criterio, key);
	if (memoriaObtenida != NULL) {
		log_info(log_master->logInfo, "Memoria obtenida: %d", memoriaObtenida->id);
	} else {
		log_error(log_master->logError, "No existe memoria para criterio %s", getConsistenciaCharByEnum(criterio));
	}

	pthread_mutex_unlock(&mutexListaMemorias);
	return memoriaObtenida;

}

infoMemoria* obtenerMemoriaSegunCriterio(t_consistencia consistenciaDeTabla, int key) {
	infoMemoria* memoriaCorrespondiente = NULL;
	t_list* memoriasFiltradas = filterMemoriasByCriterio(consistenciaDeTabla);
	if (!list_is_empty(memoriasFiltradas)) {

		switch (consistenciaDeTabla) {
		case STRONG:
			memoriaCorrespondiente = list_get(memoriasFiltradas, 0);
			break;
		case STRONG_HASH:
			memoriaCorrespondiente = list_get(memoriasFiltradas, funcionHash(memoriasFiltradas, key));
			break;
		case EVENTUAL:
			memoriaCorrespondiente = obtenerMemoriaAlAzar(memoriasFiltradas);
			break;
		default:
			log_error(log_master->logError, "Error: en obtenerMemoriaSegunCriterio");
		}
	}
	list_destroy(memoriasFiltradas);
	return memoriaCorrespondiente;

}

int funcionHash(t_list* memoriasSHC, int key) {
	int size = list_size(memoriasSHC);
	return key % size;
}

infoTabla* obtenerTablaByNombretabla(char* nombreTabla) {

	bool byName(infoTabla* tablaActual) {
		return strcmp(nombreTabla, tablaActual->nombreTabla) == 0;
	}
	return list_find(listaInfoTablas, (void*) byName);
}

t_list* filterMemoriasByCriterio(t_consistencia criterioBuscado) {

	bool byCriterio(infoMemoria* memoriaActual) {
		bool isCriterioBuscado(t_consistencia criterioActual) {
			return criterioActual == criterioBuscado;
		}

		return list_any_satisfy(memoriaActual->criterios, (void*) isCriterioBuscado);
	}

	return list_filter(listaMemorias, (void*) byCriterio);
}

infoMemoria* newInfoMemoria(char* ip, int puerto, int id) {
	infoMemoria* memoria = malloc(sizeof(infoMemoria));
	memoria->id = idMemoria;
	idMemoria++;
	memoria->criterios = list_create();
	memoria->ip = string_duplicate(ip);
	memoria->cantInsertEjecutados = 0;
	memoria->cantSelectsEjecutados = 0;
	memoria->puerto = puerto;
	memoria->memoryLoadUnaMemoria = 0;
	memoria->id = id;
	return memoria;
}

int asignarCriterioMemoria(infoMemoria* memoria, t_consistencia consistencia) {

	int succcess = SUPER_ERROR;
	bool mismoCriterio(t_consistencia criterio) {
		return criterio == consistencia;
	}
	if (list_any_satisfy(memoria->criterios, (void*) mismoCriterio)) {
		log_error(log_master->logError, "La memoria %d ya tiene asignado el criterio %s", memoria->id,
				getConsistenciaCharByEnum(consistencia));
	} else if (consistencia == STRONG && haySC()) {
		log_error(log_master->logError, "Ya existe otra memoria SC");
	} else {
		list_add(memoria->criterios, (void*) consistencia);
		succcess = TODO_OK;
	}
	return succcess;
}

bool haySC() {
	t_list* listaAux = filterMemoriasByCriterio(STRONG);
	bool result = !list_is_empty(listaAux);
	list_destroy(listaAux);
	return result;
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
	pthread_mutex_lock(&mutexListaMemorias);
	int socketMemoria = ConectarAServidor(config->PUERTO_MEMORIA, config->IP_MEMORIA);
	while (socketMemoria == -1) {
		infoMemoria* memoria = obtenerMemoriaAlAzar(listaMemorias);
		socketMemoria = ConectarAServidor(memoria->puerto, memoria->ip);
		usleep(20);
	}

	EnviarDatosTipo(socketMemoria, KERNEL, NULL, 0, TABLA_GOSSIPING);
	Paquete paquete;
	int codRecibir = 0;

	if (RecibirPaquete(socketMemoria, &paquete) > 0) {
		tiempoGossiping = atoi(paquete.mensaje);
		free(paquete.mensaje);
	} else {
		return TODO_OK;
		//esto no es un OK un carajo pero basta de tirar errores
	}

	while ((codRecibir = RecibirPaquete(socketMemoria, &paquete)) > 0) {

		char** response = string_split(paquete.mensaje, " ");
		infoMemoria* memoriaConocida = newInfoMemoria(response[0], atoi(response[1]), atoi(response[2]));
		agregarMemoriaConocida(memoriaConocida);
		free(paquete.mensaje);
		freePunteroAPunteros(response);
	}
	filtrarMemorias();
	pthread_mutex_unlock(&mutexListaMemorias);
	listarMemorias();
	if (codRecibir < 0)
		return SUPER_ERROR;
	log_info(log_master->logTrace, "El tamaño de la listaMemorias es: %d", list_size(listaMemorias));
	return TODO_OK;
}

void agregarMemoriaConocida(infoMemoria* memoria) {
	bool findByNumber(infoMemoria* memoriaActual) {
		return memoria->id == memoriaActual->id;
	}
	if (!list_any_satisfy(listaMemorias, (void*) findByNumber)) {
		list_add(listaMemorias, memoria);
	} else {
		freeInfoMemoria(memoria);
	}
}

