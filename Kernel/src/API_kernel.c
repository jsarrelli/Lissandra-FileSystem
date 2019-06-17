/*
 * API-Kernel.c
 *
 *  Created on: 26 may. 2019
 *      Author: utnso
 *
 */

#include "API_kernel.h"

//static const int ERROR = -1;

//void consolaKernel(){
//	int limiteIngreso = 0; // Solo para probar
//	puts("\nBienvenido a la consola del Kernel. \nIngrese un comando:");
//	while(limiteIngreso < 5){
//		char *linea = readline(">");
//
//		if(!strcmp(linea, "")){
//			free(linea);
//			continue;
//		}
//		procesarInput(linea);
//		free(linea);
//
//	limiteIngreso++;
//
//	}
//}

void consolaKernel(int socketMemoriaRecibido) {
	socketMemoria = socketMemoriaRecibido;
	char * consulta;
//	while (1) {
	for(int i=0; i<5;i++){

		puts("\nBienvenido a la consola del Kernel. \nIngrese un comando:");
		consulta = readline(">");
		if (consulta) {
			add_history(consulta);
		}
		if (consulta == NULL) {
			return;
		}
		procesarInput(consulta);
		free(consulta);

	}

}

//void procesarInput(char* linea) {
//	int cantidad;
//	char **palabras = string_split(linea, " ");
//	cantidad = cantidadParametros(palabras);
//	if (!strcmp(*palabras, "INSERT")) {
//
//		//	INSERT [NOMBRE_TABLA] [KEY] “[VALUE]”
//		consolaInsert(palabras, cantidad);
//
//	} else if (!strcmp(*palabras, "SELECT")) {
//
//		//	SELECT [NOMBRE_TABLA] [KEY]
//		consolaSelect(palabras, cantidad);
//
//	} else if (!strcmp(*palabras, "CREATE")) {
//		//	CREATE [TABLA] [TIPO_CONSISTENCIA] [NUMERO_PARTICIONES] [COMPACTION_TIME]
//
//		//consolaCreate(palabras,cantidad);
//	} else if (!strcmp(*palabras, "DESCRIBE")) {
//		// DESCRIBE [NOMBRE_TABLA]
//		// DESCRIBE
//
//		//consolaDescribe(palabras,cantidad);
//	} else if (!strcmp(*palabras, "DROP")) {
//		//	DROP [NOMBRE_TABLA]
//
//		//consolaDrop(palabras,cantidad);
//	} else if (!strcmp(*palabras, "ADD")) {
//
//		//	DROP [NOMBRE_TABLA]
//		consolaAdd(palabras, cantidad);
//
//	} else if (!strcmp(*palabras, "exit")) {
//		printf("Finalizando consola\n");
//
//	} else {
//		puts("El comando no existe.");
//	}
//	liberarPunteroDePunterosAChar(palabras);
//	free(palabras);
//
//}

void procesarInput(char* linea) {
	char** comandos = string_n_split(linea, 2, " ");
	char* operacion = comandos[0];
	char* argumentos = comandos[1];

	if (strcmp(operacion, "SELECT") == 0) {
		return consolaSelect(linea);
	} else if (strcmp(operacion, "INSERT") == 0) {
		return consolaInsert(linea);
	} else if (strcmp(operacion, "CREATE") == 0) {
		//return procesarCREATE(argumentos);
	} else if (strcmp(operacion, "DESCRIBE") == 0) {
		//return procesarDESCRIBE(argumentos);
	} else if (strcmp(operacion, "DROP") == 0) {
		//return procesarDROP(argumentos);
	} else if (strcmp(operacion, "JOURNAL") == 0) {
		//journalMemoria();
	} else if (strcmp(operacion, "ADD") == 0) {
		return procesarAdd(linea);
	}else if (strcmp(operacion, "RUN") == 0) {
		return consolaRun(argumentos);
	}
	else {
		puts("Comando no encontrado");
	}

//	free(comandos);
	return;
}

//	INSERT [NOMBRE_TABLA] [KEY] “[VALUE]”
void consolaInsert(char*consulta) {

	char** valores = string_split(consulta, "'"); //34 son las " en ASCII
	char** valoresAux = string_split(valores[0], " ");
	char* nombreTabla = valoresAux[1];
	int key = atoi(valoresAux[2]);
	char* value = valores[1];
	double timeStamp;
	if (valores[2] == NULL) {
		timeStamp = getCurrentTime();
	} else {
		timeStamp = atof(valores[2]);
	}

	int memDestino = obtenerMemDestino(nombreTabla, key);
	if (memDestino != ERROR) {

		log_info(logger, "mem destino obtenida");
		printf("\nLa mem destino es %d \n", memDestino);

		EnviarDatosTipo(socketMemoria, KERNEL, consulta, strlen(consulta), INSERT);

	} else {
		printf("Tabla no existe o no asociada a ningun criterio.");
	}
}

//	SELECT [NOMBRE_TABLA] [KEY]
void consolaSelect(char* consulta) {
	char** valores = string_split(consulta, " ");
	char* nombreTabla = valores[1];
	int key = atoi(valores[2]);

	int memDestino = obtenerMemDestino(nombreTabla, key);
	if (memDestino != ERROR) {

		//log_info(logger, "mem destino obtenida");
		printf("\nLa mem destino es %d \n", memDestino);
		EnviarDatosTipo(socketMemoria, KERNEL, consulta, strlen(consulta), SELECT);


	} else {
		printf("No se pudo obtener una memoria. Asocie alguna a un criterio según la consistencia de la tabla.\n");
	}

}
//ADD MEMORY 1 TO SC
void procesarAdd(char* consulta) {
	char** valores = string_split(consulta, " ");
	add(atoi(valores[1]), valores[3]);
}

void consolaRun(char* consulta){
	comandoRun(consulta);
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

