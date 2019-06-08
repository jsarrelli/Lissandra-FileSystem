#ifndef SERVER_H_
#define SERVER_H_

#include "AdministradorDeConsultas.h"
#include <Libraries.h>
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
#include "Parser.h"
int socketFileSystem;
void procesarAccion(int socketMemoria);
void escuchar(int listenningSocket, int socketFileSystemRecibido);
#endif /* SERVER_H_ */
