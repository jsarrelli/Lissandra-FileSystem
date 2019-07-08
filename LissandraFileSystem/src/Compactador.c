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
	cambiarExtensionTemporales(archivosTemporales);
	log_info(logger, "Se cambio la extension de los temporales");

	t_list* registrosNuevos = list_create();
	list_iterate2(archivosTemporales, (void*) agregarRegistrosFromBloqueByPath, registrosNuevos);
	log_info(logger, "Lectura de registros de los tmpc finalizada");
	filtrarRegistros(registrosNuevos);

	t_list* particionesRegistros = cargarRegistrosNuevosEnEstructuraParticiones(cantParticiones, registrosNuevos);

	//mergeamos los registros viejos con los nuevos y los escribimos en los bin
	mergearRegistrosNuevosConViejos(archivosBinarios, particionesRegistros);
}

void mergearRegistrosNuevosConViejos(t_list* archivosBinarios, t_list* particionesRegistrosNuevos) {
	int numeroParticionActual = 0;

	void mergearParticion(t_list* registrosNuevos) {

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
		char* registroToChar(t_registro* registro) {
			char* registroChar = string_new();
			scanf(registroChar, "%d;%s;%f\n", registro->key, registro->value, registro->timestamp);
			return registroChar;
		}
		t_list* registrosChar = list_map(registrosParticionActual, (void*) registroToChar);

		//escribimos en el binario y los bloques de ese archivo
		escribirEnBin(registrosChar, rutaArchivoBinarioActual);

		list_destroy_and_destroy_elements(registrosParticionActual, (void*) freeRegistro);
		list_destroy_and_destroy_elements(registrosChar, free);

		numeroParticionActual++;
	}

	list_iterate(particionesRegistrosNuevos, (void*) mergearParticion);
	list_destroy(particionesRegistrosNuevos);
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
		t_list* particionCorrespondiente = list_get(particionesDeRegistrosNuevos, numeroParticionCorrespondiente);
		list_add(particionCorrespondiente, registroNuevo);
	}
	list_iterate(registrosNuevos, (void*) cargarEnParticionCorrespondiente);
	list_destroy(registrosNuevos);
	return particionesDeRegistrosNuevos;
}

void filtrarPorTimeStamp(t_list* registros) {
	t_link_element* ptr = registros->head;
	while (ptr != NULL) {
		t_registro* registroActual = ptr->data;
		t_link_element* next = ptr->next;
		t_registro* registroSiguiente = next->data;
		if (registroActual->key == registroSiguiente->key) {
			t_link_element* aux;
			if (registroActual->timestamp > registroSiguiente->timestamp) {
				aux = next;
				ptr->next = next->next;
				freeRegistro(registroSiguiente);
			} else {
				aux = ptr;
				ptr = next;
				freeRegistro(registroActual);
			}
			free(aux);
		} else {
			ptr = next;
		}
	}
}

void filtrarRegistros(t_list* registros) {

	bool sortByKey(t_registro* registro1, t_registro* registro2) {
		return registro1->key >= registro2->key;
	}
	ordernarLista(registros, (void*) sortByKey);

	filtrarPorTimeStamp(registros);
}

void persistirParticionesDeTabla(t_list* listaListas, t_list*archivosBin) {
	char*rutaBinario = string_new();
	int i;
	t_list*lista;
	for (i = 0; i < list_size(archivosBin); i++) {
		rutaBinario = list_get(archivosBin, i);
		lista = list_get(listaListas, i);

		escribirEnBin(lista, rutaBinario);
	}
}

int escribirEnBin(t_list* lista, char* rutaBinario) {
	int*bloques;
	t_archivo*archivo = malloc(sizeof(t_archivo));
	leerArchivoDeTabla(rutaBinario, archivo);
	char *rutaBloque = malloc(150);
	char **arrBloques = malloc(40);
	char*reg = string_new();
	int bytesAEscribir = obtenerTamanioListaRegistros(lista);

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
	reg = list_get(lista, j);
	while (archivo->BLOQUES[i] != NULL) {
		int tamanioArchBloque = 0;

		string_append(&rutaBloque, archivo->BLOQUES[i]);
		string_append(&rutaBloque, ".bin");

		remove(rutaBloque);
		archivoBloque = fopen(rutaBloque, "wb");
		if (archivoBloque == NULL) {
			puts("Error al guardar datos en bloques");
			return -1;
		}

		//int cantBytes = strlen(registros[j]) + 1;
		while (tamanioArchBloque + (strlen(reg) + 1) < metadata.BLOCK_SIZE && j < list_size(lista)) {
			char registro[strlen(reg) + 1];
			strcpy(registro, registro);
			//registro = string_duplicate(registros[j]);
			fwrite(registro, 1, sizeof(registro), archivoBloque);
			printf("Registro %s insertado en bloque de particion .bin\n", registro);
			tamanioArchBloque += (sizeof(registro));
			bytesAEscribir -= (sizeof(registro));
			if (bytesAEscribir == 0) {
				fclose(archivoBloque);
				archivo->TAMANIO += tamanioArchBloque;
				escribirArchivo(rutaBinario, archivo);
				escribirBitmap();
				free(archivo->BLOQUES);
				free(arrBloques);
				free(archivo);
				free(reg);
				free(rutaBloque);
				puts("Se termino de escribir en el temporal\n");
				//free(registro);
				return 1;
			}

			reg = list_get(lista, j++);
		}
		fclose(archivoBloque);
		archivo->TAMANIO += tamanioArchBloque;
		rutaBloque = obtenerRutaTablaSinArchivo(rutaBloque);
		string_append(&rutaBloque, "/");
		i++;
	}

	return 1;
}
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

	while (archivo->BLOQUES[i] != NULL) {
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

	liberarPunteroDePunterosAChar(registros);
	free(registros);
	free(rutaArchBloque);
	free(archivo);
}

