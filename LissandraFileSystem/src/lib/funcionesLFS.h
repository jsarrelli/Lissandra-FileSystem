/*
 * funcionesLFS.h
 *
 *  Created on: 23 abr. 2019
 *      Author: utnso
 */

#ifndef FUNCIONESLFS_H_
#define FUNCIONESLFS_H_

#include "Libraries.h"

t_list* listaNombresTablas;

//consola

void procesarInput(char * linea);
void consolaLFS(void);
void consolaDrop(char** palabras, int cantidadParametros);
void consolaInsert(char**palabras, int cantidadParametros);
void consolaSelect(char**palabras, int cantidadParametros);
void consolaCreate(char**palabras, int cantidadParametros);
void consolaDescribe(char**palabras, int cantidadParametros);

//tablas

int existeTabla(char* nombreTabla);
char * obtenerRutaTablaSinArchivo(char * rutaTabla);
void crearTablaYParticiones(char* nombreTabla, char* cantidadParticiones);


#endif /* FUNCIONESLFS_H_ */
