#include "SocketClienteFileSystem.h"

//int configurarSocketCliente(MEMORIA_configuracion* configuracion) {
//	struct addrinfo hints;
//	struct addrinfo *serverInfo;
//	memset(&hints, 0, sizeof(hints));
//	hints.ai_family = AF_UNSPEC;		// Permite que la maquina se encargue de verificar si usamos IPv4 o IPv6
//	hints.ai_socktype = SOCK_STREAM;	// Indica que usaremos el protocolo TCP
//
//	getaddrinfo(configuracion->IP_FS, configuracion->PUERTO_FS, &hints, &serverInfo);	// Carga en serverInfo los datos de la conexion
//
//	serverSocket = socket(serverInfo->ai_family, serverInfo->ai_socktype, serverInfo->ai_protocol);
//	if (connect(serverSocket, serverInfo->ai_addr, serverInfo->ai_addrlen) != 0) {
//		perror("No se pudo conectar");
//		return 0;
//	}
//	freeaddrinfo(serverInfo);	// No lo necesitamos mas
//	log_info(logger, "Conexion realizado con LFS");
//	return 1;
//}
