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
void hardcodearInfoMemorias();
void imprimirCriterio(infoMemoria* infoMemoria);
bool instruccionSeaSalir(char* operacion);
int contarLineasArchivo(FILE* fichero, char* path);
void destruirArraySemaforos();
int instruccionSeaMetrics(char* operacion);
void crearMetrics(t_metrics* metrica);
void destruirMetrics(t_metrics* metrica);
void reiniciarMetrics(t_metrics* metrica);
void calcularMetrics();
void copiarMetrics();
void imprimirMetrics(t_metrics metrica);

void freeInfoMemoria(infoMemoria* memoria);
void filtrarMemorias();
void listarMemorias();

#endif /* KERNELOTHERFUNCTIONS_H_ */
