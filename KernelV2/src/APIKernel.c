/*
 * APIKernel.c
 *
 *  Created on: 20 jun. 2019
 *      Author: utnso
 */

#include "APIKernel.h"

int procesarInputKernel(char* linea) {
	char** comandos = string_n_split(linea, 2, " ");
	char* operacion = comandos[0];
	char* argumentos = comandos[1];
	if (strcmp(operacion, "INSERT") == 0) {
		//	INSERT [NOMBRE_TABLA] [KEY] “[VALUE]”
		log_trace(log_master->logTrace, "Se ha escrito el comando INSERT");
		if (consolaInsert(argumentos) == SUPER_ERROR)
			return SUPER_ERROR;

	} else if (strcmp(operacion, "SELECT") == 0) {
		//	SELECT [NOMBRE_TABLA] [KEY]
		log_trace(log_master->logTrace, "Se ha escrito el comando SELECT");
		if (consolaSelect(argumentos) == SUPER_ERROR)
			return SUPER_ERROR;

	} else if (strcmp(operacion, "CREATE") == 0) {
		//	CREATE [TABLA] [TIPO_CONSISTENCIA] [NUMERO_PARTICIONES] [COMPACTION_TIME]
		log_trace(log_master->logTrace, "Se ha escrito el comando CREATE");
		if (consolaCreate(argumentos) == SUPER_ERROR)
			return SUPER_ERROR;

	} else if (strcmp(operacion, "DESCRIBE") == 0) {
		// DESCRIBE [NOMBRE_TABLA]
		// DESCRIBE
		log_trace(log_master->logTrace, "Se ha escrito el comando DESCRIBE");
		if (consolaDescribe(argumentos) == SUPER_ERROR)
			return SUPER_ERROR;

	} else if (strcmp(operacion, "DROP") == 0) {
		//	DROP [NOMBRE_TABLA]
		log_trace(log_master->logTrace, "Se ha escrito el comando DROP");
		if (consolaDrop(argumentos) == SUPER_ERROR)
			return SUPER_ERROR;

	} else if (strcmp(operacion, "ADD") == 0) {
		//	ADD MEMORY [id] TO [consistencia]
		log_trace(log_master->logTrace, "Se ha escrito el comando ADD");
		if (consolaAdd(argumentos) == SUPER_ERROR)
			return SUPER_ERROR;
	} else if (strcmp(operacion, "RUN") == 0) {
		//	ADD MEMORY [id] TO [consistencia]
		log_trace(log_master->logTrace, "Se ha escrito el comando RUN");
		if (consolaRun(argumentos) == SUPER_ERROR)
			return SUPER_ERROR;
	} else if (strcmp(operacion, "JOURNAL") == 0) {
		log_trace(log_master->logTrace, "Se ha escrito el comando JOURNAL");
		if (consolaJournal() == SUPER_ERROR)
			return SUPER_ERROR;
	} else if (strcmp(operacion, "SALIR") == 0) {
		consolaSalir(argumentos);

	} else {
		log_error(log_master->logError, "El comando no es el correcto. Por favor intente nuevamente");
	}

	free(argumentos);
	free(operacion);
	free(comandos);

	return TODO_OK;
}

int procesarAdd(int id, consistencia cons) {
	bool condicionAdd(int id, infoMemoria* memoria) {
		return id == memoria->id;
	}
	bool _esCondicionAdd(void* memoria) {
		return condicionAdd(id, memoria);
	}
	infoMemoria* memoriaEncontrada = NULL;
	if ((memoriaEncontrada = list_find(listaMemorias, (void*) _esCondicionAdd)) != NULL) {
		asignarCriterioMemoria(memoriaEncontrada, cons);

		if (!(memoriaEncontrada->criterios)[3])
			log_trace(log_master->logTrace, "Se ha asignado el criterio a la memoria correctammente");

		imprimirCriterio(memoriaEncontrada->criterios);
		log_trace(log_master->logTrace, "El id de esta memoria es: %d", memoriaEncontrada->id);
	} else {
		log_error(log_master->logError, "Problemas con el comando ADD: No se pudo encontrar la memoria");
		log_error(log_master->logError, "Posible listaMemorias vacia");
		return SUPER_ERROR;
	}
	return TODO_OK;
}

