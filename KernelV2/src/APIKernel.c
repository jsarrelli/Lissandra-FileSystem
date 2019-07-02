/*
 * APIKernel.c
 *
 *  Created on: 20 jun. 2019
 *      Author: utnso
 */

#include "APIKernel.h"

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
		consolaAdd(argumentos);
	} else if (strcmp(operacion, "RUN") == 0) {
		//	ADD MEMORY [id] TO [consistencia]
		log_trace(log_master->logTrace, "Se ha escrito el comando RUN");
		consolaRun(argumentos);
	} else if (strcmp(operacion, "EXECUTE") == 0) {
		//	ADD MEMORY [id] TO [consistencia]
		log_trace(log_master->logTrace, "Se ha escrito el comando EXECUTE");
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

void procesarAdd(int id, consistencia cons) {
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
	} else
		log_error(log_master->logError, "Problemas con el comando ADD");
}

void consolaAdd(char*argumento) {
	char** valores = string_split(argumento, " ");
//	add(atoi(valores[1]), valores[3]);
	consistencia cons = procesarConsistencia(valores[3]);

	int id = atoi(valores[1]);
	procesarAdd(id, cons);

	free(valores[0]);
	free(valores[1]);
	free(valores[2]);
	free(valores[3]);
	free(valores);
}

void consolaInsert(char*argumentos) {
	//	INSERT
	//[NOMBRE_TABLA] [KEY] “[VALUE]”
	char** valores = string_split(argumentos, "\"");
	char** valoresAux = string_split(valores[0], " ");
	char* nombreTabla = valoresAux[0];
	char* key = valoresAux[1];
	char* value = valores[1];
	log_trace(log_master->logTrace,
			"El nombre de la tabla es: %s, su key es %s, y su value es: %s",
			nombreTabla, key, value);

	obtenerMemoriaSegunTablaYKey(atoi(key), nombreTabla);

	int socketMemoria = ConectarAServidor(config->PUERTO_MEMORIA,
			config->IP_MEMORIA);
	char request[150];
	sprintf(request, "%s %s %s", nombreTabla, key, value);
	enviarInfoMemoria(socketMemoria, request, INSERT);

//	printf("Aca se tienen que poner las sockets\n");

//	infoMemoria* memoriaAEnviar = obtenerMemoria(nombreTabla, key);

	// Primero hay que obtener una memoria que cumpla con el criterio de la tabla (hecho)
	// Ahora, se usa las funciones sockets para enviar el mensaje a Memoria y luego a LFS; y listo (no requiere mensaje de retorno)

	free(valoresAux[1]);
	free(valoresAux[0]);
	free(valoresAux);
	free(valores[1]);
	free(valores[0]);
	free(valores);
}

void consolaSelect(char*argumentos) {
	char** valores = string_split(argumentos, " ");
	char* nombreTabla = valores[0];
	int key = atoi(valores[1]);

	log_trace(log_master->logTrace,
			"El nombre de la tabla es: %s, y la key es: %d", nombreTabla, key);

	// Parecido al INSERT, es decir, mando info a la memoria que cumple con la condicion, pero, a diferencia de la otra recibo una respuesta, que
	// es un value

	obtenerMemoriaSegunTablaYKey(key, nombreTabla);

	printf("Aca se tienen que poner las sockets\n");

	free(valores[1]);
	free(nombreTabla);
	free(valores);
}

void enviarInfoMemoria(int socketMemoria, char request[], t_protocolo protocolo) {
	Paquete paquete;
	int success;

	if(EnviarDatosTipo(socketMemoria, KERNEL, request, strlen(request) + 1,
			protocolo)){
		log_trace(log_master->logTrace, "El paquete se envio exitosamente");
	}
	else{
		log_error(log_master->logError, "Error al enviar paquete");
	}


	if (RecibirPaqueteCliente(socketMemoria, MEMORIA, &paquete) > 0) {
		success = atoi(paquete.mensaje);
	}
	if (success == 0) {
		log_trace(log_master->logTrace, "Paquete recibido correctamente");
	} else {
		log_error(log_master->logError,
				"Error al recibir el paquete");
	}
}

void enviarCREATE(int cantParticiones, int tiempoCompactacion,
		char* nombreTabla, char* consistenciaChar) {
	int socketMemoria = ConectarAServidor(config->PUERTO_MEMORIA,
			config->IP_MEMORIA);
	char request[100];
	sprintf(request, "%s %s %d %d", nombreTabla, consistenciaChar,
			cantParticiones, tiempoCompactacion);
	enviarInfoMemoria(socketMemoria, request, CREATE);
}

