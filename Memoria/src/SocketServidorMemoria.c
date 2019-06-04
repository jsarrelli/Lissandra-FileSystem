#include "SocketServidorMemoria.h"

void escuchar(int listenningSocket) {
	listen(listenningSocket, BACKLOG); // es una syscall bloqueante
	printf("Escuchando...\n");

	struct sockaddr_in datosConexionCliente; // Esta estructura contendra los datos de la conexion del cliente. IP, puerto, etc.
	socklen_t datosConexionClienteSize = sizeof(datosConexionCliente);
	while (true) {
		socketKernel = accept(listenningSocket, (struct sockaddr *) &datosConexionCliente, &datosConexionClienteSize);
		if(socketKernel!= -1){
			procesarAccion(socketKernel);
			printf("Escuchando.. \n");
		}
		close(socketKernel);
	}

}

void procesarAccion(int socketMemoria) {
	Paquete paquete;
	void* datos;
	if (RecibirPaqueteServidor(socketMemoria, FILESYSTEM, &paquete) > 0) {
		if (paquete.header.quienEnvia == MEMORIA) {
			datos = malloc(paquete.header.tamanioMensaje);
			datos = paquete.mensaje;
			switch(paquete.header.tipoMensaje){
			case(SELECT):
					void* datos=procesarConsulta(paquete.mensaje);
					EnviarDatosTipo(socketMemoria, MEMORIA, datos, sizeof(datos), INSERT);
					break;
			case(INSERT):
					procesarConsulta(paquete.mensaje);
					break;
			case(DELETE):
					procesarConsulta(paquete.mensaje);
			}

		}
		else{
			log_info(logger, "No es ningun proceso de Memoria");
		}


	}

	if(paquete.mensaje!=NULL){
		free(paquete.mensaje);
	}
}


