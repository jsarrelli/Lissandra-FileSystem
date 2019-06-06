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
#define PUERTO "35615"
#define MAXCONSULTA 6
#define MAX_MENSAJE_SIZE 100

#include "Funciones/Conexiones.h"
#include "Funciones/Serializacion.h"


int serverSocket;

typedef struct t_Paquete_SELECT {
	int codOp;
	int key;
	int sizeValue;
	char* value;
	int sizeNombreTabla;
	char* nombreTabla;
} t_Paquete_SELECT;

#endif /* CLIENTE_H_ */