int consolaAdd(char*argumento) {
	char** valores = string_split(argumento, " ");
	consistencia cons = procesarConsistencia(valores[3]);

	int id = atoi(valores[1]);
	if (procesarAdd(id, cons) == SUPER_ERROR)
		return SUPER_ERROR;

	freePunteroAPunteros(valores);
//	free(argumentoAux);

	return TODO_OK;
}

int consolaInsert(char*argumentos) {
	//	INSERT
	//[NOMBRE_TABLA] [KEY] “[VALUE]” TIMESTAMP

	// Info inicial de las metricas
	metricas.writes++;
	timestampInsertAlIniciar = getCurrentTime();

	// Ahora evaluamos el comando en si
	char** valores = string_split(argumentos, "\"");
	char** valoresAux = string_split(valores[0], " ");
	char* nombreTabla = string_duplicate(valoresAux[0]);
	char* key = valoresAux[1];
	char* value = valores[1];
	double timeStamp = 0;
	if (valores[2] == NULL) {
		timeStamp = getCurrentTime();
	} else {
		timeStamp = atof(valores[2]);
	}

	log_trace(log_master->logTrace, "El nombre de la tabla es: %s, su key es %s, , su value es: %s y su timeStamp: %f", nombreTabla, key,
			value, timeStamp);

	infoMemoria* memoriaAEnviar = obtenerMemoria(nombreTabla, atoi(key));
	if (memoriaAEnviar == NULL) {
		log_error(log_master->logError, "Error: No se pudo obtener la memoria deseada");
		freePunteroAPunteros(valoresAux);
		freePunteroAPunteros(valores);
		return SUPER_ERROR;
	}
	obtenerMemoriaSegunTablaYKey(atoi(key), nombreTabla, INSERT, memoriaAEnviar);

	// Seguimos con las metrics
	timestampInsertAlFinalizar = getCurrentTime();
	double* diferencia = malloc(sizeof(double));
	*diferencia = timestampInsertAlFinalizar - timestampInsertAlIniciar;
	list_add(metricas.diferenciaDeTiempoWriteLatency, diferencia);
	cantInserts++;

	int socketMemoria = ConectarAServidor(memoriaAEnviar->puerto, memoriaAEnviar->ip);

	Paquete paquete;
	if(socketMemoria >=0)
		if (enviarInfoMemoria(socketMemoria, argumentos, INSERT, &paquete) == SUPER_ERROR)
			return SUPER_ERROR;

	freePunteroAPunteros(valoresAux);
	freePunteroAPunteros(valores);

	return TODO_OK;
}

