#ifndef SERVER_H_
#define SERVER_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <stdint.h>


#define PUERTO "6667"
#define BACKLOG 5			// Define cuantas conexiones vamos a mantener pendientes al mismo tiempo
#define MAX_PACKAGE_SIZE 1024	//El servidor no admitira paquetes de mas de 1024 bytes
#define MAXCONSULTA 6
#define MAX_MENSAJE_SIZE 100
int listenningSocket;

typedef struct _t_Paquete {
	char consulta[MAXCONSULTA];
	uint32_t consulta_long;
	char mensaje[MAX_MENSAJE_SIZE];
	uint32_t mensaje_long;
} t_Paquete;


int configurarSocketServidor();
void escuchar();
void recibirYDeserealizarINSERT(socketCliente);

#endif /* SERVER_H_ */
