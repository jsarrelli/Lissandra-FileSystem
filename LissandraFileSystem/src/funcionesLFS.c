/*
 * funcionesLFS.c
 *
 *  Created on: 23 abr. 2019
 *      Author: utnso
 */

#include "funcionesLFS.h"

char * obtenerRutaTablaSinArchivo(char * rutaTabla) {
	char ** directorios;
	char * archivo = malloc(50);
	int tamanioNombreArchivo;
	int tamanioRuta;

	directorios = string_split(rutaTabla, "/");
	archivo = obtenerUltimoElementoDeUnSplit(directorios);
	tamanioNombreArchivo = strlen(archivo) + 1;
	tamanioRuta = strlen(rutaTabla);

	liberarPunteroDePunterosAChar(directorios);
	free(directorios);
	free(archivo);
	return string_substring_until(rutaTabla, tamanioRuta - tamanioNombreArchivo);
}

char * obtenerExtensionDeUnArchivo(char * nombreArchivoConExtension) {
	char* nombreArchivoConExtensionAux = string_duplicate(nombreArchivoConExtension);
	char ** palabras = string_split(nombreArchivoConExtensionAux, ".");
	char * extension = string_duplicate(palabras[1]);
	freePunteroAPunteros(palabras);
	free(nombreArchivoConExtensionAux);
	return extension;

}

char * obtenerExtensionDeArchivoDeUnaRuta(char * rutaLocal) {
	char * archivoConExtension = obtenerNombreDeArchivoDeUnaRuta(rutaLocal);
	char * extension = obtenerExtensionDeUnArchivo(archivoConExtension);
	free(archivoConExtension);
	return extension;
}

int existeTabla(char* nombreTabla) {
	char* rutaTabla = armarRutaTabla(nombreTabla);
	DIR* tablaActual;

	tablaActual = opendir(rutaTabla);

	if (tablaActual != NULL) {
		closedir(tablaActual);
		free(rutaTabla);
		return 1;
	}
	closedir(tablaActual);
	free(rutaTabla);
	return 0;
}

void insertarTablaEnMemtable(char* nombreTabla) {
	t_tabla_memtable* tabla = newTablaMemtable(nombreTabla);
	list_add(memtable, tabla);
	printf("%s insertada en memtable \n", tabla->tabla);
}

void crearTablaYParticiones(char* nombreTabla, char* cantidadParticiones) {

	char* rutaTabla = armarRutaTabla(nombreTabla);
	mkdir(rutaTabla, 0777);
	log_info(logger, "Se creo la tabla: %s en LFS\n", nombreTabla);

	int cantPart = atoi(cantidadParticiones);
	for (int i = 0; i < cantPart; i++) {
		char* rutaParticion = string_duplicate(rutaTabla);

		string_append(&rutaParticion, "/");
		string_append_with_format(&rutaParticion, "%d.bin", i);
		crearArchReservarBloqueYEscribirBitmap(rutaParticion);
		printf("Particion %d creada \n", i);
		free(rutaParticion);

	}

	insertarTablaEnMemtable(nombreTabla);
	free(rutaTabla);

}

t_tabla_memtable* newTablaMemtable(char* nombreTabla) {
	t_tabla_memtable* tabla = malloc(sizeof(t_tabla_memtable));
	tabla->tabla = string_duplicate(nombreTabla);
	tabla->registros = list_create();
	return tabla;
}

void crearMetadataTabla(char*nombreTabla, char* consistencia, char* cantidadParticiones, char* tiempoCompactacion) {
	char*rutaTabla = armarRutaTabla(nombreTabla);
	string_append(&rutaTabla, "Metadata.txt");
	FILE*arch = fopen(rutaTabla, "w+");
	fprintf(arch, "CONSISTENCIA=%s\nPARTICIONES=%s\nTIEMPO_COMPACTACION=%s\n", consistencia, cantidadParticiones, tiempoCompactacion);

	fclose(arch);
	free(rutaTabla);
	log_info(logger, "Metadata de %s creada\n", nombreTabla);

}

