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
				procesarInput(paquete.mensaje);
				break;
			case (DROP):
				procesarInput(paquete.mensaje);
				break;

			case (CREATE):
							procesarInput(paquete.mensaje);
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
	close(socketMemoria);
}

