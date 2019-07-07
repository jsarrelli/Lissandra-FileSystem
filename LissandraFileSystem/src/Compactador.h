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
#include <BubbleSort.h>

void compactarTabla(char*nombreTabla);
void cambiarExtensionTemporales(t_list* listaTemporalesTmp);

int escribirEnBin(t_list* lista, char*rutaBinario);
void persistirParticionesDeTabla(t_list* listaListas, t_list*archivosBin);

t_list* buscarTemporalesByNombreTabla(char* nombreTabla);
t_list* buscarBinariosByNombreTabla(char* nombreTabla);

void agregarRegistrosFromBloqueByPath(char* rutaTmpc, t_list* listaRegistros);
t_list* obtenerRegistrosFromBloque(char* rutaArchivoBloque);
t_list* obtenerRegistrosFromBinByNombreTabla(char* nombreTabla);
t_list* obtenerRegistrosFromTempByNombreTabla(char* nombreTabla);

void filtrarRegistros(t_list* registros);
#endif /* COMPACTADOR_H_ */
