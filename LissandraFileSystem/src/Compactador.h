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
void agregarRegistrosDeTmpc(char* rutaTmpc, t_list* listaRegistros);
void buscarBinarios(char** archivos, t_list* archivosBin);
int escribirEnBin(t_list* lista, char*rutaBinario);
void persistirParticionesDeTabla(t_list* listaListas, t_list*archivosBin);

#endif /* COMPACTADOR_H_ */