int consolaSelect(char*argumentos) {
	// Info inicial de las metricas
	metricas.reads++;
	timestampSelectAlIniciar = getCurrentTime();

	// Ahora evaluamos el comando en si
	char* argumentosAux = string_duplicate(argumentos);
	char** valores = string_split(argumentosAux, " ");
	char* nombreTabla = valores[0];
	int key = atoi(valores[1]);

	log_trace(log_master->logTrace, "El nombre de la tabla es: %s, y la key es: %d", nombreTabla, key);

	infoMemoria* memoriaAEnviar = obtenerMemoria(nombreTabla, key);

	if (obtenerMemoriaSegunTablaYKey(key, nombreTabla, SELECT, memoriaAEnviar) == SUPER_ERROR) {
		freePunteroAPunteros(valores);
		free(argumentosAux);
		return SUPER_ERROR;
	}

	// Seguimos con las metrics y luego mandamos el mensaje
	timestampSelectAlFinalizar = getCurrentTime();
	double* diferencia = malloc(sizeof(double));
	*diferencia = timestampSelectAlFinalizar - timestampSelectAlIniciar;
	list_add(metricas.diferenciaDeTiempoReadLatency, diferencia);
	cantSelects++;

	// Ahora mandamos el mensaje
	int socketMemoria = ConectarAServidor(memoriaAEnviar->puerto, memoriaAEnviar->ip);
	// El mennsaje de error se imprime por pantalla en esta funcion

	if (socketMemoria >= 0) {
		Paquete paquete;
		char* request = string_new();
		string_append_with_format(&request, "%s %d", nombreTabla, key);
//		EnviarDatosTipo(socketMemoria, KERNEL, request, strlen(request) + 1, SELECT);
//		free(request);
//
//		RecibirPaqueteCliente(socketMemoria, MEMORIA, &paquete);
//
//		if (atoi(paquete.mensaje) == 1) {
//			log_info(log_master->logInfo, "El registro no se encuentra");
//		} else {
//			log_info(log_master->logInfo, "Registro de tabla %s: %s", nombreTabla, paquete.mensaje);
//		}

		if (enviarInfoMemoria(socketMemoria, request, INSERT, &paquete) == SUPER_ERROR)
			return SUPER_ERROR;
		else{
			log_info(log_master->logInfo, "Registro de tabla %s: %s", nombreTabla, paquete.mensaje);
		}

		free(paquete.mensaje);
		freePunteroAPunteros(valores);
		free(argumentosAux);
	} else
		return SUPER_ERROR;

	return TODO_OK;
}

int enviarInfoMemoria(int socketMemoria, char* request, t_protocolo protocolo, Paquete* paquete) {
//	Paquete paquete;
	int success;

	if (EnviarDatosTipo(socketMemoria, KERNEL, request, strlen(request) + 1, protocolo)) {
		log_trace(log_master->logTrace, "El paquete se envio exitosamente");
	} else {
		log_error(log_master->logError, "Error al enviar paquete");
		return SUPER_ERROR;
	}
	RecibirPaqueteCliente(socketMemoria, MEMORIA, paquete);
	success =atoi(paquete->mensaje);
	printf("Fue un exito? 0 = si, 1 = no: %d\n",success); // Creo que esto lo puedo sacar porque los logs ya hacen el trabajo
	if (success == 0) {
		log_trace(log_master->logTrace, "Paquete recibido correctamente");
	} else {
		log_error(log_master->logError, "Error al recibir el paquete");
		success = SUPER_ERROR;
	}

	free(paquete->mensaje);
	return success;
}

int enviarCREATE(int cantParticiones, int tiempoCompactacion, char* nombreTabla, char* consistenciaChar, infoMemoria* memoria) {

	int socketMemoria = ConectarAServidor(memoria->puerto, memoria->ip);
	char request[100];
	sprintf(request, "%s %s %d %d", nombreTabla, consistenciaChar, cantParticiones, tiempoCompactacion);
	Paquete paquete;
	if (enviarInfoMemoria(socketMemoria, request, CREATE, &paquete) == SUPER_ERROR)
		return SUPER_ERROR;
	return TODO_OK;
}

void enviarJournalMemoria(int socketMemoria) {
	log_trace(log_master->logTrace, "Comando JOURNAL enviado a Memoria");
	EnviarDatosTipo(socketMemoria, KERNEL, NULL, 0, JOURNAL);
}

int consolaJournal() {

	//TODO: Cambiarlo para que sean solo las memorias que tienen criterios
	// En listaMemorias se guardan todas las memorias conocidas, pero no todas tienen criterio porque eso se lo asigna el usuario en tiempo de ejecucion

	void journalMemoria(infoMemoria* memoria) {
		log_info(log_master->logInfo, "Enviando comando JOURNAL a memoria %d", memoria->id);
		int socketMemoria = ConectarAServidor(memoria->puerto, memoria->ip);
		EnviarDatosTipo(socketMemoria, KERNEL, NULL, 0, JOURNAL);
	}
	list_iterate(listaMemorias, (void*) journalMemoria);
	return TODO_OK;
}

