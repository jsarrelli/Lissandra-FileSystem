/*
 * funcionesLFS.c
 *
 *  Created on: 23 abr. 2019
 *      Author: utnso
 */

#include "funcionesLFS.h"

int cantTablas = 0;

char * obtenerRutaTablaSinArchivo(char * rutaTabla){
	char ** directorios;
	char * archivo = malloc(50);
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
	char* rutaTabla = malloc(100);
	DIR* tablaActual;
	armarRutaTabla(rutaTabla, nombreTabla);
	tablaActual = opendir(rutaTabla);

			if(tablaActual !=NULL){
				closedir(rutaTabla);
				return 1;
			}



	free(rutaTabla);
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
	cantTablas += 1;
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
	armarRutaTabla(rutaTabla, nombreTabla);
	string_append(&rutaTabla, "Metadata");
	FILE*arch = fopen(rutaTabla, "w+");
	fprintf(arch, "CONSISTENCIA=%s\nPARTICIONES=%s\nTIEMPO_COMPACTACION=%s\n", consistencia, cantidadParticiones, tiempoCompactacion);

	fclose(arch);
	log_info(logger, "Metadata de %s creada", nombreTabla);


}

void mostrarMetadataTabla(char* nombreTabla){

	char* rutaTabla=malloc(100);
	armarRutaTabla(rutaTabla, nombreTabla);
	string_append(&rutaTabla, "Metadata");
	t_config* configMetadata = config_create(rutaTabla);
	t_metadata_tabla* metadataTabla = malloc(sizeof (t_metadata_tabla));

	printf("\nMetadata de %s: \n", nombreTabla);

	metadataTabla->CONSISTENCIA = config_get_string_value(configMetadata, "CONSISTENCIA");
	metadataTabla->CANT_PARTICIONES = config_get_int_value(configMetadata, "PARTICIONES");
	metadataTabla->T_COMPACTACION = config_get_int_value(configMetadata, "TIEMPO_COMPACTACION");

	printf("CONSISTENCIA: %s\nPARTICIONES=%i\nTIEMPO_COMPACTACION=%i\n\n", metadataTabla->CONSISTENCIA, metadataTabla->CANT_PARTICIONES, metadataTabla->T_COMPACTACION);
	free(metadataTabla);
	free(rutaTabla);
	config_destroy(configMetadata);
}

char* armarRutaTabla(char* rutaTabla, char* nombreTabla){

		strcpy(rutaTabla, rutas.Tablas);
		string_append(&rutaTabla, nombreTabla);
		string_append(&rutaTabla, "/");

		return rutaTabla;
}

/*void mostrarMetadataTodasTablas(){
	char* nombreTabla = malloc(20);
	int i=1;

	while(i <= cantTablas){
		string_append_with_format(&nombreTabla, "TABLA%d", i);
		mostrarMetadataTabla(nombreTabla);
		nombreTabla = NULL;
		i++;
	}

	free(nombreTabla);
}*/