t_metadata_tabla obtenerMetadata(char* nombreTabla) {
	char* rutaTabla = armarRutaTabla(nombreTabla);
	string_append(&rutaTabla, "Metadata.txt");
	t_config* configMetadata = config_create(rutaTabla);
	t_metadata_tabla metadataTabla;
	metadataTabla.CONSISTENCIA = getConsistenciaByChar(config_get_string_value(configMetadata, "CONSISTENCIA"));
	metadataTabla.CANT_PARTICIONES = config_get_int_value(configMetadata, "PARTICIONES");
	metadataTabla.T_COMPACTACION = config_get_int_value(configMetadata, "TIEMPO_COMPACTACION");
	free(rutaTabla);
	config_destroy(configMetadata);
	return metadataTabla;
}

void mostrarMetadataTabla(t_metadata_tabla metadataTabla, char* nombreTabla) {
	printf("\nMetadata de %s: \n", nombreTabla);
	printf("CONSISTENCIA: %d\nPARTICIONES=%i\nTIEMPO_COMPACTACION=%i\n\n", metadataTabla.CONSISTENCIA, metadataTabla.CANT_PARTICIONES,
			metadataTabla.T_COMPACTACION);
}

char* armarRutaTabla(char* nombreTabla) {
	char* rutaTabla = string_duplicate(rutas.Tablas);
	string_append(&rutaTabla, nombreTabla);
	string_append(&rutaTabla, "/");
	return rutaTabla;
}

int contarArchivosTemporales(t_list* archivos) {
	bool isTmp(char* rutaArchivoActual) {
		char* extension = obtenerExtensionDeArchivoDeUnaRuta(rutaArchivoActual);
		bool response = strcmp(extension, "tmp") == 0;
		free(extension);
		return response;
	}
	return list_count_satisfying(archivos, (void*) isTmp);

}

int existeArchivo(char*nombreTabla, char * rutaArchivoBuscado) {
	t_list* archivos = buscarArchivos(nombreTabla);

	bool isArchivoBuscado(char* rutaArchivoActual) {
		return strcmp(rutaArchivoActual, rutaArchivoBuscado) == 0;
	}

	bool respuesta = list_any_satisfy(archivos, (void*) isArchivoBuscado);
	list_destroy_and_destroy_elements(archivos, free);
	return respuesta;

}

t_list* buscarArchivos(char* nombreTabla) {
	char* rutaTabla = armarRutaTabla(nombreTabla);

	DIR *directorioActual = opendir(rutaTabla);
	struct dirent *archivo;
	t_list* archivos = list_create();

	if (directorioActual == NULL) {
		log_error(loggerError, "No se pudo abrir el directorio.");

	} else {
		// Leo uno por uno los archivos que estan adentro del directorio actual
		while ((archivo = readdir(directorioActual)) != NULL) {

			//Con readdir aparece siempre . y .. como no me interesa no lo contemplo
			if ((strcmp(archivo->d_name, ".") != 0) && (strcmp(archivo->d_name, "..") != 0)) {

				char * rutaNueva = string_duplicate(rutaTabla);
				string_append(&rutaNueva, archivo->d_name);

				if (esArchivo(rutaNueva)) {
					list_add(archivos, rutaNueva);

				} else {
					free(rutaNueva);
				}
			}
		}
		closedir(directorioActual);
	}
	free(rutaTabla);
	return archivos;
}

int esArchivo(char* ruta) {
	struct stat estado;
	int i;
	stat(ruta, &estado);
	i = S_ISREG(estado.st_mode);

	return i;
}

int esDirectorio(char * ruta) {
	struct stat estado;
	int i;

	stat(ruta, &estado);
	i = S_ISDIR(estado.st_mode);

	return i;
}

void eliminarArchivo(char* rutaArchivo) {
	liberarBloquesDeArchivo(rutaArchivo);
	remove(rutaArchivo);
}

void removerArchivosDeTabla(char * nombreTabla) {
	t_list* archivos = buscarArchivos(nombreTabla);
	list_iterate(archivos, (void*) eliminarArchivo);
	list_destroy_and_destroy_elements(archivos, free);

}

