#include "AdministradorConsultasLFS.h"

int funcionCREATE(char* nombreTabla, char* cantParticiones, char* consistenciaChar, char* tiempoCompactacion) {
	if (existeTabla(nombreTabla)) {
		puts("Ya existe una tabla con ese nombre.");
		return 1;
	}
	crearTablaYParticiones(nombreTabla, cantParticiones);
	crearMetadataTabla(nombreTabla, consistenciaChar, cantParticiones, tiempoCompactacion);
	return 0;
}

int funcionDROP(char* nombreTabla) {
	if (existeTabla(nombreTabla)) {
		removerTabla(nombreTabla);
		printf("%s eliminada\n\n", nombreTabla);
		return 0;
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
}

int funcionINSERT(double timeStamp, char* nombreTabla, char* key, char* value) {
	if (existeTabla(nombreTabla)) {
		insertarKey(nombreTabla, key, value, timeStamp);
		log_info(logger, "Insert realizado en memtable");
		return 0;
	} else {
		printf("La %s no existe", nombreTabla);
		return 1;
	}
}
