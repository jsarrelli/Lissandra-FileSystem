#include "SocketServidorMemoria.h"

void escuchar(int listenningSocket) {
	listen(listenningSocket, BACKLOG); // es una syscall bloqueante
	log_info(loggerInfo, "Servidor memoria escuchando..");
	while (true) {
		struct sockaddr_in datosConexionCliente; // Esta estructura contendra los datos de la conexion del cliente. IP, puerto, etc.
		socklen_t datosConexionClienteSize = sizeof(datosConexionCliente);
		int socketCliente = accept(listenningSocket, (struct sockaddr *) &datosConexionCliente, &datosConexionClienteSize);
		if (socketCliente != -1) {
			//no  me convence esto del multihilo
//			pthread_t threadId;
//			pthread_create(&threadId, NULL, (void*) procesarAccion, (void*) socketKernel);
//			pthread_detach(threadId);

			//O hace un journal o procesa una accion, boludeces no
			pthread_mutex_lock(&lockMemoria);
			procesarAccion(socketCliente);
			close(socketCliente);
			pthread_mutex_unlock(&lockMemoria);

			printf("Escuchando.. \n");
		}


	}

}

void procesarAccion(int socketEntrante) {
	Paquete paquete;
	void* datos;
	if (RecibirPaquete(socketEntrante, &paquete) > 0) {
		usleep(configuracion->RETARDO_MEMORIA * 1000);
		if (paquete.header.quienEnvia == KERNEL) {
			datos = paquete.mensaje;
			switch ((int) paquete.header.tipoMensaje) {
			case (SELECT):
				procesarRequestSELECT(datos, socketEntrante);
				break;
			case (INSERT):
				procesarRequestINSERT(datos, socketEntrante);
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
	}
}

void procesarRequestSELECT(char* request, int socketKernel) {
	log_info(loggerInfo, "Procesando SELECT");
	t_registro_memoria* registro = procesarSELECT(request);

	if (registro != NULL) {
		char* response = string_new();
		string_append_with_format(&response, "%d \"%s\" %f", registro->key, registro->value, registro->timestamp);
		EnviarDatosTipo(socketKernel, MEMORIA, response, strlen(response) + 1, SELECT);
		free(response);
	} else {
		enviarSuccess(1, SELECT, socketKernel);
	}
}

void procesarRequestINSERT(char* request, int socketKernel) {
	char* consulta = string_duplicate(request);
	log_info(loggerInfo, "Procesando INSERT");
	t_registro_memoria* registro = procesarINSERT(consulta);
	log_info(loggerInfo, "Regitro ya insertado, enviado respuesta a Kernel");
	if (registro != NULL) {
		enviarSuccess(0, INSERT, socketKernel);
	} else {
		enviarSuccess(1, INSERT, socketKernel);
	}
}

void procesarRequestCREATE(char* request, int socketKernel) {

	log_info(loggerInfo, "Procesando CREATE. Request %s", request);
	int success = procesarCREATE(request);
	enviarSuccess(success, CREATE, socketKernel);
}

void procesarRequestDESCRIBE(char* nombreTabla, int socketKernel) {
	log_info(loggerInfo, "Procesando DESCRIBE");
	t_metadata_tabla* metaData = DESCRIBE_MEMORIA(nombreTabla);
	if (metaData != NULL) {
		char response[100];
		sprintf(response, "%s %s %d %d", nombreTabla, getConsistenciaCharByEnum(metaData->CONSISTENCIA), metaData->CANT_PARTICIONES,
				metaData->T_COMPACTACION);
		EnviarDatosTipo(socketKernel, FILESYSTEM, response, strlen(response) + 1, DESCRIBE);
		free(metaData);
	} else {
		enviarSuccess(1, DESCRIBE, socketKernel);
	}

}

void procesarRequestDESCRIBE_ALL(int socketKernel) {
	t_list* metaDatas = DESCRIBE_ALL_MEMORIA();
	void enviarMetadataAKernel(char* metadata) {
		EnviarDatosTipo(socketKernel, MEMORIA, metadata, strlen(metadata) + 1, DESCRIBE);
	}
	list_iterate(metaDatas, (void*) enviarMetadataAKernel);
	list_destroy_and_destroy_elements(metaDatas, free);

}

void procesarRequestDROP(char* nombreTabla, int socketKernel) {
	int succes = procesarDROP(nombreTabla);
	enviarSuccess(succes, DROP, socketKernel);

}

void enviarSuccess(int resultado, t_protocolo protocolo, int socketKernel) {
	char* succes = string_new();
	string_append_with_format(&succes, "%d", resultado);
	EnviarDatosTipo(socketKernel, MEMORIA, succes, strlen(succes) + 1, protocolo);
	free(succes);
}

void procesarGossiping(char* memoriaGossiping, int socketMemoria) {
	//recibimos toda la tabla
	t_memoria* memoriaRecibida = deserealizarMemoria(memoriaGossiping);
	agregarMemoriaNueva(memoriaRecibida);

	Paquete paquete;
	while (true) {
		RecibirPaqueteServidor(socketMemoria, MEMORIA, &paquete);
		if (strcmp(paquete.mensaje, "fin") == 0) {
			free(paquete.mensaje);
			break;
		}
		t_memoria* memoriaRecibida = deserealizarMemoria(memoriaGossiping);
		agregarMemoriaNueva(memoriaRecibida);
		free(paquete.mensaje);
	}

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

	list_iterate(tablaGossiping, (void*) enviarMemoria);
}