int consolaCreate(char*argumentos) {
	char* argumentoAux = string_duplicate(argumentos);
	char** valores = string_split(argumentoAux, " ");
	char* nombreTabla = valores[0];
	char* consistenciaChar = valores[1];
	int cantParticiones = atoi(valores[2]);
	int tiempoCompactacion = atoi(valores[3]);

	log_trace(log_master->logTrace,
			"El nombre de la tabla es: %s, la consistencia es: %s, la cantParticiones:%d, y el tiempoCompactacion es: %d", nombreTabla,
			consistenciaChar, cantParticiones, tiempoCompactacion);

	infoMemoria* memoriaAlAzar = obtenerMemoriaAlAzarParaFunciones();

//	if (enviarCREATE(cantParticiones, tiempoCompactacion, nombreTabla, consistenciaChar, memoriaAlAzar) == SUPER_ERROR) {
//		return SUPER_ERROR;
//	} else {
//		metadataTabla* metaData = deserealizarMetadata(argumentos);
//		agregarTabla(metaData);
//	}

	freePunteroAPunteros(valores);
	free(argumentoAux);

	return TODO_OK;

}

metadataTabla* deserealizarMetadata(char* metadataSerializada) {
	char* mensaje = string_duplicate(metadataSerializada);
	char** datos = string_split(mensaje, " ");
	char* nombreTabla = string_duplicate(datos[0]);

	consistencia consistencia = getConsitenciaFromChar(datos[1]);
	int cantParticiones = atoi(datos[2]);
	int tiempoCompactacion = atoi(datos[3]);
	metadataTabla* metadata = newMetadata(nombreTabla, consistencia, cantParticiones, tiempoCompactacion);

	//mostrarMetadata(metadata);

	freePunteroAPunteros(datos);
	free(nombreTabla);
	free(mensaje);
	return metadata;

}

consistencia getConsitenciaFromChar(char* consistenciaChar) {
	if (strcmp(consistenciaChar, "SC") == 0) {
		return SC;
	} else if (strcmp(consistenciaChar, "SHC") == 0) {
		return SHC;
	} else {
		return EC;
	}
}

metadataTabla* newMetadata(char* nombreTabla, consistencia consistencia, int cantParticiones, int tiempoCompactacion) {

	metadataTabla* tabla = malloc(sizeof(metadataTabla));
	tabla->consistencia = consistencia;
	tabla->nParticiones = cantParticiones;
	tabla->nombreTabla = string_duplicate(nombreTabla);

	return tabla;
}

int procesarDescribeAll(int socketMemoria) {
	int estadoRecibir = 0;
	log_trace(log_master->logTrace, "Se pide la metadata de todos las tablas");
//	EnviarDatosTipo(socketMemoria, KERNEL, NULL, 0, DESCRIBE_ALL);
//
//	Paquete paquete;
//	while ((estadoRecibir = RecibirPaqueteCliente(socketMemoria, MEMORIA, &paquete)) > 0) {
//		metadataTabla* metadata = deserealizarMetadata(paquete.mensaje);
//		agregarTabla(metadata);
//		free(paquete.mensaje);
//	}

	if (estadoRecibir < 0)
		return SUPER_ERROR;
	return TODO_OK;
}

int procesarDescribe(int socketMemoria, char* nombreTabla) {
	log_trace(log_master->logTrace, "Se pide la metadata de %s", nombreTabla);
//	EnviarDatosTipo(socketMemoria, KERNEL, nombreTabla, strlen(nombreTabla) + 1, DESCRIBE);
//
//	Paquete paquete;
//	if (RecibirPaqueteCliente(socketMemoria, MEMORIA, &paquete) < 0) {
//		return SUPER_ERROR;
//	} else if (atoi(paquete.mensaje) == 1) {
//		puts("La tabla no existe");
//
//	} else {
//		// TODO: Agregar verificacion aca
//		metadataTabla* metadataRecibida = deserealizarMetadata(paquete.mensaje);
//		agregarTabla(metadataRecibida);
//	}
//	free(paquete.mensaje);
	return TODO_OK;
}

