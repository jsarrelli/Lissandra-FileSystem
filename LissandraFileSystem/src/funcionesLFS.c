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

void crearTablaYParticiones(char* nombreTabla, char* cantidadParticiones) {
	t_tabla_memtable* tabla;

	char* rutaTabla = malloc(100);
	strcpy(rutaTabla, rutas.Tablas);
	int i = 0, j = 1;
	int cantPart = atoi(cantidadParticiones);

	string_append(&rutaTabla, "/");
	string_append(&rutaTabla, nombreTabla);
	mkdir(rutaTabla, 0777);
	printf("%s creada en LFS\n", nombreTabla);
	log_info(logger, "Se creo la  %s en LFS\n", nombreTabla);

	while (i < cantPart) {
		string_append(&rutaTabla, "/");
		string_append_with_format(&rutaTabla, "%d.bin", i);
		crearArchReservarBloqueYEscribirBitmap(rutaTabla);

		printf("Particion %d creada \n", j);
		rutaTabla = obtenerRutaTablaSinArchivo(rutaTabla);
		i++;
		j++;
	}

	tabla = malloc(sizeof(t_tabla_memtable));
	strcpy(tabla->tabla, nombreTabla);
	tabla->registros = list_create();
	list_add(memtable, tabla);
	printf("%s insertada en memtable \n", tabla->tabla);

	free(rutaTabla);

}

void crearMetadataTabla(char*nombreTabla, char* consistencia, char* cantidadParticiones, char* tiempoCompactacion) {
	char*rutaTabla = armarRutaTabla(nombreTabla);
	string_append(&rutaTabla, "Metadata.txt");
	FILE*arch = fopen(rutaTabla, "w+");
	fprintf(arch, "CONSISTENCIA=%s\nPARTICIONES=%s\nTIEMPO_COMPACTACION=%s\n", consistencia, cantidadParticiones,
			tiempoCompactacion);

	fclose(arch);
	log_info(logger, "Metadata de %s creada\n", nombreTabla);

}

t_metadata_tabla obtenerMetadata(char* nombreTabla) {
	char* rutaTabla = armarRutaTabla(rutaTabla, nombreTabla);
	string_append(&rutaTabla, "Metadata.txt");
	t_config* configMetadata = config_create(rutaTabla);
	t_metadata_tabla metadataTabla;
	metadataTabla.CONSISTENCIA = getConsistenciaByChar(config_get_string_value(configMetadata, "CONSISTENCIA"));
	metadataTabla.CANT_PARTICIONES = config_get_int_value(configMetadata, "PARTICIONES");
	metadataTabla.T_COMPACTACION = config_get_int_value(configMetadata, "TIEMPO_COMPACTACION");
	free(rutaTabla);
	return metadataTabla;
}

void mostrarMetadataTabla(t_metadata_tabla metadataTabla, char* nombreTabla) {
	printf("\nMetadata de %s: \n", nombreTabla);
	printf("CONSISTENCIA: %d\nPARTICIONES=%i\nTIEMPO_COMPACTACION=%i\n\n", metadataTabla.CONSISTENCIA,
			metadataTabla.CANT_PARTICIONES, metadataTabla.T_COMPACTACION);
}

char* armarRutaTabla(char* nombreTabla) {
	char* rutaTabla = string_new();
	strcpy(rutaTabla, rutas.Tablas);
	string_append(&rutaTabla, nombreTabla);
	string_append(&rutaTabla, "/");

	return rutaTabla;
}

int contarArchivosTemporales(t_list* archivos) {
	bool isTmp(char* rutaArchivoActual) {
		char* extension = obtenerExtensionDeArchivoDeUnaRuta(rutaArchivoActual);
		return strcmp(extension, "tmp") == 0;
	}
	return list_count_satisfying(archivos, isTmp);

}

int existeArchivo(char*nombreTabla, char * rutaArchivoBuscado) {
	int i = 0;

	char *archivo;
	t_list* archivos = buscarArchivos(nombreTabla);

	bool isArchivoBuscado(char* rutaArchivoActual) {
		return strcmp(rutaArchivoActual, rutaArchivoBuscado) == 0;
	}

	bool respuesta = list_any_satisfy(archivos, isArchivoBuscado);
	list_destroy(archivos);
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
				string_append(&rutaNueva, "/");
				string_append(&rutaNueva, archivo->d_name);

				if (esArchivo(rutaNueva)) {
					list_add(archivos, rutaNueva);

				}
			}
		}
		closedir(directorioActual);
	}
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

void removerArchivosDeTabla(char * rutaTabla) {
	t_list* archivos = buscarArchivos(rutaTabla);

	void eliminarArchivo(char* rutaArchivo) {
		liberarBloquesDeArchivo(rutaArchivo);
		remove(rutaArchivo);

	}
	list_destroy_and_destroy_elements(archivos, (void*) eliminarArchivo);

}

