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
void cambiarExtensionTemporales(t_list* listaTemporalesTmp);
t_list* buscarTemporalesByNombreTabla(char* nombreTabla);
void agregarRegistrosDeTmpc(char* rutaTmpc, t_list* listaRegistros);
t_list* buscarBinariosByTabla(char* nombreTabla);
int escribirEnBin(t_list* lista, char*rutaBinario);
void persistirParticionesDeTabla(t_list* listaListas, t_list*archivosBin);
t_list* obtenerRegistrosFromBloque(char* rutaArchivoBloque);

#endif /* COMPACTADOR_H_ */
