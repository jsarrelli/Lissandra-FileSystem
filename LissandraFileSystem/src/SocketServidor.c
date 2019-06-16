#include "SocketServidor.h"

void escuchar(int listenningSocket) {
	listen(listenningSocket, BACKLOG); // es una syscall bloqueante
	printf("\nEscuchando...\n");

	struct sockaddr_in datosConexionCliente; // Esta estructura contendra los datos de la conexion del cliente. IP, puerto, etc.
	socklen_t datosConexionClienteSize = sizeof(datosConexionCliente);
	while (true) {
		socketMemoria = accept(listenningSocket, (struct sockaddr *) &datosConexionCliente, &datosConexionClienteSize);
		if (socketMemoria != -1) {
			pthread_t threadId;
			pthread_create(&threadId, NULL, (void*) procesarAccion, (void*) socketMemoria);
			pthread_detach(threadId);
			printf("Escuchando.. \n");
		}

	}
}

void procesarAccion(int socketMemoria) {
	Paquete paquete;
	void* datos;
	if (RecibirPaqueteServidor(socketMemoria, FILESYSTEM, &paquete) > 0) {
		if (paquete.header.quienEnvia == MEMORIA) {
			datos = malloc(paquete.header.tamanioMensaje);
			datos = paquete.mensaje;
			int valueMaximo = 100;
			switch ((int) paquete.header.tipoMensaje) {
			case (CONEXION_INICIAL_FILESYSTEM_MEMORIA):
				EnviarDatosTipo(socketMemoria, FILESYSTEM, &valueMaximo, sizeof(valueMaximo), CONEXION_INICIAL_FILESYSTEM_MEMORIA);
				break;
			case (SELECT):
				//
				break;
			case (INSERT):
				procesarINSERT(paquete.mensaje);
				break;
			case (DROP):
				procesarInput(paquete.mensaje);
				break;

			case (CREATE):
				procesarCREATE(paquete.mensaje);
				break;
			case (DESCRIBE):
				//
				break;
			}

		} else {
			log_info(logger, "No es ningun proceso de Memoria");
		}

	}

	if (paquete.mensaje != NULL) {
		free(paquete.mensaje);
	}

}

void procesarINSERT(char* request) {
	char** valores = string_split(request, "'"); //34 son las " en ASCII
	char** valoresAux = string_split(valores[0], " ");
	char* nombreTabla = valoresAux[0];
	char* key = valoresAux[1];
	char* value = valores[1];
	double timeStamp;
	if (valores[2] == NULL) {
		timeStamp = getCurrentTime();
	} else {
		timeStamp = atof(valores[2]);
	}
	char* success = malloc(1);

	int resultado = funcionINSERT(timeStamp, nombreTabla, key, value);

	enviarSuccess(resultado, INSERT);

}

void procesarCREATE(char* request) {
	char** valores = string_split(request, " ");
	char* nombreTabla = valores[0];
	char* consistenciaChar = valores[1];
	char* cantParticiones = valores[2];
	char* tiempoCompactacion = valores[3];

	int resultado = funcionCREATE(nombreTabla, cantParticiones, consistenciaChar, tiempoCompactacion);
	enviarSuccess(resultado, CREATE);
}

void procesarDROP(char* nombreTabla) {
	int resultado = funcionDROP(nombreTabla);
	enviarSuccess(resultado, DROP);
}

void enviarSuccess(int resultado, t_protocolo protocolo) {
	char* success = malloc(1);
	if (resultado == 0) {
		strcpy(success, "0");
	} else {
		strcpy(success, "1");
	}

	EnviarDatosTipo(socketMemoria, FILESYSTEM, success, 1, protocolo);
	free(success);
}
