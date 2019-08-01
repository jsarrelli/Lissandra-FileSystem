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
		if (consolaInsert(argumentos) == SUPER_ERROR) {
			freePunteroAPunteros(comandos);
			return SUPER_ERROR;
		}

	} else if (strcmp(operacion, "SELECT") == 0) {
		//	SELECT [NOMBRE_TABLA] [KEY]
		log_trace(log_master->logTrace, "Se ha escrito el comando SELECT");
		if (consolaSelect(argumentos) == SUPER_ERROR) {
			freePunteroAPunteros(comandos);
			return SUPER_ERROR;
		}
	} else if (strcmp(operacion, "CREATE") == 0) {
		//	CREATE [TABLA] [TIPO_CONSISTENCIA] [NUMERO_PARTICIONES] [COMPACTION_TIME]
		log_trace(log_master->logTrace, "Se ha escrito el comando CREATE");
		if (consolaCreate(argumentos) == SUPER_ERROR) {
			freePunteroAPunteros(comandos);
			return SUPER_ERROR;
		}
	} else if (strcmp(operacion, "DESCRIBE") == 0) {
		// DESCRIBE [NOMBRE_TABLA]
		// DESCRIBE
		log_trace(log_master->logTrace, "Se ha escrito el comando DESCRIBE");
		if (consolaDescribe(argumentos) == SUPER_ERROR) {
			freePunteroAPunteros(comandos);
			return SUPER_ERROR;
		}
	} else if (strcmp(operacion, "DROP") == 0) {
		//	DROP [NOMBRE_TABLA]
		log_trace(log_master->logTrace, "Se ha escrito el comando DROP");
		if (consolaDrop(argumentos) == SUPER_ERROR) {
			freePunteroAPunteros(comandos);
			return SUPER_ERROR;
		}
	} else if (strcmp(operacion, "ADD") == 0) {
		//	ADD MEMORY [id] TO [consistencia]
		log_trace(log_master->logTrace, "Se ha escrito el comando ADD");
		if (consolaAdd(argumentos) == SUPER_ERROR) {
			freePunteroAPunteros(comandos);
			return SUPER_ERROR;
		}
	} else if (strcmp(operacion, "RUN") == 0) {
		//	ADD MEMORY [id] TO [consistencia]
		log_trace(log_master->logTrace, "Se ha escrito el comando RUN");
		if (consolaRun(argumentos) == SUPER_ERROR) {
			freePunteroAPunteros(comandos);
			return SUPER_ERROR;
		}
	} else if (strcmp(operacion, "JOURNAL") == 0) {
		log_trace(log_master->logTrace, "Se ha escrito el comando JOURNAL");
		if (consolaJournal() == SUPER_ERROR) {
			freePunteroAPunteros(comandos);
			return SUPER_ERROR;
		}
	} else {
		log_error(log_master->logError, "El comando no es el correcto. Por favor intente nuevamente");
	}

	freePunteroAPunteros(comandos);

	return TODO_OK;
}

int procesarAdd(int id, t_consistencia consistencia) {
	int success = TODO_OK;

	log_info(log_master->logInfo, "Asignando criterio %s a memoria: %d", getConsistenciaCharByEnum(consistencia), id);
	bool byId(infoMemoria* memoria) {
		return memoria->id == id;
	}
	infoMemoria* memoriaEncontrada = list_find(listaMemorias, (void*) byId);
	if (memoriaEncontrada != NULL) {
		success = asignarCriterioMemoria(memoriaEncontrada, consistencia);

		if (success == TODO_OK) {
			log_trace(log_master->logTrace, "Criterio asignado exitosamente");
			imprimirCriterio(memoriaEncontrada);
		}

	} else {
		log_error(log_master->logError, "Problemas con el comando ADD: No se pudo encontrar la memoria");
		return SUPER_ERROR;
	}
	return success;
}