int liberarBloquesDeArchivo(char *rutaArchivo) {

	t_archivo *archivo = malloc(sizeof(t_archivo));
	int result = leerArchivoDeTabla(rutaArchivo, archivo);
	if (result < 0) {
		return -1;
	}
	int i;
	for (i = 0; i < archivo->cantBloques; ++i) {
		liberarBloque(strtol(archivo->BLOQUES[i], NULL, 10));
		log_info(logger, "Bloque liberado: %s\n", archivo->BLOQUES[i]);
		printf("Bloque liberado: %s\n", archivo->BLOQUES[i]);
		i++;
	}
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
		archivo->BLOQUES = config_get_array_value(config, "BLOQUES");
		int cant = 0;
		for (cant = 0; archivo->BLOQUES[cant]; cant++)
			;
		archivo->cantBloques = cant;
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

	char* rutaTabla = armarRutaTabla(nombreTabla);
	removerArchivosDeTabla(rutaTabla);
	rmdir(rutaTabla);
	free(rutaTabla);
}

void buscarDirectorios(char * ruta, t_list* listaDirectorios) {

	DIR *directorioActual;
	struct dirent *directorio;
//aca estamos limitando los
	char * rutaNueva;
	directorioActual = opendir(ruta);

	if (directorioActual == NULL) {
		puts("No pudo abrir el directorio");
		log_error(loggerError, "No se pudo abrir el directorio.");
	} else {
		// Leo uno por uno los directorios que estan adentro del directorio actual
		while ((directorio = readdir(directorioActual)) != NULL) {

			//Con readdir aparece siempre . y .. como no me interesa no lo contemplo
			if ((strcmp(directorio->d_name, ".") != 0) && (strcmp(directorio->d_name, "..") != 0)) {

				rutaNueva = string_duplicate(ruta);
				string_append(&rutaNueva, directorio->d_name);

				if (esDirectorio(rutaNueva)) {
					//strcpy(directorios[i], rutaNueva);
					char* direccion = malloc(strlen(rutaNueva));
					strcpy(direccion, rutaNueva);
					list_add(listaDirectorios, direccion);
				}
				free(rutaNueva);
			}

		}

		closedir(directorioActual);
	}
}

char* obtenerNombreTablaByRuta(char* rutaTabla) {
	char** directorios = string_split(rutaTabla, "/");
	return (char*) obtenerUltimoElementoDeUnSplit(directorios);
}

void mostrarMetadataTodasTablas(char *ruta) {

	t_list* listaDirectorios = list_create();
	buscarDirectorios(ruta, listaDirectorios);

	char* obtenerNombreTablaByRuta(char* rutaTabla) {
		char** directorios = string_split(rutaTabla, "/");
		return (char*) obtenerUltimoElementoDeUnSplit(directorios);
	}

	void describeDirectorio(char* directorio) {
		char* nombreTabla = obtenerNombreTablaByRuta(directorio);
		funcionDESCRIBE(nombreTabla);
	}

	list_iterate(listaDirectorios, (void*) describeDirectorio);
	list_destroy(listaDirectorios);

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
			printf("Registro %f;%d;%s insertado correctamente en memtable, %s\n\n", timestamp, registro->key,
					registro->value, nombreTabla);

			break;
		}
	}
}

void crearYEscribirArchivosTemporales(char*ruta) {

	t_list* listaDirectorios = list_create();
	buscarDirectorios(ruta, listaDirectorios);

	list_iterate(listaDirectorios, (void*) crearYEscribirTemporal);
	list_destroy(listaDirectorios);
}

void crearYEscribirTemporal(char*rutaTabla) {

	char* nombTabla = malloc(50);
	char**palabras = string_split(rutaTabla, "/");
	nombTabla = (char*) obtenerUltimoElementoDeUnSplit(palabras);
	t_list*archivos = buscarArchivos(rutaTabla);
	int cantidadTmp = contarArchivosTemporales(archivos);

	string_append(&rutaTabla, "/");
	string_append_with_format(&rutaTabla, "%stmp%d.tmp", nombTabla, cantidadTmp);

	t_tabla_memtable* tabla = getTablaFromMemtable(nombTabla);
	if (tabla != NULL && !list_is_empty(tabla->registros)) {
		crearArchReservarBloqueYEscribirBitmap(rutaTabla);
		printf("Archivo temporal creado en %s\n\n", nombTabla);
		escribirEnTmp(nombTabla, rutaTabla);
		limpiarRegistrosDeTabla(nombTabla);
	}

	liberarPunteroDePunterosAChar(palabras);
	free(palabras);
	list_destroy(archivos);
	free(nombTabla);

}

