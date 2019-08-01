#include "SocketServidorMemoria.h"

void escuchar(int listenningSocket) {
	listen(listenningSocket, SOMAXCONN); // es una syscall bloqueante
	log_info(loggerInfo, "Servidor memoria escuchando..");
	while (true) {
		struct sockaddr_in datosConexionCliente; // Esta estructura contendra los datos de la conexion del cliente. IP, puerto, etc.
		socklen_t datosConexionClienteSize = sizeof(datosConexionCliente);

		int socketCliente = accept(listenningSocket, (struct sockaddr *) &datosConexionCliente, &datosConexionClienteSize);
		if (socketCliente != -1) {
			log_info(loggerInfo, "Esperando requests...");
			pthread_t threadId;
			pthread_create(&threadId, NULL, (void*) procesarAccion, (void*) socketCliente);
			pthread_detach(threadId);

		}

	}

}

void procesarAccion(int socketEntrante) {
	Paquete paquete;
	void* datos;
	int cantDatosRecibidos = RecibirPaquete(socketEntrante, &paquete);

	log_info(loggerInfo, "Request en memoria recibida: %s / %d", paquete.mensaje, cantDatosRecibidos);

	if (cantDatosRecibidos > 0) {
		usleep(configuracion->RETARDO_MEMORIA * 1000);
		if (paquete.header.quienEnvia == KERNEL) {

			datos = paquete.mensaje;
			switch ((int) paquete.header.tipoMensaje) {
			case (SELECT):
				pthread_mutex_lock(&mutexSelect);
				procesarRequestSELECT(datos, socketEntrante);
				pthread_mutex_unlock(&mutexSelect);
				break;
			case (INSERT):
				pthread_mutex_lock(&mutexInsert);
				procesarRequestINSERT(datos, socketEntrante);
				pthread_mutex_unlock(&mutexInsert);
				break;
			case (CREATE):

				procesarRequestCREATE(datos, socketEntrante);

				break;

			case (DESCRIBE):
				procesarRequestDESCRIBE(datos, socketEntrante);
				break;

			case (DESCRIBE_ALL):
				procesarRequestDESCRIBE_ALL(socketEntrante);
				break;

			case (DROP):

				procesarRequestDROP(datos, socketEntrante);

				break;

			case (TABLA_GOSSIPING):
				procesarRequestTABLA_GOSSIPING(socketEntrante);
				break;
			case (JOURNAL):

				JOURNAL_MEMORIA();
				break;
			}

		} else if (paquete.header.quienEnvia == MEMORIA && paquete.header.tipoMensaje == GOSSIPING) {
			log_info(loggerInfo, "Request de tabla gossiping recibido");
			procesarGossiping(paquete.mensaje, socketEntrante);

		} else {
			log_info(loggerInfo, "No es ningun proceso valido para Memoria");
		}
		free(paquete.mensaje);
		//close(socketEntrante);
	}

	if (socketEntrante != -1) {
		close(socketEntrante);
	}

}

void procesarRequestSELECT(char* request, int socketKernel) {
	log_info(loggerInfo, "Procesando SELECT: %s", request);

	t_registro_memoria* registro = procesarSELECT(request);

	if (registro != NULL) {
		char* response = string_new();
		string_append_with_format(&response, "%d \"%s\" %f", registro->key, registro->value, registro->timestamp);
		log_info(loggerInfo, "Enviado registro a kernel: %s", response);
		EnviarDatosTipo(socketKernel, MEMORIA, response, strlen(response) + 1, SELECT);
		free(response);
	} else {
		enviarSuccess(-1, SELECT, socketKernel);
	}

}

void procesarRequestINSERT(char* request, int socketKernel) {
	char* consulta = string_duplicate(request);
	log_info(loggerInfo, "Procesando INSERT: %s", request);
	t_registro_memoria* registro = procesarINSERT(consulta);
	log_info(loggerInfo, "Regitro ya insertado, enviado respuesta a Kernel");
	if (registro != NULL) {
		enviarSuccess(0, INSERT, socketKernel);
	} else {
		enviarSuccess(1, INSERT, socketKernel);
	}
	free(consulta);
}

