/*
 * funcionesLFS.c
 *
 *  Created on: 23 abr. 2019
 *      Author: utnso
 */

#include "funcionesLFS.h"

char * obtenerRutaTablaSinArchivo(char * rutaTabla){
	char ** directorios;
	char * archivo;
	int tamanioNombreArchivo;
	int tamanioRuta;

	directorios = string_split(rutaTabla, "/");
	archivo = obtenerUltimoElementoDeUnSplit(directorios);
	tamanioNombreArchivo = strlen(archivo) +1;
	tamanioRuta = strlen(rutaTabla);

	liberarPunteroDePunterosAChar(directorios);
	free(directorios);
	free(archivo);
	return string_substring_until(rutaTabla,tamanioRuta-tamanioNombreArchivo);
}

int existeTabla(char* nombreTabla){
		int i;
		int tamanioListaRutasTablas = list_size(listaNombresTablas);
		char* elemento;
		for (i=0; i< tamanioListaRutasTablas; i++){
		strcpy(elemento, list_get(listaNombresTablas, i));
			if(string_equals_ignore_case(nombreTabla, elemento)){
				return 1;
			}
		}

		return 0;
}

void crearTablaYParticiones(char* nombreTabla, int cantidadParticiones){

	char* rutaTabla;
	int i = 0;
	string_append(&rutaTabla, "/home/utnso/tp-2019-1c-Los-Sisoperadores/LissandraFileSystem/Tablas");
	string_append(&rutaTabla,"/");
	string_append(&rutaTabla, nombreTabla);
	mkdir(rutaTabla,0777);
	puts("Tabla creada");
	FILE* archivo;


	for (i=0; i< cantidadParticiones; i++){
		string_append(&rutaTabla,"/");
		string_append_with_format(&rutaTabla, "%d.bin", i);
		archivo = fopen(rutaTabla, "w+");
		printf("Particion %d creada", i);
		rutaTabla = obtenerRutaTablaSinArchivo(rutaTabla);
	}
}



