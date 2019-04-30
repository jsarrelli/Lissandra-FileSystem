/*
 * ApiLFS.c
 *
 *  Created on: 23 abr. 2019
 *      Author: utnso
 */

#include "LissandraFileSystem.h"
#include "lib/funcionesLFS.h"

void consolaLFS(){
	puts("Bienvenido a la consola. Ingrese un comando:");
	while(1){
		char *linea = readline(">");
		//esto se hace por que si el tipo apreta enter
		//rompe todo
		if(!strcmp(linea, "")){
			free(linea);
			continue;
		}
		procesarInput(linea);
		free(linea);
	}
}

void procesarInput(char* linea) {
	int cantidad;
	char **palabras = string_split(linea, " ");
	cantidad = cantidadParametros(palabras);
	if (!strcmp(*palabras, "INSERT")) {
		//consolaInsert(palabras, cantidad);
	} else if (!strcmp(*palabras, "")) {
		//consolaRemove(palabras,cantidad);
	} else if (!strcmp(*palabras, "SELECT")) {
		//consolaSelect(palabras,cantidad);
	} else if (!strcmp(*palabras, "CREATE")) {
		consolaCreate(palabras,cantidad);
	} else if (!strcmp(*palabras, "DESCRIBE")) {
		//consolaDescribe(palabras,cantidad);
	} else if (!strcmp(*palabras, "DROP")) {
		//consolaDrop(palabras,cantidad);
	} else if(!strcmp(*palabras, "exit")){
		printf("Finalizando consola\n");



	} else {
		puts("El comando no existe.");
	}
	liberarPunteroDePunterosAChar(palabras);
	free(palabras);

}


void consolaCreate(char**palabras, int cantidad){
	if(cantidad == 4){

			if(existeTabla(palabras[1])){
				puts("Ya existe una tabla con ese nombre.");
			}
			else{


				crearTablaYParticiones(palabras[1], palabras[3]);
				crearMetadataTabla(palabras[1],palabras[2], palabras[3], palabras[4]);

			}
	}
	else{
		puts("Error en la cantidad de parametros.");
	}
}
