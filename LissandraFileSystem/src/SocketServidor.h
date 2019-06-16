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
#include "funcionesLFS.h"
#include "EstructurasFileSystem.h"

void escuchar(int listenningSocket);
void procesarAccion(int socketMemoria);
void configuracionNuevaMemoria(int socketMemoria, int valueMaximo);
void procesarDescribe(char* consulta,int socketMemoria);
void procesarINSERT(char* request,int socketMemoria);
void procesarCREATE(char* request,int socketMemoria);
void procesarDESCRIBE(char* nombreTabla, int socketMemoria);
void procesarDROP(char* nombreTabla, int socketMemoria);
void enviarSuccess(int resultado, t_protocolo protocolo,int socketMemoria);
#endif /* SOCKETSERVIDORFILESYSTEM_H_ */
