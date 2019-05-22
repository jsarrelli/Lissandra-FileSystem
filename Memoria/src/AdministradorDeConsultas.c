/*
 * AdministradorDeConsultas.c
 *
 *  Created on: 11 may. 2019
 *      Author: utnso
 */

#include "AdministradorDeConsultas.h"

t_registro* SELECT(char nombreTabla[20], int key) {
	Segmento* tabla = buscarSegmentoEnMemoria(nombreTabla);
	if (tabla != NULL) {
		Pagina* pagina = buscarPagina(tabla, key);
		if (pagina != NULL) {
			return pagina->registro;
		}
	}
	return NULL;
}

t_registro* INSERT(char nombreTabla[20], int key, char value[112]) {
	Segmento *tabla = buscarSegmentoEnMemoria(nombreTabla);
	if(tabla==NULL){
		tabla=insertarSegmentoEnMemoria(nombreTabla,NULL);
	}
	Pagina* pagina = insertarPaginaEnMemoria(key, value, tabla);
	return pagina->registro;
}

void DROP(char nombreTabla[20]) {

}
