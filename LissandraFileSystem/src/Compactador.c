/*
 * Compactador.c
 *
 *  Created on: 23 abr. 2019
 *      Author: utnso
 */

#include "Compactador.h"

void compactarTabla(char*nombreTabla) {

	log_info(logger, "Obteniendo directorios de archivos binarios..");
	t_list* archivosBinarios = buscarBinariosByNombreTabla(nombreTabla);
	int cantParticiones = list_size(archivosBinarios);

	log_info(logger, "Modificando tmp a tmpc..");
	t_list* archivosTemporales = buscarTemporalesByNombreTabla(nombreTabla);
	t_list* archivosTmpc = cambiarExtensionTemporales(archivosTemporales);
	log_info(logger, "Se cambio la extension de los temporales");

	t_list* registrosNuevos = list_create();
	list_iterate2(archivosTmpc, (void*) agregarRegistrosFromBloqueByPath, registrosNuevos);
	log_info(logger, "Lectura de registros de los tmpc finalizada");
	filtrarRegistros(registrosNuevos);

	t_list* particionesRegistros = cargarRegistrosNuevosEnEstructuraParticiones(cantParticiones, registrosNuevos);

	//mergeamos los registros viejos con los nuevos y los escribimos en los bin
	mergearRegistrosNuevosConViejos(archivosBinarios, particionesRegistros);

	list_iterate(archivosTmpc, (void*) eliminarArchivo);
	list_destroy_and_destroy_elements(archivosTmpc, free);
	list_destroy_and_destroy_elements(archivosTemporales, free);
	list_destroy_and_destroy_elements(archivosBinarios, free);

	log_info(logger, "Compactacion de <%s> exitosa", nombreTabla);
}

void mergearRegistrosNuevosConViejos(t_list* archivosBinarios, t_list* particionesRegistrosNuevos) {
	int numeroParticionActual = 0;

	log_info(logger, "Mergeando registros viejos con nuevos");

	void mergearParticion(t_list* registrosNuevos) {

		if (list_is_empty(registrosNuevos)) {
			//si no hay registros nuevos en esta particion ni te calentes en entrar
			list_destroy(registrosNuevos);
			return;
		}
		//obtengo los registros del bin correspondiente
		t_list* listaRegistrosViejos = list_create();
		char* rutaArchivoBinarioActual = list_get(archivosBinarios, numeroParticionActual);
		agregarRegistrosFromBloqueByPath(rutaArchivoBinarioActual, listaRegistrosViejos);

		//agarro los registros nuevos de la particion actual
		t_list* registrosParticionActual = list_get(particionesRegistrosNuevos, numeroParticionActual);

		//armo una lista con los registros nuevos y viejos
		list_add_all(registrosParticionActual, listaRegistrosViejos);
		list_destroy(listaRegistrosViejos);

		//los filtro para que de los que tengan la misma key se quede con el de mayor timeStamp
		filtrarRegistros(registrosParticionActual);

		//pasamos la lista de registros a una lista de char* para escribirlos en el binario

		t_list* registrosChar = list_map(registrosParticionActual, (void*) registroToChar);

		//escribimos en el binario y sobreescribimos los bloques de ese archivo

		liberarBloquesDeArchivo(rutaArchivoBinarioActual);
		escribirRegistrosEnBloquesByPath(registrosChar, rutaArchivoBinarioActual);

		list_destroy_and_destroy_elements(registrosParticionActual, (void*) freeRegistro);
		list_destroy_and_destroy_elements(registrosChar, free);

		numeroParticionActual++;
	}

	list_iterate(particionesRegistrosNuevos, (void*) mergearParticion);
	list_destroy(particionesRegistrosNuevos);
}

char* registroToChar(t_registro* registro) {
	char* registroChar = string_new();
	string_append_with_format(&registroChar, "%d;%s;%f\n", registro->key, registro->value, registro->timestamp);
	return registroChar;
}

