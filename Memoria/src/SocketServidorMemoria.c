#include "SocketServidorMemoria.h"

void escuchar(int listenningSocket, int socketFileSystemRecibido) {
	socketFileSystem = socketFileSystemRecibido;
	listen(listenningSocket, BACKLOG); // es una syscall bloqueante
	printf("Escuchando...\n");

	struct sockaddr_in datosConexionCliente; // Esta estructura contendra los datos de la conexion del cliente. IP, puerto, etc.
	socklen_t datosConexionClienteSize = sizeof(datosConexionCliente);
	while (true) {
		int socketKernel = accept(listenningSocket, (struct sockaddr *) &datosConexionCliente,
				&datosConexionClienteSize);
		if (socketKernel != -1) {
			pthread_t threadId;
			pthread_create(&threadId, NULL, procesarAccion, (void*) socketKernel);
			pthread_detach(threadId);
			printf("Escuchando.. \n");
		}
		close(socketKernel);
	}

}

void procesarAccion(int socketKernel) {
	Paquete paquete;
	void* datos;
	if (RecibirPaqueteServidor(socketKernel, FILESYSTEM, &paquete) > 0) {
		if (paquete.header.quienEnvia == KERNEL) {
			datos = malloc(paquete.header.tamanioMensaje);
			datos = paquete.mensaje;
			switch (paquete.header.tipoMensaje) {
			case (SELECT):
				datos = procesarConsulta(paquete.mensaje);
				EnviarDatosTipo(socketFileSystem, MEMORIA, datos, sizeof(datos), INSERT);
				break;
			case (INSERT):
				procesarConsulta(paquete.mensaje);
				break;
			case (CREATE):
				procesarConsulta(paquete.mensaje);
				break;

			case (DESCRIBE):
				datos = procesarConsulta(paquete.mensaje);
				//aca vamos a tener que enviar algo, proximamente..
				break;

			case (DROP):
				procesarConsulta(paquete.mensaje);
				break;

			}

		}
		else {
			log_info(logger, "No es ningun proceso de Memoria");
		}

	}

	if (paquete.mensaje != NULL) {
		free(paquete.mensaje);
	}
}

