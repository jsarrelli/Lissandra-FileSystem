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
		//procesarInput(linea);
		free(linea);
	}
}

/*
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
		//consolaMove(palabras,cantidad);
	} else if (!strcmp(*palabras, "DESCRIBE")) {
		//consolaDescribe(palabras,cantidad);
	} else if (!strcmp(*palabras, "DROP")) {
		//consolaDrop(palabras,cantidad);
	} else if(!strcmp(*palabras, "exit")){
		printf("Finalizando consola\n");
		liberarPunteroDePunterosAChar(palabras);
		free(palabras);
		free(linea);
		pthread_kill(pthread_self(),SIGKILL);
	} else {
		puts("El comando no existe.");
	}
	liberarPunteroDePunterosAChar(palabras);
	free(palabras);

}
*/