int consolaDescribe(char*nombreTabla) {
	infoMemoria* memoriaAlAzar = obtenerMemoriaAlAzarParaFunciones();
	int socketMemoria = ConectarAServidor(memoriaAlAzar->puerto, memoriaAlAzar->ip);
	if (nombreTabla == NULL) {
		if (procesarDescribeAll(socketMemoria) == SUPER_ERROR)
			return SUPER_ERROR;
	} else {
		if (procesarDescribe(socketMemoria, nombreTabla) == SUPER_ERROR)
			return SUPER_ERROR;
	}

	return TODO_OK;
}

void agregarTabla(metadataTabla* tabla) {
	bool findByNombre(metadataTabla* tablaActual) {
		return strcmp(tabla->nombreTabla, tablaActual->nombreTabla) == 0;
	}

	if (!list_any_satisfy(listaMetadataTabla, (void*) findByNombre)) {
		list_add(listaMetadataTabla, tabla);
	}

}

void mostrarMetadata(metadataTabla* metadataTabla) {

	log_info(log_master->logInfo, "Segmento: %s", metadataTabla->nombreTabla);
	log_info(log_master->logInfo, "Consistencia: %s / cantParticiones: %d ", consistenciaToChar(metadataTabla->consistencia),
			metadataTabla->nParticiones);
}

char* consistenciaToChar(consistencia consistencia) {
	if (consistencia == SC) {
		return "SC";
	} else if (consistencia == SHC) {
		return "SHC";
	} else {
		return "EC";
	}
}

int consolaDrop(char*nombreTabla) {
	if (nombreTabla != NULL)
		log_trace(log_master->logTrace, "Se desea elminar la tabla %s", nombreTabla);
	else {
		log_error(log_master->logError, "Error: ingresar el nombre de la tabla a eliminar");
		return SUPER_ERROR;
	}

	infoMemoria* memoriaAlAzar = obtenerMemoriaAlAzarParaFunciones();

//	int socketMemoria = ConectarAServidor(memoriaAlAzar->puerto, memoriaAlAzar->ip);
//	char request[100];
//	sprintf(request, "%s", nombreTabla);
//	if (enviarInfoMemoria(socketMemoria, request, DROP) == SUPER_ERROR) {
//		return SUPER_ERROR;
//	} else {
//
//		bool findByNombre(metadataTabla* tablaActual) {
//			return strcmp(nombreTabla, tablaActual->nombreTabla) == 0;
//		}
//		list_remove_and_destroy_by_condition(listaMetadataTabla, (void*) findByNombre, (void*) freeMetadata);
//	}

	return TODO_OK;
}

void freeMetadata(metadataTabla* tabla) {
	if (tabla != NULL) {
		free(tabla->nombreTabla);
		free(tabla);
	}

}

int consolaRun(char*path) {
	FILE* fd = NULL;
	int num_lineas = contarLineasArchivo(fd, path);
	if (num_lineas != 0) {
		fd = fopen(path, "rt");
		procExec* proceso = newProceso();

		for (int i = 0; i < num_lineas; i++) {
			char ejemplo[500];
			fgets(ejemplo, sizeof(ejemplo), fd);
			size_t tam = strlen(ejemplo) + 1;

			if (ejemplo[tam - 2] == '\n')
				ejemplo[tam - 2] = '\0';
			else
				ejemplo[tam - 1] = '\0';

			// TODO: Aca creo que hag un memory leak -> Arreglar
			char* aGuardar = (char*) malloc(tam - 1);
			strcpy(aGuardar, ejemplo);
			agregarRequestAlProceso(proceso, aGuardar);
		}

		fclose(fd);
		deNewAReady(proceso);

	} else {
		log_error(log_master->logError, "El path no es correcto o el archivo esta dañado");
		return SUPER_ERROR;
	}
	return TODO_OK;
}

void consolaSalir(char*nada) {
	log_trace(log_master->logTrace, "Finalizando consola");

	puedeHaberRequests = false;
	for (int i = 0; i < multiprocesamiento; i++)
		sem_post(&cantProcesosColaReady);
	usleep(5);
	sem_post(&fin);
}