t_list* cargarRegistrosNuevosEnEstructuraParticiones(int cantParticiones, t_list* registrosNuevos) {
	t_list* particionesDeRegistrosNuevos = list_create();
	/*aca vamos a crear una lista de particiones
	 * y a su vez en cada particion vamos a guardar una lista de los registros nuevos que corresponden a cada una*/
	for (int i = 0; i < cantParticiones; i++) {
		t_list* particionRegistrosNuevos = list_create();
		list_add(particionesDeRegistrosNuevos, particionRegistrosNuevos);
	}

	void cargarEnParticionCorrespondiente(t_registro* registroNuevo) {
		int numeroParticionCorrespondiente = registroNuevo->key % cantParticiones;
		if (numeroParticionCorrespondiente > list_size(particionesDeRegistrosNuevos)) {
			freeRegistro(registroNuevo);
			log_error(logger, "Hermano me metiste un registro de una particion que esta tabla no tiene, no te hagas el loco");
			return;
		}
		t_list* particionCorrespondiente = list_get(particionesDeRegistrosNuevos, numeroParticionCorrespondiente);
		list_add(particionCorrespondiente, registroNuevo);
	}
	list_iterate(registrosNuevos, (void*) cargarEnParticionCorrespondiente);
	list_destroy(registrosNuevos);
	return particionesDeRegistrosNuevos;
}

//void filtrarPorTimeStamp(t_list* registros) {
//	log_info(logger, "Filtrando Registros por TimeStamp..");
//
//	void eliminar(t_link_element* ptr) {
//		if (ptr->data != NULL) {
//			freeRegistro(ptr->data);
//			free(ptr);
//		}
//	}
//	t_link_element* ant;
//	void compararYeliminar(t_link_element* ptr) {
//		if (ptr->next == NULL) {
//			return;
//			//llego al final
//		}
//
//		t_link_element* next = ptr->next;
//		t_registro* registroActual = ptr->data;
//		t_registro* registroSiguiente = next->data;
//
//		if (registroActual->key == registroSiguiente->key) {
//			t_link_element* victima;
//
//			if (registroActual->timestamp > registroSiguiente->timestamp) {
//				victima = ptr->next;
//
//				ptr->next = victima->next;
//
//				eliminar(victima);
//				compararYeliminar(ptr);
//			} else {
//
//				victima = ptr;
//
//				ptr = ptr->next;
//				if (victima == registros->head) {
//					registros->head = ptr;
//				}
//
//				eliminar(victima);
//				//avanzo
//				ant->next = ptr;
//				compararYeliminar(ptr);
//			}
//		}
//		ant = ptr;
//		compararYeliminar(ptr->next);
//	}
//
//}

//hay una funcion mucho mas linda, pero son las 5 am tengo suenio y la vida es una mierda
void filtrarRegistros(t_list* registros) {
	log_info(logger, "Filtrando registros..");
	void eliminarDuplicadoIfTimestampMenor(t_registro* registro) {

		int index = 0;
		bool isDuplicadoConTimeStampMenor(t_registro* registroActual) {

			bool var = registroActual->key == registro->key && registro->timestamp > registroActual->timestamp;
			if (!var) {
				index++;
			}
			return var;

		}
		t_registro* registroDuplicado = list_find(registros, (void*) isDuplicadoConTimeStampMenor);

		if (registroDuplicado != NULL) {

			list_remove_and_destroy_element(registros, index, (void*) freeRegistro);
		}

	}

	list_iterate(registros, (void*) eliminarDuplicadoIfTimestampMenor);

	bool sortByKey(t_registro* registro1, t_registro* registro2) {
		return registro1->key >= registro2->key;
	}

	list_sort(registros, (void*) sortByKey);

	log_info(logger, "Filtrado completo");

}
//void filtrarRegistros(t_list* registros) {
//
//	if (registros != NULL && !list_is_empty(registros)) {
//
//		bool sortByKey(t_registro* registro1, t_registro* registro2) {
//			return registro1->key >= registro2->key;
//		}
//		ordernarLista(registros, (void*) sortByKey);
//
//		filtrarPorTimeStamp(registros);
//	}
//}

