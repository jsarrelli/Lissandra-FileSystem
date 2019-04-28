#include "SocketCliente.h"



void enviarConsulta(){

	char* consulta=malloc(MAXCONSULTA);
	*consulta="hola";

	int enviar = 1;
	t_Paquete paquete;
	paquete.mensaje = malloc(MAX_MENSAJE_SIZE);
	char *serializedPackage;

		send(serverSocket,"hola \n",4,0);

	printf("Desconectado.\n");

}

int configurarSocketCliente(){
	struct addrinfo hints;
	struct addrinfo *serverInfo;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;		// Permite que la maquina se encargue de verificar si usamos IPv4 o IPv6
	hints.ai_socktype = SOCK_STREAM;	// Indica que usaremos el protocolo TCP

	getaddrinfo(IP, PUERTO, &hints, &serverInfo);	// Carga en serverInfo los datos de la conexion

	serverSocket = socket(serverInfo->ai_family, serverInfo->ai_socktype, serverInfo->ai_protocol);
	if(connect(serverSocket, serverInfo->ai_addr, serverInfo->ai_addrlen)!=0){
		perror("No se pudo conectar");
		return 0;
	}
	freeaddrinfo(serverInfo);	// No lo necesitamos mas
	printf("Conectado al servidor.\n");
	return 1;
}