int liberarBloquesDeArchivo(char *rutaArchivo) {

	t_archivo *archivo = malloc(sizeof(t_archivo));
	int result = leerArchivoDeTabla(rutaArchivo, archivo);
	if (result < 0) {
		free(archivo);
		return -1;
	}
	if (list_is_empty(archivo->BLOQUES)) {
		freeArchivo(archivo);
		return -1;
	}
	list_iterate(archivo->BLOQUES, (void*) liberarBloque);
	escribirBitmap();

	list_iterate(archivo->BLOQUES, (void*) borrarContenidoArchivoBloque);
	freeArchivo(archivo);
	return 1;
}

void borrarContenidoArchivoBloque(int bloque) {
	FILE* archivoBloque = obtenerArchivoBloque(bloque, false);
	fclose(archivoBloque);
}

int leerArchivoDeTabla(char *rutaArchivo, t_archivo *archivo) {
	t_config* config = config_create(rutaArchivo);

	if (config == NULL) {
		config_destroy(config);
		return -1;
		puts("El archivo no existe");
	}

	if (config_has_property(config, "TAMANIO")) {
		archivo->TAMANIO = config_get_int_value(config, "TAMANIO");
	} else {
		config_destroy(config);
		return -2; //Archivo corrupto
	}
	if (config_has_property(config, "BLOQUES")) {

		char** arrayBloques = config_get_array_value(config, "BLOQUES");
		int i = 0;
		archivo->BLOQUES = list_create();
		while (arrayBloques[i] != NULL) {

			list_add(archivo->BLOQUES, (void*) atoi(arrayBloques[i]));
			i++;
		}
		archivo->cantBloques = list_size(archivo->BLOQUES);
		freePunteroAPunteros(arrayBloques);
	} else {
		config_destroy(config);
		return -2;
	}

	config_destroy(config);
	return 1;
}

void removerTabla(char* nombreTabla) {

	bool isTablaBuscada(t_tabla_memtable* tablaActual) {
		return strcmp(tablaActual->tabla, nombreTabla) == 0;
	}

	list_remove_and_destroy_by_condition(memtable, (void*) isTablaBuscada, (void*) freeTabla);
	log_info(logger, "%s eliminada de memtable", nombreTabla);

	removerArchivosDeTabla(nombreTabla);
	char* rutaTabla = armarRutaTabla(nombreTabla);
	rmdir(rutaTabla);
	free(rutaTabla);
}

void freeTabla(t_tabla_memtable* tabla) {
	if (tabla->registros != NULL) {
		list_destroy_and_destroy_elements(tabla->registros, (void*) freeRegistro);
	}
	free(tabla->tabla);
	free(tabla);
}

void vaciarMemtable() {
	list_destroy_and_destroy_elements(memtable, (void*) freeTabla);
}

void buscarDirectorios(char * ruta, t_list* listaDirectorios) {

	DIR *directorioActual;
	struct dirent *directorio;
//aca estamos limitando los

	directorioActual = opendir(ruta);

	if (directorioActual == NULL) {
		log_error(loggerError, "No se pudo abrir el directorio.");
	} else {
		// Leo uno por uno los directorios que estan adentro del directorio actual
		while ((directorio = readdir(directorioActual)) != NULL) {

			//Con readdir aparece siempre . y .. como no me interesa no lo contemplo
			if ((strcmp(directorio->d_name, ".") != 0) && (strcmp(directorio->d_name, "..") != 0)) {

				char * rutaNueva = string_duplicate(ruta);
				string_append(&rutaNueva, directorio->d_name);
				if (esDirectorio(rutaNueva)) {
					list_add(listaDirectorios, rutaNueva);
				}
			}

		}

		closedir(directorioActual);
	}
}

char* obtenerNombreTablaByRuta(char* rutaTabla) {
	char* rutaAux = string_duplicate(rutaTabla);
	char** directorios = string_split(rutaAux, "/");
	char* nombreTabla = (char*) obtenerUltimoElementoDeUnSplit(directorios);
	freePunteroAPunteros(directorios);
	free(rutaAux);
	return nombreTabla;
}

