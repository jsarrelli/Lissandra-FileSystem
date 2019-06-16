/*
 * AdministradorDeConsultas.h
 *
 *  Created on: 11 may. 2019
 *      Author: utnso
 */
#ifndef ADMINISTRADORCONSULTAS_H_
#define ADMINISTRADORCONSULTAS_H_

#include "MemoriaPrincipal.h"
#include "EstructurasMemoria.h"

t_registro* SELECT_MEMORIA(char* nombreTabla, int key);
t_registro* INSERT_MEMORIA(char* nombreTabla, int key, char* value,double timeStamp);
int CREATE_MEMORIA(char* nombreTabla, t_consistencia consitencia, int cantParticiones, int tiempoCompactacion);
Segmento* DESCRIBE_MEMORIA(char* nombreTabla);
t_list* DESCRIBE_ALL_MEMORIA();
void DROP_MEMORIA(char* nombreTabla);
void* JORUNAL();

#endif /*ADMINISTRADORCONSULTAS_H_*/
