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

t_registro_memoria* SELECT_MEMORIA(char* nombreTabla, int key) ;
t_registro_memoria* INSERT_MEMORIA(char* nombreTabla, int key, char* value, double timeStamp);
int CREATE_MEMORIA(char* nombreTabla, t_consistencia consitencia, int cantParticiones, int tiempoCompactacion);
t_metadata_tabla* DESCRIBE_MEMORIA(char* nombreTabla);
char* DESCRIBE_ALL_MEMORIA();
int DROP_MEMORIA(char* nombreTabla);
void JOURNAL_MEMORIA();

#endif /*ADMINISTRADORCONSULTAS_H_*/
