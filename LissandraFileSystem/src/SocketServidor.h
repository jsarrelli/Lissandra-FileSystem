#ifndef SOCKETSERVIDORFILESYSTEM_H_
#define SOCKETSERVIDORFILESYSTEM_H

#include "Libraries.h"
#include "Funciones/Conexiones.h"
#include "Funciones/Serializacion.h"


int socketMemoria;

void escuchar(int listenningSocket);
void procesarAccion(int socketMemoria);

#endif /* SOCKETSERVIDORFILESYSTEM_H_ */
