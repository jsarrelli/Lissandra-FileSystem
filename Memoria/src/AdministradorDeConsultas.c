/*
 * AdministradorDeConsultas.c
 *
 *  Created on: 11 may. 2019
 *      Author: utnso
 */

#include "AdministradorDeConsultas.h"

t_registro_memoria* SELECT_MEMORIA(char* nombreTabla, int key) {
	log_info(loggerInfo, "SELECT en memoria..");
	usleep(configuracion->RETARDO_MEMORIA * 1000);
	Segmento* tabla = buscarSegmento(nombreTabla);

	Pagina* pagina = buscarPagina(tabla, key);
	if (pagina != NULL) {
		log_trace(loggerTrace, "Registro: %d / %s / %f", pagina->registro->key, pagina->registro->value, pagina->registro->timestamp);
		return pagina->registro;
	}

	log_trace(loggerTrace, "Registro no encontrado");
	return NULL;
}

t_registro_memoria* INSERT_MEMORIA(char* nombreTabla, int key, char* value, double timeStamp) {
	log_info(loggerInfo, "Insertando en memoria");
	usleep(configuracion->RETARDO_MEMORIA * 1000);

	Segmento *tabla = buscarSegmento(nombreTabla);

	if (validarValueMaximo(value)) {
		Pagina* pagina = insertarPaginaEnMemoria(key, value, timeStamp, tabla, true);
		log_trace(loggerTrace, "Se ha insertado el siguiente registro: %d \"%s\" en la tabla %s \n", key, value, nombreTabla);
		return pagina->registro;
	}
	return NULL;
}

int DROP_MEMORIA(char* nombreTabla) {
	usleep(configuracion->RETARDO_MEMORIA * 1000);
	Segmento* segmentoEnMemoria = buscarSegmentoEnMemoria(nombreTabla);
	if (segmentoEnMemoria != NULL) {
		eliminarSegmentoDeMemoria(segmentoEnMemoria);
	}
	int success = eliminarSegmentoFileSystem(nombreTabla);

	if (success == 0) {
		log_trace(loggerTrace, "Se ha eliminado la tabla %s", nombreTabla);
	} else {
		log_error(loggerError, "No se ha podido eliminar la tabla %s", nombreTabla);
	}

	return success;

}

t_metadata_tabla* newMetadata(t_consistencia consistencia, int cantParticiones, int tiempoCompactacion) {
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
		usleep(configuracion->RETARDO_MEMORIA * 1000);
		insertarSegmentoEnMemoria(nombreTabla);
		log_trace(loggerTrace, "Se ha creado la tabla %s", nombreTabla);

	} else {
		log_error(loggerError, "Hubo un error al crear la tabla %s, la tabla ya existe", nombreTabla);
	}

	return succes;
}

t_metadata_tabla* DESCRIBE_MEMORIA(char* nombreTabla) {

	void mostrarMetadata(t_metadata_tabla* metadata) {

		log_trace(loggerTrace, "Segmento: %s \n Consistencia: %s / cantParticiones: %d / tiempoCompactacion: %d ", nombreTabla,
				getConsistenciaCharByEnum(metadata->CONSISTENCIA), metadata->CANT_PARTICIONES, metadata->T_COMPACTACION);
	}

	t_metadata_tabla* metadata = describeSegmento(nombreTabla);
	if (metadata == NULL) {
		log_error(loggerError, "La tabla: %s no Sse encuentra en sistema", nombreTabla);
		return NULL;
	} else {
		mostrarMetadata(metadata);
		return metadata;
	}
}

char* DESCRIBE_ALL_MEMORIA() {
	void mostrarMetadataSerializada(char* tablaSerializada) {
		char* tablaSerializadaAux = string_duplicate(tablaSerializada);
		char** valores = string_split(tablaSerializada, " ");

		log_trace(loggerTrace, "Segmento: %s \n Consistencia: %s / cantParticiones: %s / tiempoCompactacion: %s ", valores[0], valores[1],
				valores[2], valores[3]);

		freePunteroAPunteros(valores);
		free(tablaSerializadaAux);
	}

	char* tablasSerializadas = describeAllFileSystem();
	if (tablasSerializadas == NULL) {
		log_info(loggerInfo, "No hay tablas en el sistema");
	} else {
		char* tablasSerializadasAux = string_duplicate(tablasSerializadas);
		char** tablas = string_split(tablasSerializadasAux, "/");
		int i = 0;
		while (tablas[i] != NULL) {
			mostrarMetadataSerializada(tablas[i]);
			i++;
		}

		freePunteroAPunteros(tablas);
		free(tablasSerializadasAux);
	}

	return tablasSerializadas;

}

void JOURNAL_MEMORIA() {
	pthread_mutex_lock(&mutexInsert);
	pthread_mutex_lock(&mutexSelect);
	pthread_mutex_lock(&mutexJournal);

	journalMemoria();
	pthread_mutex_unlock(&mutexInsert);
	pthread_mutex_unlock(&mutexSelect);
	pthread_mutex_unlock(&mutexJournal);
}
