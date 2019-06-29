/*
 * Compactador.c
 *
 *  Created on: 23 abr. 2019
 *      Author: utnso
 */

#include "Compactador.h"

void compactarTabla(char*nombreTabla){
	t_list* archivosBin =list_create();
	char* nombreFile = string_new();
	char* rutaBinario = string_new();
	int i;
	t_list* listaTmpc = list_create();
	t_list* listaRegistros = list_create();

	char*rutaTabla = malloc(100);
	armarRutaTabla(rutaTabla, nombreTabla);
	char** archivos = buscarArchivos(rutaTabla);
	buscarBinarios(archivos, archivosBin);
	int tamanioListaBinarios = list_size(archivosBin);

	for(i=0; i<tamanioListaBinarios; i++){
		sprintf(nombreFile, "particion%d", i);
		rutaBinario = list_get(archivosBin, i);
		FILE* nombreFile = fopen(rutaBinario, "wb");
	}
	puts("Archivos de particiones abiertos");
	cambiarExtensionTemporales(archivos, listaTmpc);
	puts("Se cambio la extension de los temporales");
	log_info(logger,"Se cambio la extension de los temporales");
	list_iterate2(listaTmpc, (void*) agregarRegistrosDeTmpc, listaRegistros);
	//list_iterate(archivosBin, (void*) abrirParticiones);
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
