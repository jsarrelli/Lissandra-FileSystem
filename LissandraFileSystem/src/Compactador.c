/*
 * Compactador.c
 *
 *  Created on: 23 abr. 2019
 *      Author: utnso
 */

#include "Compactador.h"

void compactarTabla(char*nombreTabla) {
	t_metadata_tabla metadata = obtenerMetadata(nombreTabla);
	char* registro = string_new();
	int  resto;

	t_list* registrosNuevos = list_create();

	t_list* listaRegistrosViejos = list_create();
	t_list* particionesDeRegistrosNuevos = list_create();

	log_info(logger, "Obteniendo archivos binarios..");
	t_list* archivosBinarios = buscarBinariosByTabla(nombreTabla);
	int tamanioListaBinarios = list_size(archivosBinarios);

	log_info(logger, "Modificando tmp a tmpc..");
	t_list* archivosTemporales = buscarTemporalesByNombreTabla(nombreTabla);
	cambiarExtensionTemporales(archivosTemporales);
	log_info(logger, "Se cambio la extension de los temporales");

	list_iterate2(archivosTemporales, (void*) agregarRegistrosDeTmpc, registrosNuevos);
	log_info(logger, "Lectura de registros de los tmpc finalizada");
	//aca filtramos los registros nuevos

	/*aca vamos a crear una lista de particiones
	 * y a su vez en cada particion vamos a guardar una lista de los registros nuevos que corresponden a cada una*/
	for (int i = 0; i < tamanioListaBinarios; i++) {
		t_list* particionRegistrosNuevos = list_create();
		list_add(particionesDeRegistrosNuevos, particionRegistrosNuevos);
	}
	/////////////////////////
	puts("Archivos de particiones abiertos");

	list_iterate2(archivosBin, (void*) agregarRegistrosDeBin, listaRegistrosViejos);
	list_add_all(registrosNuevos, listaRegistrosViejos);
	int tamanioListaRegistros = list_size(registrosNuevos);

	for (int j = 0; j < tamanioListaRegistros; j++) {
		registro = list_get(registrosNuevos, j);
		char** reg = string_split(registro, ";");
		resto = (atoi(reg[1])) % metadata.CANT_PARTICIONES;
		list_add(list_get(particionesDeRegistrosNuevos, resto), registro);
	}
	list_destroy(registrosNuevos);

	persistirParticionesDeTabla(particionesDeRegistrosNuevos, archivosBin);
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

int escribirEnBin(t_list* lista, char*rutaBinario) {
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

t_list* buscarBinariosByTabla(char* nombreTabla) {
	t_list* archivos = buscarArchivos(nombreTabla);
	bool isBin(char* rutaArchivoActual) {
		char* extension = obtenerExtensionDeArchivoDeUnaRuta(rutaArchivoActual);
		return strcmp(extension, "bin") == 0;
	}
	list_destroy(archivos);
	return list_filter(archivos, (void*)isBin);
}

void agregarRegistrosDeTmpc(char* rutaTmpc, t_list* listaRegistros) {
	t_archivo* archivo = malloc(sizeof(t_archivo));
	leerArchivoDeTabla(rutaTmpc, archivo);
	int i = 0;
	log_info(logger, "Leyendo registros de %s... ",rutaTmpc);
	while (archivo->BLOQUES[i] != NULL) {
		char * archivoMapeado;
		char* rutaArchivoBloque = string_new();
		strcpy(rutaArchivoBloque, rutas.Bloques);
		string_append_with_format(&rutaArchivoBloque, "%s.bin", archivo->BLOQUES[i]);

		///home/utnso/tp-2019-1c-Los-Sisoperadores/LissandraFileSystem/FS_LISSANDRA/Bloques/48.bin

		FILE* archivoBloque = fopen(rutaArchivoBloque, "rb");
		int tamanioArchBloque = tamanioArchivo(archivoBloque);
		int fileDescriptor = fileno(archivoBloque);

		//mapeamos el archivo a memoria
		if ((archivoMapeado = mmap(NULL, tamanioArchBloque, PROT_READ, MAP_SHARED, fileDescriptor, 0)) == MAP_FAILED) {
			logErrorAndExit("Error al hacer mmap al levantar archivo de bloque");
		}

		//cargo la lista con los registros obtenidos
		char ** registros = string_split(archivoMapeado, "\n");
		int j = 0;
		while (registros[j] != NULL) {
			list_add(listaRegistros, registros[j]);
			j++;
		}

		munmap(archivoMapeado, tamanioArchBloque);
		fclose(archivoBloque);
		free(rutaArchivoBloque);
		freePunteroAPunteros(registros);
		close(fileDescriptor);
		i++;
	}

	free(archivo);
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

t_list* buscarTemporalesByNombreTabla(char* nombreTabla) {
	t_list* archivos = buscarArchivos(nombreTabla);

	bool isTemporal(char* rutaArchivoActual) {
		char* extension = obtenerExtensionDeArchivoDeUnaRuta(rutaArchivoActual);
		return strcmp(extension, "tmp") == 0;
	}
	list_destroy(archivos);
	return list_filter(archivos, (void*)isTemporal);

}
