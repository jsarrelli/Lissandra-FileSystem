/*
 * KernelMainFunctions.c
 *
 *  Created on: 17 jun. 2019
 *      Author: utnso
 */

#include "KernelHeader.h"

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

bool instruccionSeaSalir(char* operacion){
	if(strcmp(operacion, "SALIR")==0)
		return true;
	return false;
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

void procesarInput(char* linea) {
	char** comandos = string_n_split(linea, 2, " ");
	char* operacion = comandos[0];
	char* argumentos = comandos[1];
//	char **palabras = string_split(linea, " ");
//	int cantidad = cantidadParametros(comandos);
	if (strcmp(operacion, "INSERT") == 0) {
		//	INSERT [NOMBRE_TABLA] [KEY] “[VALUE]”
		log_trace(log_master->logTrace, "Se ha escrito el comando INSERT");
		consolaInsert(argumentos);
	} else if (strcmp(operacion, "SELECT") == 0) {
		//	SELECT [NOMBRE_TABLA] [KEY]
		log_trace(log_master->logTrace, "Se ha escrito el comando SELECT");
		consolaSelect(argumentos);
	} else if (strcmp(operacion, "CREATE") == 0) {
		//	CREATE [TABLA] [TIPO_CONSISTENCIA] [NUMERO_PARTICIONES] [COMPACTION_TIME]
		log_trace(log_master->logTrace, "Se ha escrito el comando CREATE");
		consolaCreate(argumentos);
		//consolaCreate(palabras,cantidad);
	} else if (strcmp(operacion, "DESCRIBE") == 0) {
		// DESCRIBE [NOMBRE_TABLA]
		// DESCRIBE
		log_trace(log_master->logTrace, "Se ha escrito el comando DESCRIBE");
		consolaDescribe(argumentos);
		//consolaDescribe(palabras,cantidad);
	} else if (strcmp(operacion, "DROP") == 0) {
		//	DROP [NOMBRE_TABLA]
		log_trace(log_master->logTrace, "Se ha escrito el comando DROP");
		consolaDrop(argumentos);
		//consolaDrop(palabras,cantidad);
	} else if (strcmp(operacion, "ADD") == 0) {
		//	ADD MEMORY [id] TO [consistencia]
		log_trace(log_master->logTrace, "Se ha escrito el comando ADD");
		procesarAdd(argumentos);
	} else if (strcmp(operacion, "SALIR") == 0) {
		log_trace(log_master->logTrace, "Finalizando consola");
	} else {
		log_trace(log_master->logTrace,
				"El comando no es el correcto. Por favor intente nuevamente");
	}

//	for(int i=0;i< cantidad;i++){
//		free()
//	}
	free(argumentos);
	free(operacion);
	free(comandos);

//	liberarPunteroDePunterosAChar(palabras);
//	free(palabras);
}

consistencia procesarConsistencia(char* palabra){
	if(strcmp(palabra, "SC")==0)
		return SC;
	else if(strcmp(palabra, "SHC")==0)
		return SHC;
	else if(strcmp(palabra, "EC")==0)
		return EC;
	return ERROR_CONSISTENCIA;
}

void comandoAdd(int id, consistencia cons){
	bool condicionAdd(int id, infoMemoria* memoria){
		return id == memoria->id;
	}
	bool _esCondicionAdd(void* memoria){
		return condicionAdd(id , memoria);
	}
	infoMemoria* memoriaEncontrada = malloc(sizeof(metadataTablas));
	if((memoriaEncontrada = list_find(listaMemorias, _esCondicionAdd))!=NULL){
		memoriaEncontrada->ccia = cons;
		log_info(log_master->logInfo, "Se ha asignado la consistencia a la memoria correctamente");
		log_trace(log_master->logTrace,
				"La consistencia de esta memoria es: %d y el id de esta memoria es %d",
				memoriaEncontrada->ccia, memoriaEncontrada->id);
	}
	else
		log_error(log_master->logError, "Problemas con el comando ADD");
}

void procesarAdd(char*argumento){
	char** valores = string_split(argumento, " ");
//	add(atoi(valores[1]), valores[3]);
	consistencia cons = procesarConsistencia(valores[3]);

	int id = atoi(valores[1]);
	comandoAdd(id, cons);
}

void consolaInsert(char*argumentos){
	char** valores = string_split(argumentos, "\""); //34 son las " en ASCII
	char** valoresAux = string_split(valores[0], " ");
	char* nombreTabla = valoresAux[0];
	char* key = valoresAux[1];
	char* value = valores[1];
	log_trace(log_master->logTrace, "El nombre de la tabla es: %s, su key es %s, y su value es: %s", nombreTabla, key, value);

//	infoMemoria* memoriaAEnviar = obtenerMemoria(nombreTabla, key);

	// Primero hay que obtener una memoria que cumpla con el criterio de la tabla.
		// Ahora, se usa las funciones sockets para enviar el mensaje a Memoria y luego a LFS; y listo (no requiere mensaje de retorno)
}

void consolaSelect(char*argumentos){
	char** valores = string_split(argumentos, " ");
	char* nombreTabla = valores[0];
	int key = atoi(valores[1]);

	log_trace(log_master->logTrace, "El nombre de la tabla es: %s, y la key es: %d", nombreTabla, key);

	// Parecido al INSERT, es decir, mando info a la memoria que cumple con la condicion, pero, a diferencia de la otra recibo una respuesta, que
		// es un value

	infoMemoria* memoriaAEnviar = obtenerMemoria(nombreTabla, key);

	log_trace(log_master->logTrace, "Los datos obtenidos son:");
	log_trace(log_master->logTrace, "Criterio de la memoria: %d", memoriaAEnviar->ccia);
	log_trace(log_master->logTrace, "Id de la memoria: %d", memoriaAEnviar->id);

}

void consolaCreate(char*argumentos){
	char** valores = string_split(argumentos, " ");
	char* nombreTabla = valores[0];
	char* consistenciaChar = valores[1];
	int cantParticiones = atoi(valores[2]);
	int tiempoCompactacion = atoi(valores[3]);

//	consistencia cons = procesarConsistencia(consistenciaChar);
	log_trace(log_master->logTrace,
			"El nombre de la tabla es: %s, la consistencia es: %s, la cantParticiones:%d, y el tiempoCompactacion es: %d",
			nombreTabla, consistenciaChar, cantParticiones, tiempoCompactacion);

	// En el caso del CREATE, como el de muchas otras funciones, se le manda la info a una memoria al azar y no recibe respuesta

}

void consolaDescribe(char*nombreTabla){
	if (nombreTabla==NULL){
		log_trace(log_master->logTrace, "Se pide la metadata de todos las tablas");
	}
	else{
		log_trace(log_master->logTrace, "Se pide la metadata de %s", nombreTabla);
	}

	// El DESCRIBE es igual que el CREATE y que el DROP
	comandoDescribe(nombreTabla);
}

void consolaDrop(char*nombreTabla){
	log_trace(log_master->logTrace, "Se desea elminar la tabla %s", nombreTabla);
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
		quantum++;
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

void hardcodearInfoMemorias(){
//	listaMemorias
	int idMemoria=1;
	infoMemoria* memoria1= malloc(sizeof(infoMemoria));
//	memoria1->ccia = SC;
	memoria1->id=idMemoria;
	list_add(listaMemorias, memoria1);
	idMemoria++;

	infoMemoria* memoria2 = malloc(sizeof(infoMemoria));
//	memoria2->ccia = SHC;
	memoria2->id = idMemoria;
	list_add(listaMemorias, memoria2);
	idMemoria++;

	infoMemoria* memoria3 = malloc(sizeof(infoMemoria));
//	memoria3->ccia = EC;
	memoria3->id = idMemoria;
	list_add(listaMemorias, memoria3);
	idMemoria++;

	infoMemoria* memoria4 = malloc(sizeof(infoMemoria));
	//	memoria3->ccia = EC;
	memoria4->id = idMemoria;
	list_add(listaMemorias, memoria4);
	idMemoria++;

	infoMemoria* memoria5 = malloc(sizeof(infoMemoria));
	//	memoria3->ccia = EC;
	memoria5->id = idMemoria;
	list_add(listaMemorias, memoria5);
	idMemoria++;

	infoMemoria* memoria6 = malloc(sizeof(infoMemoria));
	//	memoria3->ccia = EC;
	memoria6->id = idMemoria;
	list_add(listaMemorias, memoria6);
	idMemoria++;

	infoMemoria* memoria7 = malloc(sizeof(infoMemoria));
	//	memoria3->ccia = EC;
	memoria7->id = idMemoria;
	list_add(listaMemorias, memoria7);
	idMemoria++;

}

void hardcodearListaMetadataTabla(){
	metadataTablas* metadata1 = malloc(sizeof(metadataTablas));
	metadata1->consistencia = SC;
	metadata1->nParticiones = 2;
	metadata1->nombreTabla = "TABLA1";
	list_add(listaMetadataTabla, metadata1);

	metadataTablas* metadata2 = malloc(sizeof(metadataTablas));
	metadata2->consistencia = SHC;
	metadata2->nParticiones = 3;
	metadata2->nombreTabla = "TABLA2";
	list_add(listaMetadataTabla, metadata2);

	metadataTablas* metadata3 = malloc(sizeof(metadataTablas));
	metadata3->consistencia = EC;
	metadata3->nParticiones = 4;
	metadata3->nombreTabla = "TABLA3";
	list_add(listaMetadataTabla, metadata3);
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
	srand(time(NULL));
	int numeroAleatorio = rand() % list_size(listaMemorias);
	return list_get(listaMemorias, numeroAleatorio);
}

void comandoDescribe(char*nombreTabla){
	infoMemoria* memoriaAlAzar = NULL;

	memoriaAlAzar = obtenerMemoriaAlAzar();
	log_trace(log_master->logTrace, "El id de la memoria obtenida es: %d", memoriaAlAzar->id);

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
		return cons == memoria->ccia;
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
	srand(time(NULL));
	int randomNumber = rand() % list_size(memoriasEncontradas);

	return list_get(memoriasEncontradas, randomNumber);
}

//infoMemoria* newInfoMemoria(){
//	infoMemoria* memoria = malloc(sizeof(infoMemoria));
//	memoria->id = idMemoria;
//	idMemoria++;
//	for(int i=0; i < 3; i++){
//
//	}
//}

consistencia mejorCriterioMemoria(bool* criterios){
	if(criterios[0])
		return SC;
	else if(criterios[1])
		return SHC;
	else if(criterios[2])
		return EC;

	return ERROR_CONSISTENCIA;
}

//void asignarCriterioMemoria(infoMemoria* memoria, consistencia cons){
//	if(!haySC && cons == SC)
//
//}