int consolaAdd(char*argumento) {
	char** valores = string_split(argumento, " ");
	t_consistencia cons = getConsistenciaByChar(valores[3]);

	int id = atoi(valores[1]);
	if (procesarAdd(id, cons) == SUPER_ERROR) {
		freePunteroAPunteros(valores);
		return SUPER_ERROR;
	}

	freePunteroAPunteros(valores);

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
	char* nombreTabla = valoresAux[0];
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
		freePunteroAPunteros(valoresAux);
		freePunteroAPunteros(valores);
		return SUPER_ERROR;
	}
	actualizarMetricasDeMemoria(atoi(key), nombreTabla, INSERT, memoriaAEnviar);

	// Seguimos con las metrics
	timestampInsertAlFinalizar = getCurrentTime();
	double* diferencia = malloc(sizeof(double));
	*diferencia = timestampInsertAlFinalizar - timestampInsertAlIniciar;
	list_add(metricas.diferenciaDeTiempoWriteLatency, diferencia);
	cantInserts++;
	log_info(log_master->logInfo, "Intenando conectarse a memoria..");
	int socketMemoria = ConectarAServidorPlus(memoriaAEnviar->puerto, memoriaAEnviar->ip);

	Paquete paquete;
	if (socketMemoria >= 0) {
		log_info(log_master->logInfo, "Conexion existosa");
		if (enviarInfoMemoria(socketMemoria, argumentos, INSERT, &paquete) == SUPER_ERROR) {
			log_info(log_master->logInfo, "Por algun motivo lo recibe mal. Pero anda piola");
			//return SUPER_ERROR;
		}

	}

	freePunteroAPunteros(valoresAux);
	freePunteroAPunteros(valores);

	return TODO_OK;
}

int consolaSelect(char*argumentos) {
	//	SELECT [NOMBRE_TABLA] [KEY]
	// Info inicial de las metricas
	metricas.reads++;
	timestampSelectAlIniciar = getCurrentTime();

	// Ahora evaluamos el comando en si
	char* argumentosAux = string_duplicate(argumentos);
	char** valores = string_split(argumentosAux, " ");
	char* nombreTabla = valores[0];
	int key = atoi(valores[1]);

	log_info(log_master->logInfo, "Realizando SELECT %s %d", nombreTabla, key);

	infoMemoria* memoriaAEnviar = obtenerMemoria(nombreTabla, key);
	if (memoriaAEnviar == NULL) {
		return SUPER_ERROR;
		freePunteroAPunteros(valores);
		free(argumentosAux);
	}

	// Seguimos con las metrics y luego mandamos el mensaje
	timestampSelectAlFinalizar = getCurrentTime();
	double* diferencia = malloc(sizeof(double));
	*diferencia = timestampSelectAlFinalizar - timestampSelectAlIniciar;
	list_add(metricas.diferenciaDeTiempoReadLatency, diferencia);
	cantSelects++;

	actualizarMetricasDeMemoria(key, nombreTabla, SELECT, memoriaAEnviar);

	log_info(log_master->logInfo, "Intenando conectarse a memoria..");
	int socketMemoria = ConectarAServidorPlus(memoriaAEnviar->puerto, memoriaAEnviar->ip);
	int success = TODO_OK;
	if (socketMemoria >= 0) {
		log_info(log_master->logInfo, "Conexion exitosa", nombreTabla, key);

		Paquete paquete;
		char* request = string_new();
		string_append_with_format(&request, "%s %d", nombreTabla, key);

		log_info(log_master->logInfo, "Enviando datos..", nombreTabla, key);
		EnviarDatosTipo(socketMemoria, KERNEL, request, strlen(request) + 1, SELECT);
		free(request);
		log_info(log_master->logInfo, "Recibiendo datos.. ", nombreTabla, key);
		if (RecibirPaquete(socketMemoria, &paquete) <= 0) {

			log_info(log_master->logInfo, "Fijate que algo fallo en ese SELECT. Pero anda piola");
			//success = SUPER_ERROR;
		} else {
			log_info(log_master->logInfo, "Datos recibidos ", nombreTabla, key);
			if (atoi(paquete.mensaje) == -1) {
				log_trace(log_master->logTrace, "Registros no encontrado");
			} else {
				log_trace(log_master->logTrace, "Registro de tabla %s: %s", nombreTabla, paquete.mensaje);
			}
			free(paquete.mensaje);
		}

	} else {
		log_error(log_master->logError, "Hubo un error en la recepcion del mensaje");
		success = SUPER_ERROR;
	}

	freePunteroAPunteros(valores);
	free(argumentosAux);
	close(socketMemoria);
	return success;
}