void mostrarMetadataTodasTablas(char *ruta) {

	t_list* listaDirectorios = list_create();
	buscarDirectorios(ruta, listaDirectorios);

	char* obtenerNombreTablaByRuta(char* rutaTabla) {
		char** directorios = string_split(rutaTabla, "/");
		char* nombreTabla = obtenerUltimoElementoDeUnSplit(directorios);
		freePunteroAPunteros(directorios);
		return nombreTabla;
	}

	void describeDirectorio(char* directorio) {
		char* nombreTabla = obtenerNombreTablaByRuta(directorio);
		funcionDESCRIBE(nombreTabla);
		free(nombreTabla);
	}

	list_iterate(listaDirectorios, (void*) describeDirectorio);
	list_destroy_and_destroy_elements(listaDirectorios, free);

}

void insertarKey(char* nombreTabla, char* key, char* value, double timestamp) {
	int clave = atoi(key);
	t_tabla_memtable *tabla = getTablaFromMemtable(nombreTabla);

	t_registro* registro = malloc(sizeof(t_registro));
	registro->value = malloc(strlen(value) + 1);
	registro->timestamp = timestamp;
	registro->key = clave;
	strcpy(registro->value, value);

	list_add(tabla->registros, registro);

}

void crearYEscribirArchivosTemporales(char*ruta) {

	t_list* listaDirectorios = list_create();
	buscarDirectorios(ruta, listaDirectorios);

	list_iterate(listaDirectorios, (void*) crearYEscribirTemporal);
	list_destroy_and_destroy_elements(listaDirectorios, free);
}

void crearYEscribirTemporal(char* rutaTabla) {
	char* nombTabla = obtenerNombreTablaByRuta(rutaTabla);

	t_list*archivos = buscarArchivos(nombTabla);
	int cantidadTmp = contarArchivosTemporales(archivos);

	char* rutaArchTemporal = string_duplicate(rutaTabla);
	string_append(&rutaArchTemporal, "/");
	string_append_with_format(&rutaArchTemporal, "tmp%d.tmp", cantidadTmp);

	t_tabla_memtable* tabla = getTablaFromMemtable(nombTabla);
	if (tabla != NULL && !list_is_empty(tabla->registros)) {
		crearArchReservarBloqueYEscribirBitmap(rutaArchTemporal);
		printf("Archivo temporal creado en %s\n\n", nombTabla);

		t_list* registros = list_map(tabla->registros, (void*) registroToChar);

		//sem_wait(&mutexEscrituraBloques);
		escribirRegistrosEnBloquesByPath(registros, rutaArchTemporal);
		//sem_post(&mutexEscrituraBloques);

		list_destroy_and_destroy_elements(registros, free);
		limpiarRegistrosDeTabla(nombTabla);
	}

//	free(rutaArchTemporal);
	free(rutaArchTemporal);
	list_destroy_and_destroy_elements(archivos, free);
	free(nombTabla);

}

void limpiarRegistrosDeTabla(char*nombreTabla) {
	t_tabla_memtable* tabla = getTablaFromMemtable(nombreTabla);
	list_clean_and_destroy_elements(tabla->registros, (void*) freeRegistro);
	printf("Lista de Registros de %s limpia\n", nombreTabla);
}

char * obtenerNombreDeArchivoDeUnaRuta(char * ruta) {
	char* rutaAux = string_duplicate(ruta);
	char ** split = string_split(rutaAux, "/");
	char * archivoConExtension = (char*) obtenerUltimoElementoDeUnSplit(split);
	freePunteroAPunteros(split);
	free(rutaAux);
	if (strstr(archivoConExtension, ".") != NULL) {
		return archivoConExtension;
	}
	return NULL;

}

