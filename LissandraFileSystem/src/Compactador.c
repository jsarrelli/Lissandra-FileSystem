/*
 * Compactador.c
 *
 *  Created on: 23 abr. 2019
 *      Author: utnso
 */

#include "Compactador.h"

void compactarTabla(char*nombreTabla){
	t_metadata_tabla metadata = obtenerMetadata(nombreTabla);
	t_list* archivosBin =list_create();
	char* nombLista = string_new();
	char* registro = string_new();
	int i,j, resto;

	t_list* listaTmpc = list_create();
	t_list* listaRegistros = list_create();
	t_list* listaRegistrosViejos = list_create();
	t_list* listaListasParticiones=list_create();
	char*rutaTabla = malloc(100);
	armarRutaTabla(rutaTabla, nombreTabla);
	char** archivos = buscarArchivos(rutaTabla);
	buscarBinarios(archivos, archivosBin);
	int tamanioListaBinarios = list_size(archivosBin);

	for(i=0; i<tamanioListaBinarios; i++){
		sprintf(nombLista, "lista%d", i);
		t_list* nombLista = list_create();
		list_add(listaListasParticiones, nombLista);
	}
	puts("Archivos de particiones abiertos");
	cambiarExtensionTemporales(archivos, listaTmpc);
	puts("Se cambio la extension de los temporales");
	log_info(logger,"Se cambio la extension de los temporales");
	list_iterate2(listaTmpc, (void*) agregarRegistrosDeTmpc, listaRegistros);
	list_iterate2(archivosBin, (void*) agregarRegistrosDeBin, listaRegistrosViejos);
	list_add_all(listaRegistros, listaRegistrosViejos);
	int tamanioListaRegistros = list_size(listaRegistros);

	for(j=0;j<tamanioListaRegistros; j++){
		registro = list_get(listaRegistros,j);
		char** reg = string_split(registro, ";");
		resto = (atoi(reg[1]))%metadata.CANT_PARTICIONES;
		list_add(list_get(listaListasParticiones, resto), registro);
	}
	list_destroy(listaRegistros);

	persistirParticionesDeTabla(listaListasParticiones, archivosBin);
}
void persistirParticionesDeTabla(t_list* listaListas, t_list*archivosBin){
	char*rutaBinario=string_new();
	int i;
	t_list*lista;
	for(i=0; i<list_size(archivosBin);i++){
		rutaBinario = list_get(archivosBin, i);
		lista = list_get(listaListas, i);

		escribirEnBin(lista, rutaBinario);
	}
}

int escribirEnBin(t_list* lista, char*rutaBinario){
	int*bloques;
	t_archivo*archivo= malloc(sizeof(t_archivo));
	leerArchivoDeTabla(rutaBinario, archivo);
	char *rutaBloque = malloc(150);
	char **arrBloques = malloc(40);
	char*reg= string_new();
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
		while (tamanioArchBloque + (strlen(reg) + 1) < metadata.BLOCK_SIZE && j<list_size(lista)) {
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

			reg= list_get(lista,j++);
		}
		fclose(archivoBloque);
		archivo->TAMANIO += tamanioArchBloque;
		rutaBloque = obtenerRutaTablaSinArchivo(rutaBloque);
		string_append(&rutaBloque, "/");
		i++;
	}

	return 1;
}
void agregarRegistrosDeBin(char* rutaBinario, t_list* listaRegistrosViejos){
	t_archivo* archivo = malloc(sizeof(t_archivo));
	leerArchivoDeTabla(rutaBinario, archivo);
	int i=0, j;
	char* rutaArchBloque = malloc(100);
	strcpy(rutaArchBloque, rutas.Bloques);
	int tamanioArchBloque, fd;
	FILE* archivoBloque;

	char * archivoMapeado;
	char ** registros;

	while(archivo->BLOQUES[i] !=NULL){
		string_append_with_format(&rutaArchBloque, "%s.bin", archivo->BLOQUES[i]);
		archivoBloque = fopen(rutaArchBloque, "rb");
		tamanioArchBloque = tamanioArchivo(archivoBloque);
		fd = fileno(archivoBloque);

		if ((archivoMapeado = mmap(NULL,tamanioArchBloque, PROT_READ, MAP_SHARED,fd, 0)) == MAP_FAILED) {
				logErrorAndExit("Error al hacer mmap al levantar archivo de bloque");
			}
			fclose(archivoBloque);
			close(fd);

			registros = string_split(archivoMapeado,"\n");
			int cant= contarPunteroDePunteros(registros);
			for(j=0;j<cant; j++){
				list_add(listaRegistrosViejos, registros[j]);
			}


			munmap(archivoMapeado,tamanioArchBloque);


			rutaArchBloque = obtenerRutaTablaSinArchivo(rutaArchBloque);
			i++;
	}

	liberarPunteroDePunterosAChar(registros);
	free(registros);
	free(rutaArchBloque);
	free(archivo);
}

