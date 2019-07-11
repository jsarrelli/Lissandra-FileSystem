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
	}  else if (strcmp(operacion, "RUN") == 0) {
		log_trace(log_master->logTrace, "Se ha escrito el comando JOURNAL");
		if (consolaJournal() == SUPER_ERROR)
			return SUPER_ERROR;
	}else if (strcmp(operacion, "SALIR") == 0) {
		consolaSALIR(argumentos);

	} else {
		log_error(log_master->logError,
				"El comando no es el correcto. Por favor intente nuevamente");
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
	if ((memoriaEncontrada = list_find(listaMemorias, _esCondicionAdd)) != NULL) {
		asignarCriterioMemoria(memoriaEncontrada, cons);

		if (!(memoriaEncontrada->criterios)[3])
			log_trace(log_master->logTrace,
					"Se ha asignado el criterio a la memoria correctammente");

		imprimirCriterio(memoriaEncontrada->criterios);
		log_trace(log_master->logTrace, "El id de esta memoria es: %d",
				memoriaEncontrada->id);
	} else {
		log_error(log_master->logError, "Problemas con el comando ADD");
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

	free(valores[0]);
	free(valores[1]);
	free(valores[2]);
	free(valores[3]);
	free(valores);

	return TODO_OK;
}

int consolaInsert(char*argumentos) {
	//	INSERT
	//[NOMBRE_TABLA] [KEY] “[VALUE]” TIMESTAMP

	metricas.writes++;

	timestampInsertAlIniciar = getCurrentTime();

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
	log_trace(log_master->logTrace,
			"El nombre de la tabla es: %s, su key es %s, y su value es: %s",
			nombreTabla, key, value);

	obtenerMemoriaSegunTablaYKey(atoi(key), nombreTabla, INSERT);

	timestampInsertAlFinalizar = getCurrentTime();

	double* diferencia = malloc(sizeof(double));
	*diferencia = timestampInsertAlFinalizar - timestampInsertAlIniciar;

	list_add(metricas.diferenciaDeTiempoWriteLatency, diferencia);

	cantInserts++;


	int socketMemoria = ConectarAServidor(config->PUERTO_MEMORIA,
			config->IP_MEMORIA);

	if (enviarInfoMemoria(socketMemoria, argumentos, INSERT) == SUPER_ERROR)
		return SUPER_ERROR;

//	infoMemoria* memoriaAEnviar = obtenerMemoria(nombreTabla, key);

	free(valoresAux[1]);
	free(valoresAux[0]);
	free(valoresAux);
	free(valores[1]);
	free(valores[0]);
	free(valores);

	return TODO_OK;
}

int consolaSelect(char*argumentos) {
	metricas.reads++;
	timestampSelectAlIniciar = getCurrentTime();
	char** valores = string_split(argumentos, " ");
	char* nombreTabla = valores[0];
	int key = atoi(valores[1]);

	log_trace(log_master->logTrace,
			"El nombre de la tabla es: %s, y la key es: %d", nombreTabla, key);

	// Parecido al INSERT, es decir, mando info a la memoria que cumple con la condicion, pero, a diferencia de la otra recibo una respuesta, que
	// es un value

	if (obtenerMemoriaSegunTablaYKey(key, nombreTabla, SELECT) == SUPER_ERROR)
		return SUPER_ERROR;

//	infoMemoria* memoriaAEnviar = obtenerMemoria(nombreTabla, key);

	free(valores[1]);
	free(nombreTabla);
	free(valores);

	timestampSelectAlFinalizar = getCurrentTime();

	double* diferencia = malloc(sizeof(double));
	*diferencia = timestampSelectAlFinalizar - timestampSelectAlIniciar;

	list_add(metricas.diferenciaDeTiempoReadLatency, diferencia);

	cantSelects++;

	return TODO_OK;
}

int enviarInfoMemoria(int socketMemoria, char request[], t_protocolo protocolo) {
	Paquete paquete;
	int success;

	if (EnviarDatosTipo(socketMemoria, KERNEL, request, strlen(request) + 1,
			protocolo)) {
		log_trace(log_master->logTrace, "El paquete se envio exitosamente");
	} else {
		log_error(log_master->logError, "Error al enviar paquete");
		return SUPER_ERROR;
	}

	if (RecibirPaqueteCliente(socketMemoria, MEMORIA, &paquete) > 0) {
		success = atoi(paquete.mensaje);
	}
	if (success == 0) {
		log_trace(log_master->logTrace, "Paquete recibido correctamente");
	} else {
		log_error(log_master->logError, "Error al recibir el paquete");
		return SUPER_ERROR;
	}
	return ERROR;
}

int enviarCREATE(int cantParticiones, int tiempoCompactacion, char* nombreTabla,
		char* consistenciaChar) {

	int socketMemoria = ConectarAServidor(config->PUERTO_MEMORIA,
			config->IP_MEMORIA);
	char request[100];
	sprintf(request, "%s %s %d %d", nombreTabla, consistenciaChar,
			cantParticiones, tiempoCompactacion);
	if (enviarInfoMemoria(socketMemoria, request, CREATE) == SUPER_ERROR)
		return SUPER_ERROR;
	return TODO_OK;
}

void enviarJournalMemoria(int socketMemoria) {
	log_trace(log_master->logTrace, "Comando JOURNAL enviado a Memoria");
	EnviarDatosTipo(socketMemoria, KERNEL, NULL, 0, JOURNAL);
}

void consolaJournal(){
//
//	void journalMemoria(infoMemoria* memoria){
//		int socketMemoria= ConectarAServidor(memoria., ip)
//	}
//	list_iterate(listaMemorias, journalMemoria);
}

int consolaCreate(char*argumentos) {
	char** valores = string_split(argumentos, " ");
	char* nombreTabla = valores[0];
	char* consistenciaChar = valores[1];
	int cantParticiones = atoi(valores[2]);
	int tiempoCompactacion = atoi(valores[3]);

	log_trace(log_master->logTrace,
			"El nombre de la tabla es: %s, la consistencia es: %s, la cantParticiones:%d, y el tiempoCompactacion es: %d",
			nombreTabla, consistenciaChar, cantParticiones, tiempoCompactacion);

	// En el caso del CREATE, como el de muchas otras funciones, se le manda la info a una memoria al azar y no recibe respuesta

//	infoMemoria* memoriaAlAzar = obtenerMemoriaAlAzarParaFunciones(); // Esto se tiene que usar como dato para las sockets, lo comento para que no me tire warning
	obtenerMemoriaAlAzarParaFunciones();

	if (enviarCREATE(cantParticiones, tiempoCompactacion, nombreTabla,
			consistenciaChar) == SUPER_ERROR)
		return SUPER_ERROR;

	free(valores[3]);
	free(valores[2]);
	free(valores[1]);
	free(valores[0]);
	free(valores);

	return TODO_OK;

}

void deserealizarYMostrarMetadata(Paquete* paquete) {
	char* mensaje = malloc(paquete->header.tamanioMensaje + 1);
	mensaje = strcpy(mensaje, (char*) paquete->mensaje);
	char** datos = string_split(mensaje, " ");
	char* nombreTabla = malloc(strlen(datos[0]) + 1);
	strcpy(nombreTabla, datos[0]);
	t_consistencia consistencia = getConsistenciaByChar(datos[1]);
	int cantParticiones = atoi(datos[2]);
	int tiempoCompactacion = atoi(datos[3]);
	//creo la metadata
	t_metadata_tabla* metadata = malloc(sizeof(t_metadata_tabla));
	metadata->CONSISTENCIA = consistencia;
	metadata->CANT_PARTICIONES = cantParticiones;
	metadata->T_COMPACTACION = tiempoCompactacion;
	free(datos);
	mostrarMetadata(nombreTabla, metadata);
	free(nombreTabla);
}

int procesarDescribeAll(int socketMemoria, Paquete* paquete) {
	int estadoRecibir = 0;
	log_trace(log_master->logTrace, "Se pide la metadata de todos las tablas");
	if (EnviarDatosTipo(socketMemoria, KERNEL, NULL, 0,
			DESCRIBE_ALL)==SUPER_ERROR)
		return SUPER_ERROR;

	while ((estadoRecibir = RecibirPaqueteCliente(socketMemoria, FILESYSTEM,
			&*paquete)) > 0) {
		deserealizarYMostrarMetadata(&*paquete);
	}

	if (estadoRecibir < 0)
		return SUPER_ERROR;
	return TODO_OK;
}

int procesarDescribe(int socketMemoria, Paquete* paquete, char* nombreTabla) {
	log_trace(log_master->logTrace, "Se pide la metadata de %s", nombreTabla);
	EnviarDatosTipo(socketMemoria, KERNEL, nombreTabla, strlen(nombreTabla) + 1,
			DESCRIBE);
	if (RecibirPaqueteCliente(socketMemoria, FILESYSTEM, &*paquete) < 0)
		return SUPER_ERROR;
	if (atoi(paquete->mensaje) == 1) {
		puts("La tabla no existe");
	}
	t_metadata_tabla* metadataRecibida = deserealizarTabla(&*paquete);
	mostrarMetadata(nombreTabla, metadataRecibida);
	//falta free metadata
	free(paquete->mensaje);
	return TODO_OK;
}

int consolaDescribe(char*nombreTabla) {
	obtenerMemoriaAlAzarParaFunciones();
	int socketMemoria = ConectarAServidor(config->PUERTO_MEMORIA,
			config->IP_MEMORIA);
	Paquete paquete;
	if (nombreTabla == NULL) {
		if (procesarDescribeAll(socketMemoria, &paquete) == SUPER_ERROR)
			return SUPER_ERROR;
	} else {
		if (procesarDescribe(socketMemoria, &paquete, nombreTabla)==SUPER_ERROR)
			return SUPER_ERROR;
	}

//	 El DESCRIBE es igual que el CREATE y que el DROP
//	infoMemoria* memoriaAlAzar = obtenerMemoriaAlAzarParaFunciones(); // Esto se tiene que usar como dato para las sockets, lo comento para que no me tire warning

	return TODO_OK;
}

void mostrarMetadata(char* nombreTabla, t_metadata_tabla* metadata) {

	log_info(log_master->logInfo, "Segmento: %s", nombreTabla);
	log_info(log_master->logInfo,
			"Consistencia: %s / cantParticiones: %d / tiempoCompactacion: %d",
			getConsistenciaCharByEnum(metadata->CONSISTENCIA),
			metadata->CANT_PARTICIONES, metadata->T_COMPACTACION);
}

t_metadata_tabla* deserealizarTabla(Paquete* paquete) {
	char* mensaje = malloc(paquete->header.tamanioMensaje + 1);
	mensaje = strcpy(mensaje, (char*) paquete->mensaje);
	char** datos = string_split(mensaje, " ");

	t_consistencia consistencia = getConsistenciaByChar(datos[1]);
	int cantParticiones = atoi(datos[2]);
	int tiempoCompactacion = atoi(datos[3]);
	//creo la metadata
	t_metadata_tabla* metadata = malloc(sizeof(t_metadata_tabla));
	metadata->CONSISTENCIA = consistencia;
	metadata->CANT_PARTICIONES = cantParticiones;
	metadata->T_COMPACTACION = tiempoCompactacion;

	free(datos);
	return metadata;
}

int consolaDrop(char*nombreTabla) {
	if (nombreTabla != NULL)
		log_trace(log_master->logTrace, "Se desea elminar la tabla %s",
				nombreTabla);
	else {
		log_error(log_master->logError,
				"Error: ingresar el nombre de la tabla a eliminar");
		return SUPER_ERROR;
	}

//	infoMemoria* memoriaAlAzar = obtenerMemoriaAlAzarParaFunciones(); // Esto se tiene que usar como dato para las sockets, lo comento para que no me tire warning
	obtenerMemoriaAlAzarParaFunciones();

	int socketMemoria = ConectarAServidor(config->PUERTO_MEMORIA,
			config->IP_MEMORIA);
	char request[100];
	sprintf(request, "%s", nombreTabla);
	if (enviarInfoMemoria(socketMemoria, request, DROP) == SUPER_ERROR)
		return SUPER_ERROR;

	return TODO_OK;
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

			char* aGuardar = (char*) malloc(tam - 1);
			strcpy(aGuardar, ejemplo);
			agregarRequestAlProceso(proceso, aGuardar);
		}

		fclose(fd);
		deNewAReady(proceso);

	} else {
		log_error(log_master->logError,
				"El path no es correcto o el archivo esta dañado");
		return SUPER_ERROR;
	}
	return TODO_OK;
}

void consolaSALIR(char*nada) {
	log_trace(log_master->logTrace, "Finalizando consola");

	puedeHaberRequests = false;
	for (int i = 0; i < multiprocesamiento; i++)
		sem_post(&cantProcesosColaReady);
	usleep(5);
	sem_post(&fin);
}

