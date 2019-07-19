/*
 * ApiLFS.c
 *
 *  Created on: 23 abr. 2019
 *      Author: utnso
 */

#include "LissandraFileSystem.h"
#include "funcionesLFS.h"
#include "FileSystem.h"

void* consolaLFS() {
	char * consulta;
	while (1) {

		puts("\nIngrese comando a ejecutar. Para salir presione enter");
		consulta = readline(">");
		if (consulta == NULL || strlen(consulta) == 0) {
			free(consulta);
			break;
		} else {
			add_history(consulta);
			procesarInput(consulta);
			free(consulta);
		}

	}
	log_info(loggerInfo, "Fin de leectura por consola");
	return NULL;
}

void procesarInput(char* consulta) {
	char** comandos = string_n_split(consulta, 2, " ");
	char* operacion = comandos[0];
	char* argumentos = comandos[1];

	if (strcmp(operacion, "SELECT") == 0) {
		consolaSelect(argumentos);
	} else if (strcmp(operacion, "INSERT") == 0) {
		consolaInsert(argumentos);
	} else if (strcmp(operacion, "CREATE") == 0) {
		consolaCreate(argumentos);
	} else if (strcmp(operacion, "DESCRIBE") == 0) {
		consolaDescribe(argumentos);
	} else if (strcmp(operacion, "DROP") == 0) {
		consolaDrop(argumentos);
	} else {
		puts("Comando no encontrado");
	}

	freePunteroAPunteros(comandos);

}

void consolaCreate(char*argumentos) {
	char** valores = string_split(argumentos, " ");
	if(cantidadParametros(valores) == 3){
	char* nombreTabla = valores[0];
	char* consistenciaChar = valores[1];
	char* cantParticiones = valores[2];
	char* tiempoCompactacion = valores[3];

	funcionCREATE(nombreTabla, cantParticiones, consistenciaChar, tiempoCompactacion);
	freePunteroAPunteros(valores);
	} else{
		puts("\nError en la cantidad de parametros. El comando ingresado necesita 4.\n");
		freePunteroAPunteros(valores);
	}
}

void consolaDescribe(char* nombreTabla) {

	if (nombreTabla != NULL && existeTabla(nombreTabla)) {
		funcionDESCRIBE(nombreTabla);
	} else
		funcionDESCRIBE_ALL();
}

void consolaDrop(char* nombreTabla) {
	funcionDROP(nombreTabla);
}

void consolaInsert(char* argumentos) {

	char** valores = string_split(argumentos, "\""); //34 son las " en ASCII
	char** valoresAux = string_split(valores[0], " ");
	char* nombreTabla = valoresAux[0];
	char* key = valoresAux[1];
	char* value = valores[1];
	double timeStamp;
	if (valores[2] == NULL) {
		timeStamp = getCurrentTime();
	} else {
		timeStamp = atof(valores[2]);
	}

	funcionINSERT(timeStamp, nombreTabla, key, value);

	freePunteroAPunteros(valoresAux);
	freePunteroAPunteros(valores);

}


void consolaSelect(char*argumentos){
	char** valores = string_split(argumentos, " ");
	if(cantidadParametros(valores) == 1){
	char* nombreTabla = valores[0];
	char* key = valores[1];
	int keyActual = atoi(key);

	t_registro* registro=funcionSELECT(nombreTabla, keyActual);
	if(registro!=NULL){

		freeRegistro(registro);
	}
	freePunteroAPunteros(valores);
	}else{
		puts("\nError en la cantidad de parametros. El comando ingresado necesita 2.\n");
		freePunteroAPunteros(valores);
	}
}
