/*
 * API-Kernel.c
 *
 *  Created on: 26 may. 2019
 *      Author: utnso
*
*/

#include "API_kernel.h"
#include "kernel.h"
#include "Libraries.h"

void consolaKernel(){
	puts("Bienvenido a la consola del Kernel. Ingrese un comando:");
	while(1){
		char *linea = readline(">");

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
		//	INSERT [NOMBRE_TABLA] [KEY] “[VALUE]”

		consolaInsert(palabras, cantidad);
	} else if (!strcmp(*palabras, "SELECT")) {
		//	SELECT [NOMBRE_TABLA] [KEY]

		consolaSelect(palabras,cantidad);
	} else if (!strcmp(*palabras, "CREATE")) {
		//	CREATE [TABLA] [TIPO_CONSISTENCIA] [NUMERO_PARTICIONES] [COMPACTION_TIME]

		//consolaCreate(palabras,cantidad);
	} else if (!strcmp(*palabras, "DESCRIBE")) {
		// DESCRIBE [NOMBRE_TABLA]
		// DESCRIBE

		//consolaDescribe(palabras,cantidad);
	} else if (!strcmp(*palabras, "DROP")) {
		//	DROP [NOMBRE_TABLA]

		//consolaDrop(palabras,cantidad);
	} else if (!strcmp(*palabras, "ADD")) {
		//	DROP [NOMBRE_TABLA]

		consolaAdd(palabras,cantidad);
	} else if(!strcmp(*palabras, "exit")){
		printf("Finalizando consola\n");

	} else {
		puts("El comando no existe.");
	}
	liberarPunteroDePunterosAChar(palabras);
	free(palabras);

}

//	INSERT [NOMBRE_TABLA] [KEY] “[VALUE]”
void consolaInsert(char**palabras, int cantidad){
	if(cantidad == 4 || cantidad == 3){
			int memDestino = obtenerMemDestino(palabras[1],(int)palabras[2]);
			if(memDestino != -1){

				log_info(logger, "mem destino obtenida");
				printf("\nLa mem destino es %d \n",memDestino);

			} else{
				printf("Tabla no existe o no asociada a ningun criterio.");
			}
	}

	else{
		puts("Error en la cantidad de parametros.");
	}
}
//	SELECT [NOMBRE_TABLA] [KEY]
void consolaSelect(char**palabras, int cantidad){
	if(cantidad == 2){
			int memDestino = obtenerMemDestino(palabras[1],atoi(palabras[2]));
			if(memDestino != -1){

				//log_info(logger, "mem destino obtenida");
				printf("\nLa mem destino es %d \n",memDestino);

			} else{
				printf("No se pudo obtener una memoria. Asocie alguna a un criterio según la consistencia de la tabla.\n");
			}
	}

	else{
		puts("Error en la cantidad de parametros.");
	}
}
//ADD MEMORY 1 TO SC
void consolaAdd(char**palabras, int cantidad){
	if(cantidad == 4){
		add(atoi(palabras[2]),palabras[4]);
	}else{
		puts("Error en la cantidad de parametros.");
	}
}
/*

void consolaCreate(char**palabras, int cantidad){
	if(cantidad == 4){
			char * consistencia;
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

void consolaDescribe(char**palabras, int cantidad){
	if(cantidad == 1){

			if(existeTabla(palabras[1])){
				mostrarMetadataTabla(palabras[1]);

			}else{
				printf("La %s no existe", palabras[1]);
			}

	}else if(cantidad==0){
		mostrarMetadataTodasTablas(rutas.Tablas);
		crearYEscribirArchivosTemporales(rutas.Tablas);


	}
	else{
		puts("Error en la cantidad de parametros.");
	}
}


void consolaDrop(char**palabras, int cantidad){
	if(cantidad == 1){

			if(existeTabla(palabras[1])){
				removerTabla(palabras[1]);
				printf("%s eliminada\n\n", palabras[1]);
			}

	}
	else{
		puts("Error en la cantidad de parametros.");
	}
}
*/