void crearArchReservarBloqueYEscribirBitmap(char* rutaArch) {
	FILE*archivo = fopen(rutaArch, "w");

	t_list* bloquesLibres = buscarBloquesLibres(1);
	if (bloquesLibres == NULL) {
		logErrorAndExit("No hay bloques libres para crear el archivo");
		return;
	}
	int bloqueLibre = (int) list_get(bloquesLibres, 0);

	reservarBloque(bloqueLibre);
	escribirBitmap();

	fprintf(archivo, "TAMANIO=0\n");
	fprintf(archivo, "BLOQUES=[%i]", bloqueLibre);
	fclose(archivo);

	list_destroy(bloquesLibres);

}
t_list* buscarRegistrosDeTabla(char*nombreTabla) {

	t_tabla_memtable* tabla = getTablaFromMemtable(nombreTabla);

	if (tabla != NULL && !list_is_empty(tabla->registros)) {
		t_list* registros = list_create();

		void agregarARegistros(t_registro* reg) {
			char registro[100];
			sprintf(registro, "%f;%d;%s\n", reg->timestamp, reg->key, reg->value);
			list_add(registros, string_duplicate(registro));
		}

		list_iterate(tabla->registros, (void*) agregarARegistros);
		return registros;

	}
	return NULL;

}
int obtenerTamanioListaRegistros(t_list* registros) {
	int tamanioTotal = 0;

	void acumularTamanioRegistro(char* registroActual) {
		tamanioTotal += strlen(registroActual) + 1;
	}
	list_iterate(registros, (void*) acumularTamanioRegistro);

	return tamanioTotal;
}

t_tabla_memtable* getTablaFromMemtable(char* nombreTabla) {

	bool isTablaBuscada(t_tabla_memtable* tablaActual) {
		return strcmp(tablaActual->tabla, nombreTabla) == 0;
	}
	return (t_tabla_memtable*) list_find(memtable, (void*) isTablaBuscada);
}

FILE* obtenerArchivoBloque(int numeroBloque, bool appendMode) {
	char* rutaBloque = string_new();
	string_append(&rutaBloque, rutas.Bloques);
	string_append_with_format(&rutaBloque, "/%d.bin", numeroBloque);

	FILE* archivoBloque;
	if (appendMode) {
		archivoBloque = fopen(rutaBloque, "a");
	} else {
		archivoBloque = fopen(rutaBloque, "w");
	}
	if (archivoBloque == NULL) {
		log_error(loggerError, "No se pudo abrir el archivo de bloque: %s ", rutaBloque);
		free(rutaBloque);
		return NULL;
	}
	free(rutaBloque);
	return archivoBloque;
}

int agregarNuevoBloque(t_archivo* archivo) {
	t_list* bloquesLibres = buscarBloquesLibres(1);
	if (bloquesLibres == NULL) {
		//no hay mas bloques libres
		log_error(loggerError, "No hay suficientes bloques libres");
		return -1;
	}
	int bloqueLibre = (int) list_get(bloquesLibres, 0);
	list_add(archivo->BLOQUES, (void*) bloqueLibre);
	list_destroy(bloquesLibres);
	return bloqueLibre;
}

/*le pasas el path de un archivo, se fija cuales son sus bloques
 y te escribe los registros en esos bloques*/

