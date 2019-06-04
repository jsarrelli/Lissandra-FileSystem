/*
 * ConfigKernel.h
 *
 *  Created on: 3 jun. 2019
 *      Author: utnso
 */

#ifndef CONFIGKERNEL_H_
#define CONFIGKERNEL_H_

#include "kernel.h"

t_config_kernel *cargarConfig(char *ruta);
t_dictionary *describeGlobal(char* IP_MEMORIA);
t_dictionary *conocerPoolMemorias(char* IP_MEMORIA);
int obtenerMemSegunConsistencia(char *consistencia, int key);
char * getConsistencia(char *nombreTabla);
t_criterios * inicializarCriterios();
int obtenerMemDestino(char *tabla, int key);
void add(int numeroMem, char *criterio);

#endif /* CONFIGKERNEL_H_ */