t_list* buscarBinariosByNombreTabla(char* nombreTabla) {
	t_list* archivos = buscarArchivos(nombreTabla);
	bool isBin(char* rutaArchivoActual) {
		char* extension = obtenerExtensionDeArchivoDeUnaRuta(rutaArchivoActual);
		return strcmp(extension, "bin") == 0;
	}
	list_destroy(archivos);
	return list_filter(archivos, (void*) isBin);
}

/*levanta los registros de un path (binario o temporal)
 y te los cargar en la lista que le pases*/
void agregarRegistrosFromBloqueByPath(char* path, t_list* listaRegistros) {
	t_archivo* archivo = malloc(sizeof(t_archivo));
	leerArchivoDeTabla(path, archivo);
	int i = 0;
	log_info(logger, "Leyendo registros de %s... ", path);
	while (archivo->BLOQUES[i] != NULL) {

		char* rutaArchivoBloque = string_new();
		strcpy(rutaArchivoBloque, rutas.Bloques);
		string_append_with_format(&rutaArchivoBloque, "%s.bin", archivo->BLOQUES[i]);
		///home/utnso/tp-2019-1c-Los-Sisoperadores/LissandraFileSystem/FS_LISSANDRA/Bloques/48.bin
		t_list* registrosObtenidos = obtenerRegistrosFromBloque(rutaArchivoBloque);
		list_add_all(listaRegistros, registrosObtenidos);
		//borro la referecia a registros obtenidos ya que ya estan cargados en listaRegistros
		list_destroy(registrosObtenidos);
		free(rutaArchivoBloque);
		i++;
	}

	free(archivo);
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
	int tamanioArchBloque = tamanioArchivo(archivoBloque);
	int fileDescriptor = fileno(archivoBloque);

	//mapeamos el archivo a memoria
	char * archivoMapeado;
	if ((archivoMapeado = mmap(NULL, tamanioArchBloque, PROT_READ, MAP_SHARED, fileDescriptor, 0)) == MAP_FAILED) {
		logErrorAndExit("Error al hacer mmap al levantar archivo de bloque");
	}

	//cargo la lista con los registros obtenidos
	char ** registros = string_split(archivoMapeado, "\n");
	int j = 0;
	while (registros[j] != NULL) {
		char* registroActual = registros[j];
		char** valores = string_split(registroActual, ";");
		t_registro* registro = registro_new(valores);
		list_add(listaRegistros, registro);
		j++;
	}

	//fijate que este free no se este ya liberando con el free de valores
	freePunteroAPunteros(registros);
	munmap(archivoMapeado, tamanioArchBloque);
	fclose(archivoBloque);
	close(fileDescriptor);
	return listaRegistros;
}

void cambiarExtensionTemporales(t_list* listaTemporalesTmp) {

	void cambiarExtensionATmpc(char* rutaTmpActual) {
		char* rutaTmpAux = string_duplicate(rutaTmpActual);
		string_append(&rutaTmpActual, "c");
		rename(rutaTmpAux, rutaTmpActual);
		free(rutaTmpAux);
	}

	list_iterate(listaTemporalesTmp, (void*) cambiarExtensionATmpc);

}

/*devuelve los paths en donde se encuentran los temporales*/
t_list* buscarTemporalesByNombreTabla(char* nombreTabla) {
	t_list* archivos = buscarArchivos(nombreTabla);

	bool isTemporal(char* rutaArchivoActual) {
		char* extension = obtenerExtensionDeArchivoDeUnaRuta(rutaArchivoActual);
		return strcmp(extension, "tmp") == 0;
	}
	list_destroy(archivos);
	return list_filter(archivos, (void*) isTemporal);

}
