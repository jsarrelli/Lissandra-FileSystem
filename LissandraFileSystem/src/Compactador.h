/*
 * Compactador.h
 *
 *  Created on: 23 abr. 2019
 *      Author: utnso
 */

#ifndef COMPACTADOR_H_
#define COMPACTADOR_H_
#include "EstructurasFileSystem.h"
#include "FileSystem.h"
#include "funcionesLFS.h"
#include "ApiLFS.h"
#include "AdministradorConsultasLFS.h"

void compactarTabla(char*nombreTabla);
void cambiarExtensionTemporales(char** archivos, t_list* listaTmpc);
int cambiarExtension(char* rutaVieja, char* extensionNueva, t_list* listaTmpc);


#endif /* COMPACTADOR_H_ */
