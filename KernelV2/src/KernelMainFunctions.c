/*
 * KernelMainFunctions.c
 *
 *  Created on: 17 jun. 2019
 *      Author: utnso
 */

#include "Kernel.h"
#include "APIKernel.h"

void destruirElementosMain(t_list* lista, t_queue* cola){
	list_destroy_and_destroy_elements(lista, (void*) free);
	queue_destroy_and_destroy_elements(cola, (void*) free);
//	log_destroy(logger);
}

void destruirLogStruct(logStruct* log_master){
	log_destroy(log_master->logInfo);
	log_destroy(log_master->logError);
	log_destroy(log_master->logTrace);
	free(log_master);
}

procExec* newProceso(){
	procExec* proceso = malloc(sizeof(procExec));
	proceso->script=list_create();
//	proceso->estaEjecutandose=false;
//	list_add(proceso->script, instruccion);
	return proceso;
}

void destruirProceso(procExec* proceso){
	list_destroy_and_destroy_elements(proceso->script, (void*) free);
	free(proceso);
}

//void destruirProcesoExec(procExec* proceso){
//	list_destroy_and_destroy_elements(proceso->script, (void*) free);
//}

void deNewAReady(procExec* proceso){
	queue_push(colaReady, proceso);
}

void deReadyAExec(){
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
	return (int)NULL;
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

t_config_kernel *cargarConfig(char *ruta){
//	puts("!!!Hello World!!!");

	log_info(log_master->logInfo,
			"Levantando archivo de configuracion del proceso Kernel ");

	t_config_kernel* config = malloc(sizeof(t_config_kernel));
	t_config *kernelConfig = config_create(ruta);

	if (kernelConfig == NULL) {
		perror("Error ");

		log_error(log_master->logError, "Problema al abrir el archivo");
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

	log_info(log_master->logInfo,
			"Archivo de configuracion del proceso Kernel levantado ");

	config_destroy(kernelConfig);  // Si lo ponemos, se pierden los datos

	return config;
}

int cantidadParametros(char ** palabras) {
	int i = 1;
	while (palabras[i] != NULL) {
		i++;
	}
	return i - 1;
}


void obtenerMemoriaSegunTablaYKey(int key, char* nombreTabla) {
	// Parecido al INSERT, es decir, mando info a la memoria que cumple con la condicion, pero, a diferencia de la otra recibo una respuesta, que
	// es un value
	infoMemoria* memoriaAEnviar = obtenerMemoria(nombreTabla, key);

	if(memoriaAEnviar!=NULL){
		log_trace(log_master->logTrace, "Los datos obtenidos son:");
		//	log_trace(log_master->logTrace, "Criterio de la memoria: %d", mejorCriterioMemoria(memoriaAEnviar->criterios));
		imprimirCriterio(memoriaAEnviar->criterios);
		log_trace(log_master->logTrace, "Id de la memoria: %d",
				memoriaAEnviar->id);
	}
	else
		log_error(log_master->logError, "Error: no existe memoria con ese criterio");
}






void agregarRequestAlProceso(procExec* proceso, char* operacion){
	list_add(proceso->script, operacion);
}


void* funcionThread(void* args){
	sem_wait(&ejecutarHilos);

	sem_wait(&mutex_colaReady);
//	procExec* proceso = newProceso();
	procExec* proceso=NULL;
	proceso = queue_pop(colaReady);
	sem_post(&mutex_colaReady);

//	int tam_script = list_size(proceso->script);
//	for(int i=0; i< tam_script;i++){
//		procesarInput(list_get(proceso->script, i));
//		destruirProceso(proceso);
//	}

	void _correrProceso(char*request){
		procesarInput(request);
		cantRequestsEjecutadas++;
	}

	list_iterate(proceso->script, (void*) _correrProceso);
	destruirProceso(proceso);

	// Creo que aca se liberan los recursos del proceso
	return NULL;
}


void agregarHiloAListaHilosEInicializo(t_list* hilos){
//	for(int i=0; i < cantProcesos;i++){
//		pthread_create(&(hilos[i]), NULL, funcionThread, NULL);
//		pthread_detach(hilos[i]);
//		list_add(listaHilos, (void*)hilos[i]);
//	}

	void _agregarHilo(pthread_t*hilo){
		pthread_create(&*hilo, NULL, (void*)funcionThread, NULL);
		pthread_detach(*hilo);
//		pthread_join(*hilo, NULL);
		list_add(listaHilos, hilo);
	}

	list_iterate(hilos, (void*)_agregarHilo);
}


void ejecutarProcesos(){
	sem_post(&ejecutarHilos);
}




void inicializarLogStruct(){
	log_master->logInfo = log_create((char*) INFO_KERNEL, "Kernel Info Logs", 1,
			LOG_LEVEL_INFO);
	log_master->logError = log_create((char*) ERRORES_KERNEL, "Kernel Error Logs", 1,
			LOG_LEVEL_ERROR);
	log_master->logTrace = log_create((char*) TRACE_KERNEL, "Kernel Trace Logs", 1,
			LOG_LEVEL_TRACE);
}


infoMemoria* obtenerMemoriaAlAzar(){
//	srand(time(NULL));
	int numeroAleatorio = rand() % list_size(listaMemorias);
	return list_get(listaMemorias, numeroAleatorio);
}


infoMemoria* obtenerMemoriaAlAzarParaFunciones(){
	infoMemoria* memoriaAlAzar = NULL;

	memoriaAlAzar = obtenerMemoriaAlAzar();
	log_trace(log_master->logTrace, "El id de la memoria obtenida es: %d", memoriaAlAzar->id);

	return memoriaAlAzar;
	// Aca va la funcion enviar de las sockets
}


infoMemoria* obtenerMemoria(char* nombreTabla, int key){
	consistencia consistenciaDeTabla = obtenerConsistenciaDe(nombreTabla);

	return obtenerMemoriaSegunConsistencia(consistenciaDeTabla, key);
}


consistencia obtenerConsistenciaDe(char* nombreTabla){
	bool _condicion(metadataTablas* metadata, char*nombreTabla){
		return strcmp(nombreTabla, metadata->nombreTabla)==0;
	}
	bool condicionObtenerConsistencia(void* metadata){
		return _condicion(metadata, nombreTabla);
	}
	metadataTablas* metadata = NULL;
	metadata = list_find((t_list*)listaMetadataTabla,condicionObtenerConsistencia);
	return metadata->consistencia;
}


infoMemoria* obtenerMemoriaSegunConsistencia(consistencia consistenciaDeTabla, int key){
	t_list* memoriasEncontradas=NULL;
	infoMemoria* memoriaPosta=NULL;


	bool _condicion(infoMemoria*memoria, consistencia cons){
//		return cons == mejorCriterioMemoria(memoria->criterios);
		return verificarCriterio(memoria->criterios, cons);
	}
	bool condicionParaEncontrarMemorias(void* memoria){
		return _condicion(memoria, consistenciaDeTabla);
	}
	memoriasEncontradas = list_filter(listaMemorias, condicionParaEncontrarMemorias);



	switch(consistenciaDeTabla){
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

//	list_destroy_and_destroy_elements(memoriasEncontradas, (void*) free);
//	memoriasEncontradas = NULL;
	list_destroy(memoriasEncontradas);
	return memoriaPosta;

}


infoMemoria* resolverUsandoFuncionHash(t_list* memoriasEncontradas, int key){
	int posicionMemoria = funcionHash(memoriasEncontradas, key);
	return list_get(memoriasEncontradas, posicionMemoria);
}


int funcionHash(t_list* memoriasEncontradas, int key){
	int size = list_size(memoriasEncontradas);
	return key % size;
}


infoMemoria* resolverAlAzar(t_list* memoriasEncontradas){
//	srand(time(NULL));
	int randomNumber = rand() % list_size(memoriasEncontradas);

	return list_get(memoriasEncontradas, randomNumber);
}


infoMemoria* newInfoMemoria(){
	infoMemoria* memoria = malloc(sizeof(infoMemoria));
	memoria->id = idMemoria;
	idMemoria++;
	for(int i=0; i < 4; i++){
		(memoria->criterios)[i] = false;
	}
	memoria->ip = NULL;
	return memoria;
}


//consistencia mejorCriterioMemoria(bool* criterios){
//	if(criterios[0])
//		return SC;
//	else if(criterios[1])
//		return SHC;
//	else if(criterios[2])
//		return EC;
//
//	return ERROR_CONSISTENCIA;
//}



void asignarCriterioMemoria(infoMemoria* memoria, consistencia cons){
	if(!haySC && cons == SC){
		(memoria->criterios)[0] = true;
		haySC=true;
	}
	else if (cons == SHC)
		(memoria->criterios)[1] = true;
	else if (cons == EC)
		(memoria->criterios)[2] = true;
	else if((haySC && cons == SC) || cons == ERROR_CONSISTENCIA)
		(memoria->criterios)[3] = true;
}

void destruirListaMemorias(){
	list_destroy_and_destroy_elements(listaMemorias, (void*) free);
}
