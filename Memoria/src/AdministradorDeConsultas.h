/*
 * AdministradorDeConsultas.h
 *
 *  Created on: 11 may. 2019
 *      Author: utnso
 */
#ifndef ADMINISTRADORCONSULTAS_H_
#define ADMINISTRADORCONSULTAS_H_

#include "MemoriaPrincipal.h"


t_registro* SELECT_MEMORIA(char* nombreTabla, int key);
t_registro* INSERT_MEMORIA(char* nombreTabla, int key, char* value);
int CREATE_MEMORIA(char* nombreTabla, char* consitencia, int cantParticiones, int tiempoCompactacion);
void DESCRIBE_MEMORIA(char* nombreTabla);
void DROP_MEMORIA(char* nombreTabla);
void* JORUNAL();

#endif /*ADMINISTRADORCONSULTAS_H_*/
