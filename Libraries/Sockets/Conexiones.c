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
		close(socketFD);
		return conexion;
	}
	//printf("Conexion establecida \n");
	return socketFD;

}

int ConectarAServidorPlus(int puerto, char* ip) {
	int socket;
	while (true) {

		socket = ConectarAServidor(puerto, ip);
		if (socket != -1) {
			break;
		}
		usleep(10);
	}

	return socket;
}

int configurarSocketServidor(char* puertoEscucha) {

	struct sockaddr_in direccionServidor;
	direccionServidor.sin_family = AF_INET;
	direccionServidor.sin_addr.s_addr = INADDR_ANY;
	direccionServidor.sin_port = htons(atoi(puertoEscucha));

	int servidor = socket(AF_INET, SOCK_STREAM, 0);

	int activado = 1;
	setsockopt(servidor, SOL_SOCKET, SO_REUSEADDR, &activado, sizeof(activado));

	if (bind(servidor, (void*) &direccionServidor, sizeof(direccionServidor)) != 0) {
		perror("Fallo el bind");
		return 0;
	}

	return servidor;
}
