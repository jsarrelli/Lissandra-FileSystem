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
	char ** palabras = string_split(nombreArchivoConExtension, ".");
	char * extension = strdup(palabras[1]);
	liberarPunteroDePunterosAChar(palabras);
	free(palabras);
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
		return 1;
	}

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
		puts("No se pudo abrir el directorio.");
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

	list_destroy_and_destroy_elements(archivos, (void*) eliminarArchivo);

}

int liberarBloquesDeArchivo(char *rutaArchivo) {

	t_archivo *archivo = malloc(sizeof(t_archivo));
	int result = leerArchivoDeTabla(rutaArchivo, archivo);
	if (result < 0) {
		return -1;
	}
	if (list_is_empty(archivo->BLOQUES)) {
		return -1;
	}
	list_iterate(archivo->BLOQUES, (void*) liberarBloque);
	escribirBitmap();

	free(archivo);
	return 1;
}

int leerArchivoDeTabla(char *rutaArchivo, t_archivo *archivo) {

	t_config* config = config_create(rutaArchivo);

	if (config == NULL) {
		return -1;
		puts("El archivo no existe");
	}

	if (config_has_property(config, "TAMANIO")) {
		archivo->TAMANIO = config_get_int_value(config, "TAMANIO");
	} else {
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
	} else {
		return -2;
	}

	return 1;
}

void removerTabla(char* nombreTabla) {
	int tamanio = list_size(memtable);
	int i = 0;

	while (tamanio != i) {
		t_tabla_memtable * tabla = list_get(memtable, i);
		if (string_equals_ignore_case(tabla->tabla, nombreTabla)) {
			list_remove(memtable, i);
			printf("%s eliminada de memtable\n", nombreTabla);
			break;

		}
		i++;
	}

	removerArchivosDeTabla(nombreTabla);
	char* rutaTabla = armarRutaTabla(nombreTabla);
	rmdir(rutaTabla);
	free(rutaTabla);
}

void buscarDirectorios(char * ruta, t_list* listaDirectorios) {

	DIR *directorioActual;
	struct dirent *directorio;
//aca estamos limitando los

	directorioActual = opendir(ruta);

	if (directorioActual == NULL) {
		puts("No pudo abrir el directorio");
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
	int i;
	int clave = atoi(key);

	for (i = 0; i < list_size(memtable); i++) {
		t_tabla_memtable *tabla = list_get(memtable, i);
		if (strcmp(tabla->tabla, nombreTabla) == 0) {

			t_registro* registro = malloc(sizeof(t_registro));
			registro->value = malloc(strlen(value) + 1);
			registro->timestamp = timestamp;
			registro->key = clave;
			strcpy(registro->value, value);

			list_add(tabla->registros, registro);
			printf("Registro %f;%d;%s insertado correctamente en memtable, %s\n\n", timestamp, registro->key, registro->value, nombreTabla);

			break;
		}
	}
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
		escribirRegistrosEnBloquesByPath(registros, rutaArchTemporal);
		list_destroy_and_destroy_elements(registros, free);
		limpiarRegistrosDeTabla(nombTabla);
	}

//	free(rutaArchTemporal);
	list_destroy_and_destroy_elements(archivos, free);
	free(nombTabla);

}

void limpiarRegistrosDeTabla(char*nombreTabla) {
	t_tabla_memtable* tabla = getTablaFromMemtable(nombreTabla);
	list_clean_and_destroy_elements(tabla->registros, (void*) freeRegistro);
	printf("Lista de Registros de %s limpia\n", nombreTabla);
}

char * obtenerNombreDeArchivoDeUnaRuta(char * ruta) {
	char * archivoConExtension;
	char ** split = string_split(ruta, "/");
	archivoConExtension = (char*) obtenerUltimoElementoDeUnSplit(split);
	liberarPunteroDePunterosAChar(split);
	free(split);
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
	string_append(&rutaBloque, "/");
	string_append(&rutaBloque, string_itoa(numeroBloque));
	string_append(&rutaBloque, ".bin");
	FILE* archivoBloque;
	if (appendMode) {
		archivoBloque = fopen(rutaBloque, "ab");
	} else {
		archivoBloque = fopen(rutaBloque, "wb");
	}
	if (archivoBloque == NULL) {
		log_error(loggerError, "No se pudo abrir el archivo de bloque: %s ", rutaBloque);
		free(rutaBloque);
		return NULL;
	}
	free(rutaBloque);
	return archivoBloque;
}

/*le pasas el path de un archivo, se fija cuales son sus bloques
 y te escribe los registros en esos bloques*/
int escribirRegistrosEnBloquesByPath(t_list* registrosAEscribir, char*pathArchivoAEscribir) {

	t_archivo *archivoTmp = malloc(sizeof(t_archivo));
	int res = leerArchivoDeTabla(pathArchivoAEscribir, archivoTmp);
	if (res < 0) {
		return -1;
	}

	int bytesAEscribir = obtenerTamanioListaRegistros(registrosAEscribir);
	int bloquesNecesarios = (bytesAEscribir + metadata.BLOCK_SIZE - 1) / metadata.BLOCK_SIZE; // redondeo para arriba
	int bloquesReservados = archivoTmp->cantBloques;

	if (bloquesNecesarios > bloquesReservados) {
		int cantBloques = bloquesNecesarios - bloquesReservados;
		t_list* bloquesLibres = buscarBloquesLibres(cantBloques);

		if (bloquesLibres == NULL) {
			puts("Error al guardar datos en el archivoTmp. No hay suficientes bloques libres");
			return -1;
		}

		list_iterate(bloquesLibres, (void*) reservarBloque);
		list_add_all(archivoTmp->BLOQUES, bloquesLibres);
		archivoTmp->cantBloques = list_size(archivoTmp->BLOQUES);
		list_destroy(bloquesLibres);
	}

	int indexArchivoBloque = 0;
	int tamanioTotalBloquesEscritos = 0;

	void escribirRegistroEnBloque(char* registro) {

		int bloqueActual = (int) list_get(archivoTmp->BLOQUES, indexArchivoBloque);
		//			archivoBloque = obtenerArchivoBloque(bloqueActual, true);
//		//si el archivo de bloque no esta abierto, lo abrimos
//		if (archivoBloque == NULL) {
//			int bloqueActual = (int) list_get(archivoTmp->BLOQUES, indexArchivoBloque);
//			archivoBloque = obtenerArchivoBloque(bloqueActual, true);
//			if (archivoBloque == NULL) {
//				return;
//			}
//		}
		FILE* archivoBloque = obtenerArchivoBloque(bloqueActual, true);
		//preguntamos si el registro entra en el bloque
		if (tamanioArchivo(archivoBloque) + (strlen(registro) + 1) <= metadata.BLOCK_SIZE) {
			fprintf(archivoBloque, "%s", registro);
			tamanioTotalBloquesEscritos += strlen(registro) + 1;

		} else {

			indexArchivoBloque++;
			fclose(archivoBloque);
			escribirRegistroEnBloque(registro);
			//el problema si en si un registro del ciclo entra en algun bloque pasado... nos chupa un huevo

		}
		fclose(archivoBloque);
	}

	list_iterate(registrosAEscribir, (void*) escribirRegistroEnBloque);

	archivoTmp->TAMANIO = tamanioTotalBloquesEscritos;
	escribirArchivo(pathArchivoAEscribir, archivoTmp);
	free(archivoTmp);
	return 1;
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
