/*
 ============================================================================
 Name        : Compartidas.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <Compartidas.h>

void inicializarArchivoDeLogs(char * ruta){
	FILE * archivo = fopen(ruta, "w");
	fclose(archivo);
}

void logErrorAndExit(char * mensaje){
	log_error(loggerError,mensaje);
	exit(-1);
}

void crearHilo(pthread_t * nombreHilo, void * nombreFuncion, void * parametros){
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	if(pthread_create(nombreHilo, &attr, nombreFuncion, parametros) < 0){
			logErrorAndExit("No se pudo crear el hilo.");
	}
}

void liberarPunteroDePunterosAChar(char** palabras){
	int i = 0;
	while(palabras[i] != NULL){
		free(palabras[i]);
		i++;
	}

}