void procesarRequestCREATE(char* request, int socketKernel) {

	log_info(loggerInfo, "Procesando CREATE: %s", request);
	int success = procesarCREATE(request);
	enviarSuccess(success, CREATE, socketKernel);
}

void procesarRequestDESCRIBE(char* nombreTabla, int socketKernel) {
	log_info(loggerInfo, "Procesando DESCRIBE: %s", nombreTabla);
	t_metadata_tabla* metaData = DESCRIBE_MEMORIA(nombreTabla);
	if (metaData != NULL) {
		char response[100];
		sprintf(response, "%s %s %d %d", nombreTabla, getConsistenciaCharByEnum(metaData->CONSISTENCIA), metaData->CANT_PARTICIONES,
				metaData->T_COMPACTACION);
		EnviarDatosTipo(socketKernel, FILESYSTEM, response, strlen(response) + 1, DESCRIBE);
		free(metaData);
	} else {
		enviarSuccess(-1, DESCRIBE, socketKernel);
	}

}

void procesarRequestDESCRIBE_ALL(int socketKernel) {
	char* tablasSerializadas = DESCRIBE_ALL_MEMORIA();
	if (tablasSerializadas == NULL) {
		enviarSuccess(1, DESCRIBE_ALL, socketKernel);
	} else {
		EnviarDatosTipo(socketKernel, MEMORIA, tablasSerializadas, strlen(tablasSerializadas) + 1, DESCRIBE_ALL);
		free(tablasSerializadas);
	}

}

void procesarRequestDROP(char* nombreTabla, int socketKernel) {
	log_info(loggerInfo, "Procesando DROP: %s", nombreTabla);
	int succes = procesarDROP(nombreTabla);
	enviarSuccess(succes, DROP, socketKernel);
}

void enviarSuccess(int resultado, t_protocolo protocolo, int socketKernel) {
	char* succes = string_new();
	string_append_with_format(&succes, "%d", resultado);
	EnviarDatosTipo(socketKernel, MEMORIA, succes, strlen(succes) + 1, protocolo);
	free(succes);
}

void procesarGossiping(char* memoriasGossiping, int socketMemoria) {
	log_info(loggerInfo, "Enviando success..");
	//recibimos toda la tabla
	int i = 0;
	char** memorias = string_split(memoriasGossiping, "/");
	while (memorias[i] != NULL) {
		t_memoria* memoriaRecibida = deserealizarMemoria(memorias[i]);
		agregarMemoriaNueva(memoriaRecibida);
		i++;
	}
	freePunteroAPunteros(memorias);

	//enviamos nuestra tabla
	enviarTablaGossiping(socketMemoria);
}

void procesarRequestTABLA_GOSSIPING(int socketKernel) {
	log_info(loggerInfo, "Request tabla gossiping recibida de Kernel");

	char* tiempoGossiping = string_new();
	string_append_with_format(&tiempoGossiping, "%d", configuracion->TIEMPO_GOSSIPING);
	EnviarDatosTipo(socketKernel, MEMORIA, tiempoGossiping, strlen(tiempoGossiping) + 1, TABLA_GOSSIPING);
	free(tiempoGossiping);

	void enviarMemoria(t_memoria* memoria) {
		char* response = string_new();
		string_append_with_format(&response, "%s %s %d", memoria->ip, memoria->puerto, memoria->memoryNumber);
		EnviarDatosTipo(socketKernel, MEMORIA, response, strlen(response) + 1, TABLA_GOSSIPING);
		free(response);
	}
	filtrarMemoriasConocidas();
	list_iterate(tablaGossiping, (void*) enviarMemoria);
}