void consolaCreate(char*argumentos) {
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

//	infoMemoria* memoriaAlAzar = obtenerMemoriaAlAzarParaFunciones(); // Esto se tiene que usar como dato para las sockets, lo comento para que no me tire warning
	obtenerMemoriaAlAzarParaFunciones();

	enviarCREATE(cantParticiones, tiempoCompactacion, nombreTabla,
			consistenciaChar);

	free(valores[3]);
	free(valores[2]);
	free(valores[1]);
	free(valores[0]);
	free(valores);

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

void procesarDescribeAll(int socketMemoria, Paquete* paquete) {
	log_trace(log_master->logTrace, "Se pide la metadata de todos las tablas");
	EnviarDatosTipo(socketMemoria, KERNEL, NULL, 0, DESCRIBE_ALL);
	while (RecibirPaqueteCliente(socketMemoria, FILESYSTEM, &*paquete) > 0) {
		deserealizarYMostrarMetadata(&*paquete);
	}
}

void procesarDescribe(int socketMemoria, Paquete* paquete, char* nombreTabla) {
	log_trace(log_master->logTrace, "Se pide la metadata de %s", nombreTabla);
	EnviarDatosTipo(socketMemoria, KERNEL, nombreTabla, strlen(nombreTabla) + 1,
			DESCRIBE);
	RecibirPaqueteCliente(socketMemoria, FILESYSTEM, &*paquete);
	if (atoi(paquete->mensaje) == 1) {
		puts("La tabla no existe");
		//la tabla no existe
	}
	t_metadata_tabla* metadataRecibida = deserealizarTabla(&*paquete);
	mostrarMetadata(nombreTabla, metadataRecibida);
	//falta free metadata
	free(paquete->mensaje);
}

void consolaDescribe(char*nombreTabla) {
	obtenerMemoriaAlAzarParaFunciones();
	int socketMemoria = ConectarAServidor(config->PUERTO_MEMORIA,
			config->IP_MEMORIA);
	Paquete paquete;
	if (nombreTabla == NULL) {
		procesarDescribeAll(socketMemoria, &paquete);
	} else {
		procesarDescribe(socketMemoria, &paquete, nombreTabla);
	}

	// El DESCRIBE es igual que el CREATE y que el DROP
//	infoMemoria* memoriaAlAzar = obtenerMemoriaAlAzarParaFunciones(); // Esto se tiene que usar como dato para las sockets, lo comento para que no me tire warning

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

void consolaDrop(char*nombreTabla) {
	if (nombreTabla != NULL)
		log_trace(log_master->logTrace, "Se desea elminar la tabla %s",
				nombreTabla);

	else
		log_error(log_master->logError,
				"Error: ingresar el nombre de la tabla a eliminar");

//	infoMemoria* memoriaAlAzar = obtenerMemoriaAlAzarParaFunciones(); // Esto se tiene que usar como dato para las sockets, lo comento para que no me tire warning
	obtenerMemoriaAlAzarParaFunciones();

	int socketMemoria = ConectarAServidor(config->PUERTO_MEMORIA,
			config->IP_MEMORIA);
	char request[100];
	sprintf(request, "%s", nombreTabla);
	enviarInfoMemoria(socketMemoria, request, DROP);

}

void consolaRun(char*path) {
	FILE* fd = NULL;
	int num_lineas = contarLineasArchivo(fd, path);
	if (num_lineas != 0) {
		fd = fopen(path, "rt");
		procExec* proceso = newProceso();

		for (int i = 0; i < num_lineas; i++) {
			char ejemplo[500];
			fgets(ejemplo, sizeof(ejemplo), fd);
			size_t tam = strlen(ejemplo) + 1;
			ejemplo[tam - 2] = '\0';
			char* aGuardar = (char*) malloc(tam - 1);
			strcpy(aGuardar, ejemplo);
			agregarRequestAlProceso(proceso, aGuardar);
//			printf("La instruccion es: %s", (char*)list_get(proceso->script, i));
		}

		fclose(fd);
		deNewAReady(proceso);

		void imprimirRequest(char*request) {
			printf("La instruccion es: %s\n", request);
		}
		list_iterate(((procExec*) queue_peek(colaReady))->script,
				(void*) imprimirRequest);

	} else
		log_error(log_master->logError,
				"El path no es correcto o el archivo esta dañado");
}

/*
 * Aclaracion: esta funcion no funciona con los archivos de ejemplo
 * del tp (sin sockets) ya que para eso tendria que hardcodear una bocha de cosas
 *
 * Solo para probar el funcionamiento basico del Kernel (con un solo procesador) y sin sockets
 *
 * (No soy el LFS)
 */

void consolaExecute() {
	for (int i = 0; i < queue_size(colaReady) - 1; i++)
		funcionThread(NULL);
}
