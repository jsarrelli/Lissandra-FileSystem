#include "SocketServidorMemoria.h"

void escuchar(int listenningSocket) {
	listen(listenningSocket, BACKLOG); // es una syscall bloqueante
	printf("Escuchando...\n");

	struct sockaddr_in datosConexionCliente; // Esta estructura contendra los datos de la conexion del cliente. IP, puerto, etc.
	socklen_t datosConexionClienteSize = sizeof(datosConexionCliente);
	while (true) {
		int socketKernel = accept(listenningSocket, (struct sockaddr *) &datosConexionCliente,
				&datosConexionClienteSize);
		if (socketKernel != -1) {
			//no  me convence esto del multihilo
			pthread_t threadId;
			pthread_create(&threadId, NULL, (void*) procesarAccion, (void*) socketKernel);
			pthread_detach(threadId);
			printf("Escuchando.. \n");
		}

	}

}

void procesarAccion(int socketKernel) {
	Paquete paquete;
	void* datos;
	if (RecibirPaqueteServidor(socketKernel, FILESYSTEM, &paquete) > 0) {
		if (paquete.header.quienEnvia == KERNEL) {
			datos = malloc(paquete.header.tamanioMensaje);
			datos = paquete.mensaje;
			switch ((int) paquete.header.tipoMensaje) {
			case (SELECT):
				procesarRequestSELECT(datos, socketKernel);
				break;
			case (INSERT):
				procesarRequestINSERT(datos, socketKernel);
				break;
			case (CREATE):
				procesarRequestCREATE(datos, socketKernel);
				break;

			case (DESCRIBE):
				procesarRequestDESCRIBE(datos, socketKernel);
				break;

			case (DESCRIBE_ALL):
				procesarRequestDESCRIBE_ALL(socketKernel);
				break;

			case (DROP):
				procesarConsulta(paquete.mensaje);
				break;

			}

		}
		else {
			log_info(logger, "No es ningun proceso de Kernel");
		}

	}

	if (paquete.mensaje != NULL) {
		free(paquete.mensaje);
	}
	close(socketKernel);
}

void procesarRequestSELECT(char* request, int socketKernel) {
	t_registro* registro = procesarSELECT(request);

	if (registro != NULL) {
		char response[100];
		sprintf(response, "%d %s %f", registro->key, registro->value, registro->timestamp);
		EnviarDatosTipo(socketKernel, MEMORIA, response, strlen(response) + 1, SELECT);
	}
	else {
		enviarSuccess(1, SELECT, socketKernel);
	}
}

void procesarRequestINSERT(char* request, int socketKernel) {
	t_registro* registro = procesarINSERT(request);
	if (registro != NULL) {
		enviarSuccess(0, INSERT, socketKernel);
	}
	else {
		enviarSuccess(1, INSERT, socketKernel);
	}
}

void procesarRequestCREATE(char* request, int socketKernel) {
	int success = procesarCREATE(request);
	enviarSuccess(success, INSERT, socketKernel);
}

void procesarRequestDESCRIBE(char* nombreTabla, int socketKernel) {
	t_metadata_tabla* metaData = DESCRIBE_MEMORIA(nombreTabla);
	if (metaData != NULL) {
		char response[100];
		sprintf(response, "%s %s %d %d", nombreTabla, getConsistenciaCharByEnum(metaData->CONSISTENCIA),
				metaData->CANT_PARTICIONES, metaData->T_COMPACTACION);
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
}

void procesarRequestDROP(char* nombreTabla) {
	procesarDROP(nombreTabla);
	//este no le avisa nada a nadie, porque es rebelde
	//y porque el enunciado no dice que avise
}

void enviarSuccess(int resultado, t_protocolo protocolo, int socketKernel) {
	char success[2];
	sprintf(success, "%d", resultado);
	EnviarDatosTipo(socketKernel, MEMORIA, success, 2, protocolo);
}
