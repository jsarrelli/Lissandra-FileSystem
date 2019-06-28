/*
 * Compactador.c
 *
 *  Created on: 23 abr. 2019
 *      Author: utnso
 */

#include "Compactador.h"

void compactarTabla(char*nombreTabla){
	t_list* listaTmpc = list_create();
	t_list* listaRegistros = list_create();
	char*rutaTabla = malloc(100);
	armarRutaTabla(rutaTabla, nombreTabla);
	char** archivos = buscarArchivos(rutaTabla);

	cambiarExtensionTemporales(archivos, listaTmpc);
	puts("Se cambio la extension de los temporales");
	log_info(logger,"Se cambio la extension de los temporales");
	//list_iterate2(listaTmpc, (void*) agregarRegistrosDeTmpc, listaRegistros);

}

void agregarRegistrosDeTmpc(char* rutaTmpc, t_list* listaRegistros){
	t_archivo* archivo = malloc(sizeof(t_archivo));
	int res = leerArchivoDeTabla(rutaTmpc, archivo);
	int i=0;

	while(archivo->BLOQUES[i] !=NULL){

	}
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
	string_append_with_format(rutaNueva, "%s.%s", obtenerNombreDeArchivoDeUnaRuta(rutaVieja), extensionNueva);

	if (rename(rutaVieja, rutaNueva) == 0) {
		list_add(listaTmpc, rutaNueva);
		return 1;
	} else {
		puts("No se pudo renombrar el archivo temporal");
		log_error(loggerError, "No se pudo renombrar el archivo temporal");
		return 0;
	}


}