int escribirRegistrosEnBloquesByPathCompact(t_list* registrosAEscribir, char*pathArchivoAEscribir) {

	t_archivo *archivoTmp = malloc(sizeof(t_archivo));
	int res = leerArchivoDeTabla(pathArchivoAEscribir, archivoTmp);
	if (res < 0) {
		return -1;
	}

	int tamanioTotalBloquesEscritos = 0;
	int bloqueActual = (int) list_get(archivoTmp->BLOQUES, 0);
	void escribirRegistroEnBloque(char* registro) {

		FILE* archivoBloque = obtenerArchivoBloque(bloqueActual, true);
		//preguntamos si el registro entra en el bloque
		if (tamanioArchivo(archivoBloque) + (strlen(registro)) <= metadata.BLOCK_SIZE) {
			fprintf(archivoBloque, "%s", registro);
			tamanioTotalBloquesEscritos += strlen(registro);
			reservarBloque(bloqueActual);
			fclose(archivoBloque);
		} else {

			if ((bloqueActual = agregarNuevoBloque(archivoTmp)) == -1) {
				//no hay mas bloques libres
				return;
			}
			fclose(archivoBloque);
			escribirRegistroEnBloque(registro);
			//el problema si en si un registro del ciclo entra en algun bloque pasado... nos chupa un huevo

		}

	}

	list_iterate(registrosAEscribir, (void*) escribirRegistroEnBloque);
	tamanioTotalBloquesEscritos += 1 * list_size(archivoTmp->BLOQUES); //se ve que siempre el tamanio de bloque es la cantiadad de caracteres +1; VEMO
	escribirBitmap();
	archivoTmp->TAMANIO = tamanioTotalBloquesEscritos;
	escribirArchivo(pathArchivoAEscribir, archivoTmp);
	freeArchivo(archivoTmp);
	return 1;
}
int escribirRegistrosEnBloquesByPath(t_list* registrosAEscribir, char*pathArchivoAEscribir) {

	t_archivo *archivoTmp = malloc(sizeof(t_archivo));
	int res = leerArchivoDeTabla(pathArchivoAEscribir, archivoTmp);
	if (res < 0) {
		return -1;
	}

	int tamanioTotalBloquesEscritos = 0;
	int bloqueActual = (int) list_get(archivoTmp->BLOQUES, 0);
	void escribirRegistroEnBloque(char* registro) {

		FILE* archivoBloque = obtenerArchivoBloque(bloqueActual, true);
		//preguntamos si el registro entra en el bloque
		if (tamanioArchivo(archivoBloque) + (strlen(registro)) <= metadata.BLOCK_SIZE) {
			fprintf(archivoBloque, "%s;\n", registro);
			tamanioTotalBloquesEscritos += strlen(registro);
			reservarBloque(bloqueActual);
			fclose(archivoBloque);
		} else {

			if ((bloqueActual = agregarNuevoBloque(archivoTmp)) == -1) {
				//no hay mas bloques libres
				return;
			}
			fclose(archivoBloque);
			escribirRegistroEnBloque(registro);
			//el problema si en si un registro del ciclo entra en algun bloque pasado... nos chupa un huevo

		}

	}

	list_iterate(registrosAEscribir, (void*) escribirRegistroEnBloque);
	tamanioTotalBloquesEscritos += 1 * list_size(archivoTmp->BLOQUES); //se ve que siempre el tamanio de bloque es la cantiadad de caracteres +1; VEMO
	escribirBitmap();
	archivoTmp->TAMANIO = tamanioTotalBloquesEscritos;
	escribirArchivo(pathArchivoAEscribir, archivoTmp);
	freeArchivo(archivoTmp);
	return 1;
}

void freeArchivo(t_archivo *archivo) {
	list_destroy(archivo->BLOQUES);
	free(archivo);
}

void escribirArchivo(char*rutaArchivo, t_archivo *archivo) {

	FILE* arch = fopen(rutaArchivo, "w+");
	fprintf(arch, "TAMANIO=%i\n", archivo->TAMANIO);
	fprintf(arch, "BLOQUES=[");

	int index = 0;
	void escribirBloque(int bloque) {
		fprintf(arch, "%d", bloque);

		if (index < list_size(archivo->BLOQUES) - 1) {
			fprintf(arch, ",");
		}
		index++;
	}
	list_iterate(archivo->BLOQUES, (void*) escribirBloque);
	fprintf(arch, "]");
	fclose(arch);
}

void getRegistrosFromMemtableByNombreTabla(char* nombreTabla, t_list* listaRegistros) {
	t_tabla_memtable* tablaMemtable = getTablaFromMemtable(nombreTabla);
	if (tablaMemtable != NULL) {
		list_add_all(listaRegistros, tablaMemtable->registros);
	}

}

