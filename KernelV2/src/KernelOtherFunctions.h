/*
 * KernelOtherFunctions.h
 *
 *  Created on: 20 jun. 2019
 *      Author: utnso
 */

#ifndef KERNELOTHERFUNCTIONS_H_
#define KERNELOTHERFUNCTIONS_H_

#include "KernelHeader.h"

/*
 * Aca se guardan las funciones que no son importantes, las que hacen cosas que no nos importan mucho
 *
 * Pero que son propias del Kernel
 */

consistencia procesarConsistencia(char* palabra);
bool verificarCriterio(bool* criterio, consistencia ccia);
void hardcodearInfoMemorias();
void imprimirCriterio(bool* criterio);
bool instruccionSeaSalir(char* operacion);
void hardcodearListaMetadataTabla();

#endif /* KERNELOTHERFUNCTIONS_H_ */