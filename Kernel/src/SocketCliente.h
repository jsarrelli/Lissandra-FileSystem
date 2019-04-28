#ifndef CLIENTE_H_
#define CLIENTE_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <stdint.h>

#define IP "127.0.0.1"
#define PUERTO "6667"
#define MAXCONSULTA 6
#define MAX_MENSAJE_SIZE 100

int serverSocket;

typedef struct t_Paquete {
	char* consulta;
	uint32_t consulta_long;
	char* mensaje;
	uint32_t mensaje_long;
} t_Paquete;


char* serializarOperandos(t_Paquete*);
void get_Username(char**);
void fill_package(t_Paquete*, char**);
void dispose_package(char**);
int configurarSocketCliente();
int enviarCosulta();

#endif /* CLIENTE_H_ */
