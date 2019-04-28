#include "SocketServidor.h"





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

void escuchar(){
	listen(listenningSocket, BACKLOG); // es una syscall bloqueante
	printf("Escuchando...\n");

	struct sockaddr_in datosConexionCliente;			// Esta estructura contendra los datos de la conexion del cliente. IP, puerto, etc.
	socklen_t datosConexionClienteSize = sizeof(datosConexionCliente);
	int socketCliente = accept(listenningSocket, (struct sockaddr *) &datosConexionCliente, &datosConexionClienteSize);
	t_Paquete paquete;
	int status = 1;
	printf("Cliente conectado. Esperando Envío de mensajes.\n");
	while (status!=0){
		char cadena[6];
		status=recv(socketCliente,cadena,6,0);
		//status = recieve_and_deserialize(&paquete, socketCliente);	// Ver el "Deserializando estructuras dinamicas" en el comentario de la funcion.
		//if (status) printf("%s: %s", paquete.consulta, paquete.mensaje);
				if(status!=0)printf(cadena);
	}
	printf("Cliente Desconectado.\n");
	close(socketCliente);
}


int recieve_and_deserialize(t_Paquete *paquete, int socketCliente){

	int status;
	int buffer_size;
	char *buffer = malloc(buffer_size = sizeof(uint32_t));

	uint32_t consulta_long;
	status = recv(socketCliente, buffer, sizeof(paquete->consulta_long), 0);
	memcpy(&(consulta_long), buffer, buffer_size);
	if (!status) return 0;

	status = recv(socketCliente, paquete->consulta, consulta_long, 0);
	if (!status) return 0;

	uint32_t mensaje_long;
	status = recv(socketCliente, buffer, sizeof(paquete->mensaje_long), 0);
	memcpy(&(mensaje_long), buffer, buffer_size);
	if (!status) return 0;

	status = recv(socketCliente, paquete->mensaje, mensaje_long, 0);
	if (!status) return 0;


	free(buffer);

	return status;
}


