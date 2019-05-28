#include "SocketClienteMemoria.h"

void INSERT(int key, char* value, char* nombreTabla) {

	t_Paquete_SELECT paquete;
	paquete.codOp = SELECT;
	paquete.key = key;
	paquete.sizeValue = strlen(value) ; //sumo 1 por el caracter /0
	paquete.value=malloc(paquete.sizeValue);
	strcpy(paquete.value,value);
	paquete.sizeNombreTabla = strlen(nombreTabla);
	paquete.nombreTabla=malloc(paquete.sizeNombreTabla);
	strcpy(paquete.nombreTabla, nombreTabla);

	send(serverSocket, &paquete, sizeof(paquete), 0);

	free(paquete.nombreTabla);
	free(paquete.value);
	puts("Insert enviado a memoria");

}

int configurarSocketCliente() {
	struct addrinfo hints;
	struct addrinfo *serverInfo;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;		// Permite que la maquina se encargue de verificar si usamos IPv4 o IPv6
	hints.ai_socktype = SOCK_STREAM;	// Indica que usaremos el protocolo TCP

	getaddrinfo(IP, PUERTO, &hints, &serverInfo);	// Carga en serverInfo los datos de la conexion

	serverSocket = socket(serverInfo->ai_family, serverInfo->ai_socktype, serverInfo->ai_protocol);
	if (connect(serverSocket, serverInfo->ai_addr, serverInfo->ai_addrlen) != 0) {
		perror("No se pudo conectar");
		return 0;
	}
	freeaddrinfo(serverInfo);	// No lo necesitamos mas
	printf("Conectado al servidor.\n");
	return 1;
}