void agregarRegistrosDeBin(char* rutaBinario, t_list* listaRegistrosViejos) {
	t_archivo* archivo = malloc(sizeof(t_archivo));
	leerArchivoDeTabla(rutaBinario, archivo);
	int i = 0, j;
	char* rutaArchBloque = malloc(100);
	strcpy(rutaArchBloque, rutas.Bloques);
	int tamanioArchBloque, fd;
	FILE* archivoBloque;

	char * archivoMapeado;
	char ** registros;

	void levantarRegistrosDeBloque(int bloque) {
		string_append_with_format(&rutaArchBloque, "%s.bin", archivo->BLOQUES[i]);
		archivoBloque = fopen(rutaArchBloque, "rb");
		tamanioArchBloque = tamanioArchivo(archivoBloque);
		fd = fileno(archivoBloque);

		if ((archivoMapeado = mmap(NULL, tamanioArchBloque, PROT_READ, MAP_SHARED, fd, 0)) == MAP_FAILED) {
			logErrorAndExit("Error al hacer mmap al levantar archivo de bloque");
		}
		fclose(archivoBloque);
		close(fd);

		registros = string_split(archivoMapeado, "\n");
		int cant = contarPunteroDePunteros(registros);
		for (j = 0; j < cant; j++) {
			list_add(listaRegistrosViejos, registros[j]);
		}

		munmap(archivoMapeado, tamanioArchBloque);

		rutaArchBloque = obtenerRutaTablaSinArchivo(rutaArchBloque);
		i++;
	}

	list_iterate(archivo->BLOQUES, (void*) levantarRegistrosDeBloque);

	liberarPunteroDePunterosAChar(registros);
	free(registros);
	free(rutaArchBloque);
	freeArchivo(archivo);
}

t_list* buscarBinariosByNombreTabla(char* nombreTabla) {
	t_list* archivos = buscarArchivos(nombreTabla);
	bool isBin(char* rutaArchivoActual) {
		char* extension = obtenerExtensionDeArchivoDeUnaRuta(rutaArchivoActual);
		bool result = strcmp(extension, "bin") == 0;
		free(extension);
		return result;
	}

	t_list* archivosBinarios = list_filter(archivos, (void*) isBin);
	list_destroy_and_destroy_elements(archivos, free);
	return archivosBinarios;
}

/*levanta los registros de un path (binario o temporal)
 y te los cargar en la lista que le pases*/
void agregarRegistrosFromBloqueByPath(char* pathArchivo, t_list* listaRegistros) {
	t_archivo* archivo = malloc(sizeof(t_archivo));
	if (leerArchivoDeTabla(pathArchivo, archivo) == -1) {
		return;
	}

	int i = 0;
	log_info(logger, "Leyendo registros de %s... ", pathArchivo);

	void cargarRegistrosDeBloque(int bloque) {

		char* rutaArchivoBloque = string_duplicate(rutas.Bloques);
		string_append_with_format(&rutaArchivoBloque, "%d.bin\0", bloque);
		///home/utnso/tp-2019-1c-Los-Sisoperadores/LissandraFileSystem/FS_LISSANDRA/Bloques/48.bin
		t_list* registrosObtenidos = obtenerRegistrosFromBloque(rutaArchivoBloque);
		if (registrosObtenidos != NULL) {
			if (!list_is_empty(registrosObtenidos)) {
				list_add_all(listaRegistros, registrosObtenidos);
			}
			//borro la referecia a registros obtenidos ya que ya estan cargados en listaRegistros
			list_destroy(registrosObtenidos);
		}

		free(rutaArchivoBloque);
		i++;
	}

	list_iterate(archivo->BLOQUES, (void*) cargarRegistrosDeBloque);
	freeArchivo(archivo);
}

