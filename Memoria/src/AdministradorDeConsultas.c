/*
 * AdministradorDeConsultas.c
 *
 *  Created on: 11 may. 2019
 *      Author: utnso
 */

#include "AdministradorDeConsultas.h"

t_registro* SELECT_MEMORIA(char* nombreTabla, int key) {
	log_info(logger, "SELECT en memoria..");
	//si aca el segmento no existe en ningun lado, lo tengo que crear?
	usleep(configuracion->RETARDO_MEMORIA*1000);
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
	log_info(logger, "Insertando en memoria");
	usleep(configuracion->RETARDO_MEMORIA*1000);
	Segmento *tabla = buscarSegmentoEnMemoria(nombreTabla);
	if (tabla == NULL) {
		log_info(logger, "La tabla se cargara en la lista de segmentos");
		tabla = insertarSegmentoEnMemoria(nombreTabla);
	}

	if (validarValueMaximo(value)) {
		Pagina* pagina = insertarPaginaEnMemoria(key, value, timeStamp, tabla);
		printf("Se ha insertado el siguiente registro: %d \"%s\" en la tabla %s \n", key, value, nombreTabla);
		return pagina->registro;
	}
	return NULL;
}

int DROP_MEMORIA(char* nombreTabla) {
	usleep(configuracion->RETARDO_MEMORIA*1000);
	Segmento* segmentoEnMemoria = buscarSegmentoEnMemoria(nombreTabla);
	if (segmentoEnMemoria != NULL) {
		eliminarSegmentoDeMemoria(segmentoEnMemoria);
	}
	return eliminarSegmentoFileSystem(nombreTabla);

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
	free(metaData);
	if (succes == 0) {
		usleep(configuracion->RETARDO_MEMORIA*1000);
		insertarSegmentoEnMemoria(nombreTabla);
		printf("Se ha creado la tabla %s \n \n", nombreTabla);

		return 0;
	}
	printf("Hubo un error al crear la tabla %s, la tabla ya existe \n \n", nombreTabla);

	return 1;
}

t_metadata_tabla* DESCRIBE_MEMORIA(char* nombreTabla) {

	void mostrarMetadata(char* nombreSegmento, t_metadata_tabla* metadata) {
		printf("Segmento: %s \n", nombreSegmento);
		printf("Consistencia: %s / cantParticiones: %d / tiempoCompactacion: %d \n",
				getConsistenciaCharByEnum(metadata->CONSISTENCIA),
				metadata->CANT_PARTICIONES, metadata->T_COMPACTACION);
	}

	t_metadata_tabla* metadata = describeSegmento(nombreTabla);
	if (metadata == NULL) {
		printf("La tabla: %s no se encuentra en sistema", nombreTabla);
		return NULL;
	} else {
		mostrarMetadata(nombreTabla, metadata);
		return metadata;
	}
}

t_list* DESCRIBE_ALL_MEMORIA() {
	void mostrarMetadataSerializada(char* tablaSerializada) {
		char* tablaSerializadaAux = malloc(strlen(tablaSerializada) + 1);
		strcpy(tablaSerializadaAux, tablaSerializada);
		char** valores = string_split(tablaSerializada, " ");

		printf("Segmento: %s \n", valores[0]);
		printf("Consistencia: %s / cantParticiones: %s / tiempoCompactacion: %s \n", valores[1], valores[2],
				valores[3]);
		freePunteroAPunteros(valores);
		free(tablaSerializadaAux);
	}
	t_list* tablas = describeAllFileSystem();
	list_iterate(tablas, (void*) mostrarMetadataSerializada);
	return tablas;

}

void JOURNAL_MEMORIA(){

	journalMemoria();

}