int enviarInfoMemoria(int socketMemoria, char* request, t_protocolo protocolo, Paquete* paquete) {

	int success;
	log_info(log_master->logInfo, "Enviando datos..");
	if (EnviarDatosTipo(socketMemoria, KERNEL, request, strlen(request) + 1, protocolo)) {
		log_info(log_master->logInfo, "El paquete se envio exitosamente");
	} else {
		log_error(log_master->logError, "Error al enviar paquete");
		return SUPER_ERROR;
	}
	log_info(log_master->logInfo, "Esperando para recibir datos..");
	if (RecibirPaquete(socketMemoria, paquete) <= 0) {
		log_info(log_master->logInfo, "Error en la recepcion del paquete..");
		success = 1;
	} else {
		log_info(log_master->logInfo, "Se recibio el paquete con exito..");
		success = atoi(paquete->mensaje);
		free(paquete->mensaje);
	}
	if (socketMemoria != -1) {
		close(socketMemoria);
	}

	return success;
}

int enviarCREATE(int cantParticiones, int tiempoCompactacion, char* nombreTabla, char* consistenciaChar, infoMemoria* memoria) {
	log_info(log_master->logInfo, "Intenando conectarse a memoria %d..", memoria->id);
	int socketMemoria = ConectarAServidorPlus(memoria->puerto, memoria->ip);

	if (socketMemoria > 0) {
		log_info(log_master->logInfo, "Conexion existosa");
		char request[100];
		sprintf(request, "%s %s %d %d", nombreTabla, consistenciaChar, cantParticiones, tiempoCompactacion);
		Paquete paquete;
		if (enviarInfoMemoria(socketMemoria, request, CREATE, &paquete) == SUPER_ERROR) {
			log_info(log_master->logInfo, "Error al enviar la info a memoria CREATE");
			return SUPER_ERROR;
		}

	} else {
		return SUPER_ERROR;
	}

	return TODO_OK;
}

int consolaJournal() {
	void journalMemoria(infoMemoria* memoria) {

		log_info(log_master->logInfo, "Enviando comando JOURNAL a memoria %d", memoria->id);
		log_info(log_master->logInfo, "Intenando conectarse a memoria..");
		int socketMemoria = ConectarAServidorPlus(memoria->puerto, memoria->ip);
		if (socketMemoria < 0) {
			log_error(log_master->logError, "No se pudo mandar el mensaje DE JOURNAL");
		} else {
			log_info(log_master->logInfo, "Conexion existosa");
			EnviarDatosTipo(socketMemoria, KERNEL, NULL, 0, JOURNAL);
			log_info(log_master->logInfo, "JOURNAL ENVIADO A MEMORIA..");
		}

	}

	bool _tieneCriterio(infoMemoria* memoria) {
		return !list_is_empty(memoria->criterios);
	}

	t_list* listaMemoriasConCriterio = list_filter(listaMemorias, (void*) _tieneCriterio);
	if (listaMemoriasConCriterio != NULL)
		list_iterate(listaMemoriasConCriterio, (void*) journalMemoria);
	else {
		log_error(log_master->logError, "Error al realizar el journal: No hay memorias registradas en el Kernel o no tiene criterio aun");
		log_error(log_master->logError, "Por favor, ingresar alguno");
	}
	return TODO_OK;
}

