
#include "Libraries.h"


int get_campo_config_int(t_config* archivo_configuracion, char* nombre_campo) {
	int valor;
	if(config_has_property(archivo_configuracion, nombre_campo)){
		valor = config_get_int_value(archivo_configuracion, nombre_campo);
		printf("El %s es: %i\n", nombre_campo, valor);
		return valor;
	}
	return NULL;
}


char* get_campo_config_string(t_config* archivo_configuracion, char* nombre_campo) {
	char* valor;
	if(config_has_property(archivo_configuracion, nombre_campo)){
		valor = config_get_string_value(archivo_configuracion, nombre_campo);
		printf("El %s es: %s\n", nombre_campo, valor);
		return valor;
	}
	return NULL;
}


char** get_campo_config_array(t_config* archivo_configuracion, char* nombre_campo) {
	char** valor;
	if(config_has_property(archivo_configuracion, nombre_campo)){
		valor = config_get_array_value(archivo_configuracion, nombre_campo);
		printf("El %s es: %s\n", nombre_campo, valor);
		return valor;
	}
	return NULL;
}

void logErrorAndExit(char * mensaje){
	log_error(loggerError,mensaje);
	exit(-1);
}

void inicializarArchivoDeLogs(char * ruta){
	FILE * archivo = fopen(ruta, "w");
	fclose(archivo);
}

int contarPunteroDePunteros(char ** puntero){
	char ** aux = puntero;
	int contador = 0;
	while(*aux != NULL){
		contador++;
		aux++;
	}
	return contador;
}

void liberarPunteroDePunterosAChar(char** palabras){
	int i = 0;
	while(palabras[i] != NULL){
		free(palabras[i]);
		i++;
	}

}


void crearHilo(pthread_t * nombreHilo, void * nombreFuncion, void * parametros){
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	if(pthread_create(nombreHilo, &attr, nombreFuncion, parametros) < 0){
			logErrorAndExit("No se pudo crear el hilo.");
	}
}

int cantidadParametros(char ** palabras){
	int i = 1;
	while(palabras[i] != NULL){
		i++;
	}
	return i-1;
}

char * obtenerUltimoElementoDeUnSplit(char ** palabras){
	char * ultimaPalabra;
	int i = 0;
	while(palabras[i] != NULL){
		ultimaPalabra = palabras[i];
		i++;
	}
	return strdup(ultimaPalabra);
}


