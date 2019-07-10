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
#include "SocketClienteMemoria.h"
#include "Memoria.h"

int socketFileSystem;
void procesarAccion(int socketMemoria);
void escuchar(int listenningSocket);

void procesarRequestSELECT(char* request, int socketKernel);
void procesarRequestINSERT(char* request, int socketKernel);
void procesarRequestCREATE(char* request, int socketKernel);
void procesarRequestDESCRIBE(char* nombreTabla, int socketKernel);
void procesarRequestDESCRIBE_ALL(int socketKernel);
void enviarSuccess(int resultado, t_protocolo protocolo, int socketKernel);
void procesarRequestDROP(char* nombreTabla,int socketKernel);
void procesarGossiping(char* memoriaGossiping,int socketMemoria);
#endif /* SERVER_H_ */

