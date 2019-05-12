/*
 * AdministradorDeConsultas.h
 *
 *  Created on: 11 may. 2019
 *      Author: utnso
 */

#ifndef ADMINISTRADORCONSULTAS_H_
#define ADMINISTRADORCONSULTAS_H_
#include "MemoriaPrincipal.h"


t_registro* SELECT(char* nombreTabla, int key);
int insert(char* nombreTabla, int key, char* value);
int create(char* nombreTabla,char* tipoConsistencia, int numeroParticiones, float tiempoCompactacion);
void describe(char* nombreTabla);//aca va a tener que devolver algo, proximamente...
int drop(char* nombreTabla);
void* journal();

#endif /*ADMINISTRADORCONSULTAS_H_*/
