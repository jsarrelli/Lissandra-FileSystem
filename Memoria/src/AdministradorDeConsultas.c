/*
 * AdministradorDeConsultas.c
 *
 *  Created on: 11 may. 2019
 *      Author: utnso
 */

#include "AdministradorDeConsultas.h"

t_registro* SELECT_MEMORIA(char* nombreTabla, int key) {
	//si aca el segmento no existe en ningun lado, lo tengo que crear?
	Segmento* tabla = buscarSegmento(nombreTabla);
	if (tabla != NULL) {
		Pagina* pagina = buscarPagina(tabla, key);
		if (pagina != NULL) {
			return pagina->registro;
		}
	}
	return NULL;
}

t_registro* INSERT_MEMORIA(char* nombreTabla, int key, char* value, double timeStamp) {
	Segmento *tabla = buscarSegmentoEnMemoria(nombreTabla);
	if (tabla == NULL) {
		tabla = insertarSegmentoEnMemoria(nombreTabla, NULL);
	}
	Pagina* pagina = insertarPaginaEnMemoria(key, value, timeStamp, tabla);
	return pagina->registro;
}

void DROP_MEMORIA(char* nombreTabla) {
	Segmento* segmento = buscarSegmento(nombreTabla);
	if (segmento != NULL) {
		eliminarSegmentoDeMemoria(segmento);
	}
	eliminarSegmentoFileSystem(nombreTabla);

}

t_metadata_tabla* newMetadata(t_consistencia consistencia, int cantParticiones, int tiempoCompactacion)
{
	t_metadata_tabla* metaData = malloc(sizeof(t_metadata_tabla));
	metaData->CONSISTENCIA = consistencia;
	metaData->CANT_PARTICIONES = cantParticiones;
	metaData->T_COMPACTACION = tiempoCompactacion;
	return metaData;
}


//crea una tabla en memoria y le avisa al fileSystem
int CREATE_MEMORIA(char* nombreTabla, t_consistencia consistencia, int cantParticiones, int tiempoCompactacion) {
	t_metadata_tabla* metaData = newMetadata(consistencia, cantParticiones, tiempoCompactacion);
	int succes = enviarCreateAFileSystem(metaData, nombreTabla);
	if(succes==0){
		insertarSegmentoEnMemoria(nombreTabla, metaData);
		printf("Se ha creado la tabla %s", nombreTabla);

		return 0;
	}
	printf("Hubo un error al crear la tabla %s, la tabla ya existe", nombreTabla);

	return 1;
}

t_metadata_tabla* DESCRIBE_MEMORIA(char* nombreTabla) {
	Segmento* segmento= buscarSegmento(nombreTabla);
	if(segmento==NULL){
		return NULL;
	}else{
		return segmento->metaData;
	}
}

t_list* DESCRIBE_ALL_MEMOROIA() {
	//t_list* metadatas= pedile a kevin que te pase todas sus tablas
	return NULL;
}
