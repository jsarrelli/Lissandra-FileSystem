#include "../Sockets/Conexiones.h"

int ConectarAServidor(int puerto, char* ip) {
	int socketFD = socket(AF_INET, SOCK_STREAM, 0);

	struct sockaddr_in direccion;

	direccion.sin_family = AF_INET;
	direccion.sin_port = htons(puerto);
	direccion.sin_addr.s_addr = inet_addr(ip);
	memset(&(direccion.sin_zero), '\0', 8);
	int conexion;

	conexion = connect(socketFD, (struct sockaddr *) &direccion, sizeof(struct sockaddr));
	if (conexion == -1) {
		return conexion;
	}
	printf("Conexion establecida \n");
	return socketFD;

}

int ConectarAServidorPlus(int puerto, char* ip) {
	int socket;
	while (true) {

		socket = ConectarAServidor(puerto, ip);
		if (socket != -1) {
			break;
		}

		//printf("No se pudo conectar a %s / %d, reintentando..\n",ip,puerto);
		usleep(2000);
	}

	return socket;
}

int configurarSocketServidor(char* puertoEscucha) {

	struct addrinfo hints;
	struct addrinfo *serverInfo;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;		// No importa si uso IPv4 o IPv6
	hints.ai_flags = AI_PASSIVE;		// Asigna el address del localhost: 127.0.0.1
	hints.ai_socktype = SOCK_STREAM;	// Indica que usaremos el protocolo TCP
	getaddrinfo(NULL, puertoEscucha, &hints, &serverInfo); // Notar que le pasamos NULL como IP, ya que le indicamos que use localhost en AI_PASSIVE
	int listenningSocket = socket(serverInfo->ai_family, serverInfo->ai_socktype, serverInfo->ai_protocol);

	int activado = 1;
	setsockopt(listenningSocket, SOL_SOCKET, SO_REUSEADDR, &activado, sizeof(activado)); //Para que reuse el puerto si esta ocupado

	if (bind(listenningSocket, serverInfo->ai_addr, serverInfo->ai_addrlen) != 0) {
		perror("Fallo el bind");
		return 0;
	}
	freeaddrinfo(serverInfo);
	return listenningSocket;
}
