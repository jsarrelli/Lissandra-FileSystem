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
#include <stdio.h>

void compactarTabla(char*nombreTabla);

int escribirEnBin(t_list* lista, char*rutaBinario);
void persistirParticionesDeTabla(t_list* listaListas, t_list*archivosBin);

t_list* buscarTemporalesByNombreTabla(char* nombreTabla);
t_list* buscarBinariosByNombreTabla(char* nombreTabla);
t_list* buscarTmpcsByNombreTabla(char* nombreTabla);

void agregarRegistrosFromBloqueByPath(char* rutaTmpc, t_list* listaRegistros);
t_list* obtenerRegistrosFromBloque(char* rutaArchivoBloque);
t_list* obtenerRegistrosFromBinByNombreTabla(char* nombreTabla);
t_list* obtenerRegistrosFromTempByNombreTabla(char* nombreTabla);

void filtrarRegistros(t_list* registros);
void mergearRegistrosNuevosConViejos(t_list* archivosBinarios, t_list* particionesRegistrosNuevos);
t_list* cargarRegistrosNuevosEnEstructuraParticiones(int cantParticiones, t_list* registrosNuevos);

char* registroToChar(t_registro* registro);
void iniciarThreadCompactacion(t_tabla_memtable* tabla);
t_list* cambiarExtensionTemporales(t_list* listaTemporalesTmp);
#endif /* COMPACTADOR_H_ */