void limpiarRegistrosDeTabla(char*nombreTabla) {
	t_tabla_memtable* tabla;
	int i;

	for (i = 0; i < list_size(memtable); i++) {
		tabla = (t_tabla_memtable*) list_get(memtable, i);
		if (!strcmp(tabla->tabla, nombreTabla)) {
			list_clean(tabla->registros);
		}

	}

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
	int *bloque = buscarBloquesLibres(1);

	if (bloque == NULL) {
		logErrorAndExit("No hay bloques libres para crear el archivo");
	}

	reservarBloque(bloque[0]);
	escribirBitmap();

	fprintf(archivo, "TAMANIO=0\n");

	fprintf(archivo, "BLOQUES=[%i]", bloque[0]);
	fclose(archivo);
	free(bloque);

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

int escribirEnTmp(char*nombreTabla, char*rutaTmp) {

	int*bloques;
	char *rutaBloque = malloc(150);
	char **arrBloques = malloc(40);
//char*registro = malloc(100);
	t_archivo *archivo = malloc(sizeof(t_archivo));
	strcpy(rutaBloque, rutas.Bloques);
	t_list* registros = buscarRegistrosDeTabla(nombreTabla);
	int res = leerArchivoDeTabla(rutaTmp, archivo);
	if (res < 0) {
		return -1;
	}
	int bytesAEscribir = obtenerTamanioArrayRegistros(registros);
	int bloquesNecesarios = (bytesAEscribir + metadata.BLOCK_SIZE - 1) / metadata.BLOCK_SIZE; // redondeo para arriba
	int bloquesReservados = archivo->cantBloques;
	int i, j;

	if (bloquesNecesarios > bloquesReservados) {
		int cantBloques = bloquesNecesarios - bloquesReservados;
		bloques = buscarBloquesLibres(cantBloques);
		if (bloques == NULL) {
			puts("Error al guardar datos en el archivo. No hay suficientes bloques libres");
			return -1;
		}

		for (i = 0; i < archivo->cantBloques; ++i) {
			arrBloques[i] = archivo->BLOQUES[i];
		}

		for (j = 0; j < cantBloques; ++j) {
			reservarBloque(bloques[j]);
			arrBloques[i] = string_itoa(bloques[j]);
			i++;
		}

		archivo->cantBloques = bloquesNecesarios;
		for (i = 0; i < archivo->cantBloques; i++) {
			archivo->BLOQUES[i] = arrBloques[i];
		}

		free(bloques);

	}

	FILE*archivoBloque;
	i = 0, j = 0;
	char* registroActual = list_get(registros, j);

	while (archivo->BLOQUES[i] != NULL) {
		int tamanioArchBloque = 0;

		string_append(&rutaBloque, archivo->BLOQUES[i]);
		string_append(&rutaBloque, ".bin");
		archivoBloque = fopen(rutaBloque, "wb");
		if (archivoBloque == NULL) {
			puts("Error al guardar datos en bloques");
			return -1;
		}

		//int cantBytes = strlen(registros[j]) + 1;
		while (tamanioArchBloque + (strlen(registroActual) + 1) < metadata.BLOCK_SIZE && j < list_size(registros)) {
			char registro[strlen(registroActual) + 1];
			strcpy(registro, registroActual);
			//registro = string_duplicate(registros[j]);
			fwrite(registro, 1, sizeof(registro), archivoBloque);
			printf("Registro %s insertado en bloque de .tmp\n", registro);
			tamanioArchBloque += (sizeof(registro));
			bytesAEscribir -= (sizeof(registro));
			if (bytesAEscribir == 0) {
				fclose(archivoBloque);
				archivo->TAMANIO += tamanioArchBloque;
				escribirArchivo(rutaTmp, archivo);
				escribirBitmap();
				free(archivo->BLOQUES);
				free(arrBloques);
				free(archivo);
				free(rutaBloque);
				list_destroy(registros);
				puts("Se termino de escribir en el temporal\n");
				//free(registro);
				return 1;
			}

			registroActual = list_get(registros, j++);
		}
		fclose(archivoBloque);
		archivo->TAMANIO += tamanioArchBloque;
		rutaBloque = obtenerRutaTablaSinArchivo(rutaBloque);
		string_append(&rutaBloque, "/");
		i++;
	}

	return 1;
}

void escribirArchivo(char*rutaArchivo, t_archivo *archivo) {

	FILE* arch = fopen(rutaArchivo, "w+");
	fprintf(arch, "TAMANIO=%i\n", archivo->TAMANIO);
	fprintf(arch, "BLOQUES=[");
	int i;
	for (i = 0; i < archivo->cantBloques; ++i) {
		if (i != 0) {
			fprintf(arch, ",");
		}
		fprintf(arch, "%s", archivo->BLOQUES[i]);
	}
	fprintf(arch, "]");
	fclose(arch);
}
