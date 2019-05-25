/*
 * AdministradorDeConsultas.c
 *
 *  Created on: 11 may. 2019
 *      Author: utnso
 */

#include "AdministradorDeConsultas.h"

t_registro* SELECT(char nombreTabla[20], int key) {
	Segmento* tabla = buscarSegmento(nombreTabla);
	if (tabla != NULL) {
		Pagina* pagina = buscarPagina(tabla, key);
		if (pagina != NULL) {
			return pagina->registro;
		}
	}
	return NULL;
}

t_registro* INSERT(char nombreTabla[20], int key, char value[112]) {
	Segmento *tabla = buscarSegmento(nombreTabla);
	if (tabla == NULL) {
		tabla = insertarSegmentoEnMemoria(nombreTabla, NULL);
	}
	Pagina* pagina = insertarPaginaEnMemoria(key, value, tabla);
	return pagina->registro;
}

void DROP(char nombreTabla[20]) {
	eliminarSegmentoDeMemoria(nombreTabla);
	//aca pegale a kevin y avisale
}

int CREATE(char nombreTabla[20], char* consitencia, int cantParticiones, int tiempoCompactacion) {
	t_metadata_tabla* metaData = malloc(sizeof(t_metadata_tabla));
	strcpy(metaData->CONSISTENCIA, consitencia);
	metaData->CANT_PARTICIONES = cantParticiones;
	metaData->T_COMPACTACION = tiempoCompactacion;

	if (buscarSegmento(nombreTabla) != NULL) {
		return 1; //es un error, la tabla ya existe
	}
	insertarSegmentoEnMemoria(nombreTabla, metaData);
	return 0;
}

t_metadata_tabla* DESCRIBRE(char nombreTabla[20]) {
	return buscarSegmento(nombreTabla)->metaData;
}

t_list* DESCRIBE() {
	//t_list* metadatas= pedile a kevin que te pase todas sus tablas
	return NULL;
}
