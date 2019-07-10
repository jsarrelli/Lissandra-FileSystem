#include "AdministradorConsultasLFS.h"

int funcionCREATE(char* nombreTabla, char* cantParticiones, char* consistenciaChar, char* tiempoCompactacion) {
	if (existeTabla(nombreTabla)) {
		puts("Ya existe una tabla con ese nombre.");
		return 1;
	}
	crearTablaYParticiones(nombreTabla, cantParticiones);
	crearMetadataTabla(nombreTabla, consistenciaChar, cantParticiones, tiempoCompactacion);
	//iniciarThreadCompactacion(nombreTabla);
	return 0;
}

int funcionDROP(char* nombreTabla) {
	if (existeTabla(nombreTabla)) {
		removerTabla(nombreTabla);
		printf("%s eliminada\n\n", nombreTabla);
		return 0;
	} else {
		puts("La tabla que se quiere eliminar no existe");
	}
	return 1;
}

t_metadata_tabla funcionDESCRIBE(char* nombreTabla) {
	t_metadata_tabla metadata = obtenerMetadata(nombreTabla);
	mostrarMetadataTabla(metadata, nombreTabla);
	return metadata;
}

void funcionDESCRIBE_ALL() {
	mostrarMetadataTodasTablas(rutas.Tablas);
	crearYEscribirArchivosTemporales(rutas.Tablas);
	compactarTabla("TABLA");
}



int funcionINSERT(double timeStamp, char* nombreTabla, char* key, char* value) {
	if (config->TAMANIO_VALUE < strlen(value)) {
		log_error(loggerError, "Tamanio maximo de value excedido");
		return 1;
	}
	if (existeTabla(nombreTabla)) {
		insertarKey(nombreTabla, key, value, timeStamp);
		log_info(logger, "Insert realizado en memtable");
		return 0;
	} else {
		printf("La %s no existe", nombreTabla);
		return 1;
	}
}


void funcionSELECT(char*nombreTabla, int keyActual){
	if(existeTabla(nombreTabla)){
	t_list* listaRegistros=list_create();
	getRegistrosByKeyFromNombreTabla(nombreTabla, keyActual, listaRegistros);
	char* registroFinal = buscarRegistroByKeyFromListaRegistros(listaRegistros,keyActual);
		if(registroFinal!=NULL){
			printf("%s\n\n", registroFinal);
			list_destroy(listaRegistros);
		}else{
			puts("No hay registros para mostrar");
		}
	} else{
		puts("La tabla sobre la que se quiere hacer SELECT no existe en LFS\n");
	}
}

void procesoDump(){
	pthread_t threadDump;
	while (1) {
		usleep(config->TIEMPO_DUMP * 1000);
		pthread_create(&threadDump, NULL, (void*) crearYEscribirArchivosTemporales, rutas.Tablas);
		pthread_detach(threadDump);
	}
}
