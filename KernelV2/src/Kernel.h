/*
 * Kernel.h
 *
 *  Created on: 20 jun. 2019
 *      Author: utnso
 */

#ifndef KERNEL_H_
#define KERNEL_H_
#include "KernelHeader.h" // Guarda las funciones principales
#include "KernelOtherFunctions.h" // Guarda las funciones secundarias

pthread_t hiloConsola;
pthread_t hiloMetrics;
pthread_t hiloGossiping;
pthread_t hiloMetadataRefresh;
pthread_t* arrayDeHilos;
pthread_t* arrayDeHilosPuntero;
int tiempoGossiping;

void cerrarKernel();
void terminarHilos();
int conocerMemorias();

#endif /* KERNEL_H_ */
