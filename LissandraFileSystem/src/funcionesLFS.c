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
		int tamanioListaNombresTablas = list_size(listaNombresTablas);
		char* elemento = malloc(10);
		for (i=0; i< tamanioListaNombresTablas; i++){
		strcpy(elemento, list_get(listaNombresTablas, i));
			if(string_equals_ignore_case(nombreTabla, elemento)){
				return 1;
			}
		}
		free(elemento);
		return 0;
}

void crearTablaYParticiones(char* nombreTabla, char* cantidadParticiones){

	char* rutaTabla= malloc(100);
	strcpy(rutaTabla, rutas.Tablas);
	int i=0, j=1;
	int cantPart = atoi(cantidadParticiones);

	string_append(&rutaTabla,"/");
	string_append(&rutaTabla, nombreTabla);
	mkdir(rutaTabla,0777);
	printf("%s creada\n", nombreTabla);
	list_add(listaNombresTablas, nombreTabla);
	log_info(logger, "Se creo la  %s\n", nombreTabla);

	while (i < cantPart){
		string_append(&rutaTabla,"/");
		string_append_with_format(&rutaTabla, "%d.bin", i);
		FILE* archivo = fopen(rutaTabla, "w+");
		int *bloque = buscarBloquesLibres(1);

			if(bloque == NULL){
				logErrorAndExit( "No hay bloques libres para crear el archivo");
			}
		reservarBloque(bloque[0]);

		escribirBitmap();

		fprintf(archivo, "TAMANIO=0\n");

		fprintf(archivo, "BLOQUES=[%i]", bloque[0]);

		fclose(archivo);
		free(bloque);
		printf("Particion %d creada \n", j);
		rutaTabla = obtenerRutaTablaSinArchivo(rutaTabla);
		i++;
		j++;
	}

	free(rutaTabla);
}

void crearMetadataTabla (char*nombreTabla, char* consistencia, char* cantidadParticiones, char* tiempoCompactacion){
	char*rutaTabla=malloc(100);
	strcpy(rutaTabla, rutas.Tablas);
	string_append(&rutaTabla, nombreTabla);
	string_append(&rutaTabla, "/");
	string_append(&rutaTabla, "Metadata");
	FILE*arch = fopen(rutaTabla, "w+");
	fprintf(arch, "CONSISTENCIA=%s\nPARTICIONES=%s\nTIEMPO_COMPACTACION=%s\n", consistencia, cantidadParticiones, tiempoCompactacion);

	fclose(arch);
	log_info(logger, "Metadata de %s creada", nombreTabla);


}

