/*
 * AdministradorDeConsultas.c
 *
 *  Created on: 11 may. 2019
 *      Author: utnso
 */

#include "AdministradorDeConsultas.h"

t_registro* SELECT(char* nombreTabla, int key){
	Segmento* tabla=buscarSegmento(nombreTabla);
	Pagina* pagina=buscarPagina(tabla, key);
	if(pagina!=NULL){
		return pagina->registro;
	}
	return NULL;

}
