/*
 * APIKernel.h
 *
 *  Created on: 20 jun. 2019
 *      Author: utnso
 */

#ifndef APIKERNEL_H_
#define APIKERNEL_H_

#include "Kernel.h"

/*
 * Aca se almacenan las funciones de inicio del API Kernel
 */

void procesarInput(char* linea);
void procesarAdd(int id, consistencia cons);
void consolaAdd(char*argumento);
void consolaInsert(char*request);
void consolaSelect(char*argumentos);
void consolaCreate(char*argumentos);
void consolaDescribe(char*nombreTabla);
void consolaDrop(char*nombreTabla);
void consolaRun(char*path);

void consolaExecute();

#endif /* APIKERNEL_H_ */