void buscarBinarios(char** archivos, t_list* archivosBin){
	int i =0;
	char*extension = string_new();

		while(archivos[i]!=NULL){
			extension = obtenerExtensionDeArchivoDeUnaRuta(archivos[i]);
			if(strcmp(extension, "bin") ==0){
				list_add(archivosBin, archivos[i]);

			}
		}
}

void agregarRegistrosDeTmpc(char* rutaTmpc, t_list* listaRegistros){
	t_archivo* archivo = malloc(sizeof(t_archivo));
	leerArchivoDeTabla(rutaTmpc, archivo);
	int i=0, j;
	char* rutaArchBloque = malloc(100);
	strcpy(rutaArchBloque, rutas.Bloques);
	int tamanioArchBloque, fd;
	FILE* archivoBloque;

	char * archivoMapeado;
	char ** registros;

	while(archivo->BLOQUES[i] !=NULL){
		string_append_with_format(&rutaArchBloque, "%s.bin", archivo->BLOQUES[i]);
		archivoBloque = fopen(rutaArchBloque, "rb");
		tamanioArchBloque = tamanioArchivo(archivoBloque);
		fd = fileno(archivoBloque);

		if ((archivoMapeado = mmap(NULL,tamanioArchBloque, PROT_READ, MAP_SHARED,fd, 0)) == MAP_FAILED) {
				logErrorAndExit("Error al hacer mmap al levantar archivo de bloque");
			}
			fclose(archivoBloque);
			close(fd);

			registros = string_split(archivoMapeado,"\n");
			int cant= contarPunteroDePunteros(registros);
			for(j=0;j<cant; j++){
				list_add(listaRegistros, registros[j]);
			}


			munmap(archivoMapeado,tamanioArchBloque);


			rutaArchBloque = obtenerRutaTablaSinArchivo(rutaArchBloque);
			i++;
	}

	liberarPunteroDePunterosAChar(registros);
	free(registros);
	free(rutaArchBloque);
	free(archivo);
}

void cambiarExtensionTemporales(char** archivos, t_list* listaTmpc){
	int i =0;
	char*extension = string_new();

	while(archivos[i]!=NULL){
		extension = obtenerExtensionDeArchivoDeUnaRuta(archivos[i]);
		if(strcmp(extension, "tmp") ==0){
			cambiarExtension(archivos[i], "tmpc", listaTmpc);

		}
	}
}

int cambiarExtension(char* rutaVieja, char* extensionNueva, t_list* listaTmpc){
	char* rutaNueva = malloc(100);
	strcpy(rutaNueva, obtenerRutaTablaSinArchivo(rutaVieja));
	string_append_with_format(&rutaNueva, "%s.%s", obtenerNombreDeArchivoDeUnaRuta(rutaVieja), extensionNueva);

	if (rename(rutaVieja, rutaNueva) == 0) {
		list_add(listaTmpc, rutaNueva);
		return 1;
	} else {
		puts("No se pudo renombrar el archivo temporal");
		log_error(loggerError, "No se pudo renombrar el archivo temporal");
		return 0;
	}


}
