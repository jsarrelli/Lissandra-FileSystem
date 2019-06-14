#include "AdministradorConsultasLFS.h"

int funcionCREATE(char* nombreTabla, char* cantParticiones, char* consistenciaChar, char* tiempoCompactacion) {
	if (existeTabla(nombreTabla)) {
		puts("Ya existe una tabla con ese nombre.");
		return 0;
	}
	crearTablaYParticiones(nombreTabla, cantParticiones);
	crearMetadataTabla(nombreTabla, consistenciaChar, cantParticiones, tiempoCompactacion);
	return 1;
}

int funcionDROP(char* nombreTabla) {
	if (existeTabla(nombreTabla)) {
		removerTabla(nombreTabla);
		printf("%s eliminada\n\n", nombreTabla);
		return 1;
	}
	return 0;
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
		return 1;
	} else {
		printf("La %s no existe", nombreTabla);
		return 0;
	}
}