int consolaCreate(char*argumentos) {
	//	CREATE [TABLA] [TIPO_CONSISTENCIA] [NUMERO_PARTICIONES] [COMPACTION_TIME]
	char* argumentoAux = string_duplicate(argumentos);
	char** valores = string_split(argumentoAux, " ");
	char* nombreTabla = valores[0];
	char* consistenciaChar = valores[1];
	int cantParticiones = atoi(valores[2]);
	int tiempoCompactacion = atoi(valores[3]);

	log_info(log_master->logInfo,
			"El nombre de la tabla es: %s, la consistencia es: %s, la cantParticiones:%d, y el tiempoCompactacion es: %d", nombreTabla,
			consistenciaChar, cantParticiones, tiempoCompactacion);

	infoMemoria* memoriaAlAzar = obtenerMemoriaAlAzar(listaMemorias);
	int success = TODO_OK;

	if (enviarCREATE(cantParticiones, tiempoCompactacion, nombreTabla, consistenciaChar, memoriaAlAzar) == SUPER_ERROR) {
		log_error(log_master->logError, "ERROR AL ENVIAR EL CREATE");
		success = SUPER_ERROR;
	} else {

		infoTabla* infoTabla = newInfoTabla(nombreTabla, getConsistenciaByChar(valores[1]));
		agregarTabla(infoTabla);
	}

	freePunteroAPunteros(valores);
	free(argumentoAux);

	return success;

}

infoTabla* newInfoTabla(char* nombreTabla, t_consistencia consistencia) {

	infoTabla* tabla = malloc(sizeof(infoTabla));
	tabla->nombreTabla = string_duplicate(nombreTabla);
	tabla->consitencia = consistencia;

	return tabla;
}

infoTabla* deserealizarInfoTabla(char* tablaSerializada) {
	char** valores = string_split(tablaSerializada, " ");

	log_trace(log_master->logTrace, "Tabla:%s / Consistencia:%s / Cant. Particiones:%s / Tiempo Compactacion:%s", valores[0], valores[1],
			valores[2], valores[3]);

	infoTabla* infoTabla = newInfoTabla(valores[0], getConsistenciaByChar(valores[1]));
	freePunteroAPunteros(valores);
	return infoTabla;
}
int procesarDescribeAll(int socketMemoria) {
	log_info(log_master->logInfo, "Se pide la metadata de todos las tablas");
	EnviarDatosTipo(socketMemoria, KERNEL, NULL, 0, DESCRIBE_ALL);

	Paquete paquete;

	int succes = TODO_OK;
	log_info(log_master->logInfo, "Esperando recepcion de paquete");
	if (RecibirPaquete(socketMemoria, &paquete) > 0) {
		if (atoi(paquete.mensaje) != 1) {
			log_info(log_master->logInfo, "Paquete recibido");
			char* response = string_duplicate(paquete.mensaje);
			char** tablasSerializadas = string_split(response, "/");
			int i = 0;
			while (tablasSerializadas[i] != NULL) {
				char* tablaSerializada = string_duplicate(tablasSerializadas[i]);

				infoTabla* infoTabla = deserealizarInfoTabla(tablaSerializada);
				agregarTabla(infoTabla);

				free(tablaSerializada);
				i++;
			}
			freePunteroAPunteros(tablasSerializadas);
			free(response);
		} else {
			log_info(log_master->logInfo, "No hay tablas en el sistema");
		}
		free(paquete.mensaje);
	} else {
		log_error(log_master->logError, "ERROR PROCESAR DESCRIBE ALL..");
	}

	log_info(log_master->logInfo, "Tam de listaMetadataTabla: %d", list_size(listaInfoTablas));
	return succes;
}