t_list* obtenerRegistrosFromBinByNombreTabla(char* nombreTabla) {
	t_list* binarios = buscarBinariosByNombreTabla(nombreTabla);
	t_list* registros = list_create();
	list_iterate2(binarios, (void*) agregarRegistrosFromBloqueByPath, registros);
	list_destroy_and_destroy_elements(binarios, free);
	return registros;

}

t_list* obtenerRegistrosFromTempByNombreTabla(char* nombreTabla) {
	t_list* binarios = buscarTemporalesByNombreTabla(nombreTabla);
	t_list* registros = list_create();
	list_iterate2(binarios, (void*) agregarRegistrosFromBloqueByPath, registros);
	list_destroy_and_destroy_elements(binarios, free);
	return registros;

}

t_list* obtenerRegistrosFromBloque(char* rutaArchivoBloque) {

	t_list* listaRegistros = list_create();
	FILE* archivoBloque = fopen(rutaArchivoBloque, "rb");
//	int tamanioArchBloque = tamanioArchivo(archivoBloque);
//	int fileDescriptor = fileno(archivoBloque);
//
//	//mapeamos el archivo a memoria
//	char * archivoMapeado;
//	if ((archivoMapeado = mmap(NULL, tamanioArchBloque, PROT_READ, MAP_SHARED, fileDescriptor, 0)) == MAP_FAILED) {
//		log_error(loggerError, "Error al hacer mmap al levantar archivo de bloque");
//		return NULL;
//	}

//cargo la lista con los registros obtenidos
	char registro[400];
	while (fgets(registro, sizeof registro, archivoBloque) != NULL) {
		char* registroActual = string_duplicate(registro);
		char** valores = string_split(registroActual, ";");
		t_registro* registro = registro_new(valores);
		list_add(listaRegistros, registro);
		free(registroActual);
	}
	fclose(archivoBloque);

//	char ** registros = string_split(archivoMapeado, "\n");
//	int j = 0;
//	while (registros[j] != NULL) {
//		char* registroActual = string_duplicate(registros[j]);
//		char** valores = string_split(registroActual, ";");
//		t_registro* registro = registro_new(valores);
//		list_add(listaRegistros, registro);
//		j++;
//		free(registroActual);
//	}

//fijate que este free no se este ya liberando con el free de valores
//	freePunteroAPunteros(registros);
//	munmap(archivoMapeado, tamanioArchBloque);
	//fclose(archivoBloque);
//	close(fileDescriptor);
	return listaRegistros;
}

t_list* cambiarExtensionTemporales(t_list* listaTemporalesTmp) {

	char* cambiarExtensionATmpc(char* rutaTmpActual) {
		char* rutaTmpc = string_duplicate(rutaTmpActual);
		string_append(&rutaTmpc, "c");
		rename(rutaTmpActual, rutaTmpc);
		return rutaTmpc;
	}

	return list_map(listaTemporalesTmp, (void*) cambiarExtensionATmpc);

}

/*devuelve los paths en donde se encuentran los temporales*/
t_list* buscarTemporalesByNombreTabla(char* nombreTabla) {
	t_list* archivos = buscarArchivos(nombreTabla);

	bool isTemporal(char* rutaArchivoActual) {
		char* extension = obtenerExtensionDeArchivoDeUnaRuta(rutaArchivoActual);
		bool result = strcmp(extension, "tmp") == 0;
		free(extension);
		return result;
	}
	t_list* archivosTemporales = list_filter(archivos, (void*) isTemporal);
	list_destroy_and_destroy_elements(archivos, free);
	return archivosTemporales;

}

void iniciarThreadCompactacion(char* nombreTabla) {
	pthread_t threadCompactacion;
	t_metadata_tabla metadata = obtenerMetadata(nombreTabla);
	while (0) {
		if (!existeTabla(nombreTabla)) {
			return;
		}
		usleep(metadata.T_COMPACTACION * 1000);
		pthread_create(&threadCompactacion, NULL, (void*) crearYEscribirArchivosTemporales, rutas.Tablas);
		pthread_detach(threadCompactacion);
	}

}
