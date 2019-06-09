#ifndef SOCKETSERVIDORFILESYSTEM_H_
#define SOCKETSERVIDORFILESYSTEM_H

#include "Libraries.h"
#include <Sockets/Conexiones.h>
#include <Sockets/Serializacion.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <stdint.h>


int socketMemoria;

void escuchar(int listenningSocket);
void procesarAccion(int socketMemoria);
void procesarDescribe(char* consulta);
#endif /* SOCKETSERVIDORFILESYSTEM_H_ */
