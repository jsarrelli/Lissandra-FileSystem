/*
 * AdministradorDeConsultas.c
 *
 *  Created on: 11 may. 2019
 *      Author: utnso
 */

#include "AdministradorDeConsultas.h"

t_registro* SELECT_MEMORIA(char* nombreTabla, int key) {
	Segmento* tabla = buscarSegmento(nombreTabla);
	if (tabla != NULL) {
		Pagina* pagina = buscarPagina(tabla, key);
		if (pagina != NULL) {
			return pagina->registro;
		}
	}
	return NULL;
}

t_registro* INSERT_MEMORIA(char* nombreTabla, int key, char* value,double timeStamp) {
	Segmento *tabla = buscarSegmento(nombreTabla);
	if (tabla == NULL) {
		tabla = insertarSegmentoEnMemoria(nombreTabla, NULL);
	}
	Pagina* pagina = insertarPaginaEnMemoria(key, value, tabla,timeStamp);
	return pagina->registro;
}

void DROP_MEMORIA(char* nombreTabla) {
	eliminarSegmentoDeMemoria(nombreTabla);
	//aca pegale a kevin y avisale
}

int CREATE_MEMORIA(char* nombreTabla, char* consistencia, int cantParticiones, int tiempoCompactacion) {
	t_metadata_tabla* metaData = malloc(sizeof(t_metadata_tabla));
	strcpy(metaData->CONSISTENCIA, consistencia);
	metaData->CANT_PARTICIONES = cantParticiones;
	metaData->T_COMPACTACION = tiempoCompactacion;

	if (buscarSegmento(nombreTabla) != NULL) {
		return 1; //es un error, la tabla ya existe
	}
	insertarSegmentoEnMemoria(nombreTabla, metaData);
	return 0;
}

t_metadata_tabla* DESCRIBE_MEMORIA(char* nombreTabla) {
	return buscarSegmento(nombreTabla)->metaData;
}

t_list* DESCRIBE_ALL_MEMOROIA() {
	//t_list* metadatas= pedile a kevin que te pase todas sus tablas
	return NULL;
}