void getRegistrosFromTempByNombreTabla(char* nombreTabla, t_list* listaRegistros) {
	t_list* temporales = buscarTemporalesByNombreTabla(nombreTabla);
	t_list* registros = list_create();
	list_iterate2(temporales, (void*) agregarRegistrosFromBloqueByPath, registros);
	//t_list* registrosTempToChar = list_map(registros, (void*) registroToChar);
	list_add_all(listaRegistros, registros);
	list_destroy(registros);
	list_destroy_and_destroy_elements(temporales, free);
}

t_registro* buscarRegistroByKeyFromListaRegistros(t_list* listaRegistros, int key) {

	bool BuscarPorKey(t_registro* registroActual) {

		return registroActual->key == key;
	}

	t_registro* registro = list_find(listaRegistros, (void*) BuscarPorKey);
	if (registro != NULL) {
		return registro_duplicate(registro);
	}
	return NULL;
}

void getRegistrosFromBinByNombreTabla(char*nombreTabla, int keyActual, t_list*listaRegistros) {
	t_list* archivosBinarios = buscarBinariosByNombreTabla(nombreTabla);
	t_list* listaRegistrosBin = list_create();
	int cantParticiones = list_size(archivosBinarios);
	int numParticionABuscarKey = keyActual % cantParticiones;
	char* pathArchivo = list_get(archivosBinarios, numParticionABuscarKey);
	agregarRegistrosFromBloqueByPath(pathArchivo, listaRegistrosBin);
	//t_list* registrosBinToChar = list_map(listaRegistrosBin, (void*) registroToChar);
	list_add_all(listaRegistros, listaRegistrosBin);
	list_destroy_and_destroy_elements(archivosBinarios, free);
	list_destroy(listaRegistrosBin);
}

t_registro* getRegistroByKeyAndNombreTabla(char*nombreTabla, int keyActual) {
	t_list* registros = list_create();

	t_registro* registroMemtable = getRegistroFromMemtableByKey(nombreTabla, keyActual);
	if (registroMemtable != NULL) {
		list_add(registros,registroMemtable);

	}
	t_registro* registroTmp = getRegistroFromTmpByKey(nombreTabla, keyActual);
	if (registroTmp != NULL) {
		list_add(registros, registroTmp);
	}

	t_registro* registroBin = getRegistroFromBinByKey(nombreTabla, keyActual);
	if (registroBin != NULL) {
		list_add(registros, registroBin);
	}

	if(list_is_empty(registros)){
		list_destroy(registros);
		return NULL;
	}
	filtrarRegistros(registros);
	t_registro* registroFinal =list_get(registros,0);
	list_destroy(registros);
	return registroFinal;
}

t_registro* getRegistroFromBinByKey(char* nombreTabla, int key) {
	t_list* registros = list_create();
	getRegistrosFromBinByNombreTabla(nombreTabla, key, registros);
	if (list_is_empty(registros)) {
		list_destroy(registros);
		return NULL;
	}
	filtrarRegistros(registros);
	t_registro* registroEncontrado = buscarRegistroByKeyFromListaRegistros(registros, key);
	list_destroy_and_destroy_elements(registros, (void*) freeRegistro);
	return registroEncontrado;

}

t_registro* getRegistroFromTmpByKey(char* nombreTabla, int key) {
	t_list* registros = list_create();
	getRegistrosFromTempByNombreTabla(nombreTabla, registros);
	if (list_is_empty(registros)) {
		list_destroy(registros);
		return NULL;
	}
	filtrarRegistros(registros);
	t_registro* registroEncontrado = buscarRegistroByKeyFromListaRegistros(registros, key);
	list_destroy_and_destroy_elements(registros, (void*) freeRegistro);
	return registroEncontrado;
}

t_registro* getRegistroFromMemtableByKey(char* nombreTabla, int key) {
	t_tabla_memtable* tablaMemtable = getTablaFromMemtable(nombreTabla);
	if (tablaMemtable != NULL && !list_is_empty(tablaMemtable->registros)) {
		filtrarRegistros(tablaMemtable->registros);
		return buscarRegistroByKeyFromListaRegistros(tablaMemtable->registros, key);
	}
	return NULL;
}

