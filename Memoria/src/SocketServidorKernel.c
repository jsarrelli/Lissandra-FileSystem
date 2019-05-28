#include "SocketServidorKernel.h"




int configurarSocketServidor(){

		struct addrinfo hints;
		struct addrinfo *serverInfo;

		memset(&hints, 0, sizeof(hints));
		hints.ai_family = AF_UNSPEC;		// No importa si uso IPv4 o IPv6
		hints.ai_flags = AI_PASSIVE;		// Asigna el address del localhost: 127.0.0.1
		hints.ai_socktype = SOCK_STREAM;	// Indica que usaremos el protocolo TCP

		getaddrinfo(NULL, PUERTO, &hints, &serverInfo); // Notar que le pasamos NULL como IP, ya que le indicamos que use localhost en AI_PASSIVE
		listenningSocket = socket(serverInfo->ai_family, serverInfo->ai_socktype, serverInfo->ai_protocol);

		int activado=1;
		setsockopt(listenningSocket,SOL_SOCKET,SO_REUSEADDR,&activado,sizeof(activado));//Para que reuse el puerto si esta ocupado

		if(bind(listenningSocket,serverInfo->ai_addr, serverInfo->ai_addrlen)!=0){
			perror("Fallo el bind");
			return 0;
		}
		freeaddrinfo(serverInfo);
		return 1;
}

void escuchar() {
	listen(listenningSocket, BACKLOG); // es una syscall bloqueante
	printf("Escuchando...\n");

	struct sockaddr_in datosConexionCliente;// Esta estructura contendra los datos de la conexion del cliente. IP, puerto, etc.
	socklen_t datosConexionClienteSize = sizeof(datosConexionCliente);
	int socketCliente = accept(listenningSocket, (struct sockaddr *) &datosConexionCliente, &datosConexionClienteSize);
	t_Paquete paquete;
	int status = 1;
	printf("Kernel conectado. Esperando envío de mensajes.\n");
	while (status != 0) {
		int codOp;
		status = recv(socketCliente, &codOp, 4, 0);
		if (status != 0) {
			switch (codOp) {
				case 1:
					recibirYDeserealizarINSERT(socketCliente);
					break;
				default:
					break;
			}
		}
	}
	printf("Cliente Desconectado.\n");
	close(socketCliente);
}


void recibirYDeserealizarINSERT(int socketCliente) {
	int key;
	recv(socketCliente, &key, 4, 0);

	int sizeValue;
	recv(socketCliente, &sizeValue, 4, 0);
	char* value=malloc(sizeValue);
	recv(socketCliente, value, sizeValue, 0);

	int sizeNombreTabla;
	recv(socketCliente, &sizeNombreTabla, 4, 0);
	char* nombreTabla=malloc(sizeNombreTabla);
	recv(socketCliente, nombreTabla, sizeNombreTabla, 0);

	fprintf("Se recibio un INSERT %d , %s ,%s",key,value,nombreTabla);
	INSERT_MEMORIA(nombreTabla, key, value);

}




