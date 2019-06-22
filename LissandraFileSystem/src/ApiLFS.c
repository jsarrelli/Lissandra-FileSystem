/*
 * ApiLFS.c
 *
 *  Created on: 23 abr. 2019
 *      Author: utnso
 */

#include "LissandraFileSystem.h"
#include "funcionesLFS.h"
#include "FileSystem.h"

void consolaLFS() {
	puts("Bienvenido a la consola de LFS");
	while (1) {
		puts("Ingrese un comando:");
		char *linea = readline(">");

		if (!strcmp(linea, "")) {
			free(linea);
			continue;
		}
		procesarInput(linea);
		free(linea);
	}
}

void procesarInput(char* consulta) {
	char** comandos = string_n_split(consulta, 2, " ");
	char* operacion = comandos[0];
	char* argumentos = comandos[1];

	if (strcmp(operacion, "SELECT") == 0) {
		//return procesarSELECT(argumentos);
	} else if (strcmp(operacion, "INSERT") == 0) {
		return consolaInsert(argumentos);
	} else if (strcmp(operacion, "CREATE") == 0) {
		return consolaCreate(argumentos);
	} else if (strcmp(operacion, "DESCRIBE") == 0) {
		return consolaDescribe(argumentos);
	} else if (strcmp(operacion, "DROP") == 0) {
		return consolaDrop(argumentos);
	} else {
		puts("Comando no encontrado");
	}

}

void consolaCreate(char*argumentos) {
	char** valores = string_split(argumentos, " ");
	char* nombreTabla = valores[0];
	char* consistenciaChar = valores[1];
	char* cantParticiones = valores[2];
	char* tiempoCompactacion = valores[3];

	funcionCREATE(nombreTabla, cantParticiones, consistenciaChar, tiempoCompactacion);
}

void consolaDescribe(char* nombreTabla) {

	if (nombreTabla!=NULL&&existeTabla(nombreTabla)) {
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
}
