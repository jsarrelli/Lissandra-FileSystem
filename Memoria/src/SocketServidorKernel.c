#include "SocketServidorKernel.h"

//void escuchar(int listenningSocket) {
//	listen(listenningSocket, BACKLOG); // es una syscall bloqueante
//	printf("Escuchando...\n");
//
//	struct sockaddr_in datosConexionCliente; // Esta estructura contendra los datos de la conexion del cliente. IP, puerto, etc.
//	socklen_t datosConexionClienteSize = sizeof(datosConexionCliente);
//	int socketCliente = accept(listenningSocket, (struct sockaddr *) &datosConexionCliente, &datosConexionClienteSize);
//	int status = 1;
//	printf("Kernel conectado. Esperando envío de mensajes.\n");
//	while (status != 0) {
//		int codOp;
//		status = recv(socketCliente, &codOp, 4, 0);
//		if (status != 0) {
//			switch (codOp) {
//			case 1:
//				recibirYDeserealizarINSERT(socketCliente);
//				break;
//			default:
//				break;
//			}
//		}
//	}
//	printf("Cliente Desconectado.\n");
//	close(socketCliente);
//}