int procesarDescribe(int socketMemoria, char* nombreTabla) {
	log_info(log_master->logInfo, "Se pide la metadata de %s", nombreTabla);
	EnviarDatosTipo(socketMemoria, KERNEL, nombreTabla, strlen(nombreTabla) + 1, DESCRIBE);

	Paquete paquete;
	log_info(log_master->logInfo, "Esperando para recibir paquete procesar describe..");
	if (RecibirPaqueteCliente(socketMemoria, MEMORIA, &paquete) <= 0) {
		log_error(log_master->logError, "Error recibir paquete procesar describe");
		return SUPER_ERROR;
	} else if (atoi(paquete.mensaje) == -1) {
		log_error(log_master->logError, "La tabla %S no existe", nombreTabla);

	} else {
		log_info(log_master->logInfo, "Deserializando metadata procesar describe...");
		infoTabla* infoTabla = deserealizarInfoTabla(paquete.mensaje);
		agregarTabla(infoTabla);
	}
	free(paquete.mensaje);
	return TODO_OK;
}

int consolaDescribe(char*nombreTabla) {
	// DESCRIBE [NOMBRE_TABLA]
	// DESCRIBE
	int socketMemoria;
	while (true) {

		infoMemoria* memoriaAlAzar = obtenerMemoriaAlAzar(listaMemorias);
		log_info(log_master->logInfo, "Intentando conectarse a memoria: %d/%s/%d..", memoriaAlAzar->id, memoriaAlAzar->ip,
				memoriaAlAzar->puerto);
		socketMemoria = ConectarAServidor(memoriaAlAzar->puerto, memoriaAlAzar->ip);
		log_info(log_master->logInfo, "Socket: %d", socketMemoria);
		if (socketMemoria != -1) {
			break;
		}
		log_error(log_master->logError, "Fallo la conexion con la memoria:%d , reitentando..", memoriaAlAzar->id);
		usleep(200);

	}

	int success = TODO_OK;
	log_info(log_master->logInfo, "Conexion existosa");
	if (nombreTabla == NULL) {
		success = procesarDescribeAll(socketMemoria);
	} else {
		success = procesarDescribe(socketMemoria, nombreTabla);

	}

	close(socketMemoria);
	return success;
}

void agregarTabla(infoTabla* tabla) {
	bool findByNombre(infoTabla* tablaActual) {
		return strcmp(tabla->nombreTabla, tablaActual->nombreTabla) == 0;
	}

	if (!list_any_satisfy(listaInfoTablas, (void*) findByNombre)) {
		list_add(listaInfoTablas, tabla);
		log_info(log_master->logInfo, "Tabla %s agregada al sistema", tabla->nombreTabla);
	} else {
		free(tabla->nombreTabla);
		free(tabla);
	}

}

int consolaDrop(char*nombreTabla) {
	if (nombreTabla != NULL)
		log_trace(log_master->logTrace, "Se desea elminar la tabla %s", nombreTabla);
	else {
		log_error(log_master->logError, "Error: ingresar el nombre de la tabla a eliminar");
		return SUPER_ERROR;
	}

	infoMemoria* memoriaAlAzar = obtenerMemoriaAlAzar(listaMemorias);
	log_info(log_master->logInfo, "Intenando conectarse a memoria..");
	int socketMemoria = ConectarAServidorPlus(memoriaAlAzar->puerto, memoriaAlAzar->ip);
	if (socketMemoria < 0) {
		log_error(log_master->logError, "Error de conexion");
		return SUPER_ERROR;

	} else {
		log_info(log_master->logInfo, "Conexion existosa");
		char request[100];
		sprintf(request, "%s", nombreTabla);
		Paquete paquete;
		if (enviarInfoMemoria(socketMemoria, request, DROP, &paquete) == SUPER_ERROR) {
			return SUPER_ERROR;
		} else {

			bool findByNombre(infoTabla* tablaActual) {
				return strcmp(nombreTabla, tablaActual->nombreTabla) == 0;
			}
			list_remove_and_destroy_by_condition(listaInfoTablas, (void*) findByNombre, (void*) freeInfoTabla);
		}

	}

	return TODO_OK;
}

void freeInfoTabla(infoTabla* tabla) {
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

			tam = strlen(ejemplo) + 1;
			char* aGuardar = (char*) malloc(tam);
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
	for (int i = 0; i < multiprocesamiento; i++)
		sem_post(&fin);
}

