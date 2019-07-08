/*
 * KernelOtherFunctions.c
 *
 *  Created on: 20 jun. 2019
 *      Author: utnso
 */

#include "KernelOtherFunctions.h"

consistencia procesarConsistencia(char* palabra){
	if(strcmp(palabra, "SC")==0)
		return SC;
	else if(strcmp(palabra, "SHC")==0)
		return SHC;
	else if(strcmp(palabra, "EC")==0)
		return EC;
	return ERROR_CONSISTENCIA;
}

bool verificarCriterio(bool* criterio, consistencia ccia){
	switch(ccia){
		case SC:
			return criterio[0];
			break;
		case SHC:
			return criterio[1];
			break;
		case EC:
			return criterio[2];
		case ERROR_CONSISTENCIA:
			return criterio[3];
	}
	return false;
}

void hardcodearInfoMemorias(){
//	listaMemorias
//	infoMemoria* memoria1= malloc(sizeof(infoMemoria));
////	memoria1->ccia = SC;
//	memoria1->id=idMemoria;
//	list_add(listaMemorias, memoria1);
//	idMemoria++;

	infoMemoria* memoria1 = newInfoMemoria();
	list_add(listaMemorias, memoria1);

	infoMemoria* memoria2 = newInfoMemoria();
	list_add(listaMemorias, memoria2);

	infoMemoria* memoria3 = newInfoMemoria();
	list_add(listaMemorias, memoria3);

	infoMemoria* memoria4 = newInfoMemoria();
	list_add(listaMemorias, memoria4);

	infoMemoria* memoria5 = newInfoMemoria();
	list_add(listaMemorias, memoria5);

	infoMemoria* memoria6 = newInfoMemoria();
	list_add(listaMemorias, memoria6);

	infoMemoria* memoria7 = newInfoMemoria();
	list_add(listaMemorias, memoria7);
//	infoMemoria* memoria2 = malloc(sizeof(infoMemoria));
////	memoria2->ccia = SHC;
//	memoria2->id = idMemoria;
//	list_add(listaMemorias, memoria2);
//	idMemoria++;
//
//	infoMemoria* memoria3 = malloc(sizeof(infoMemoria));
////	memoria3->ccia = EC;
//	memoria3->id = idMemoria;
//	list_add(listaMemorias, memoria3);
//	idMemoria++;
//
//	infoMemoria* memoria4 = malloc(sizeof(infoMemoria));
//	//	memoria3->ccia = EC;
//	memoria4->id = idMemoria;
//	list_add(listaMemorias, memoria4);
//	idMemoria++;
//
//	infoMemoria* memoria5 = malloc(sizeof(infoMemoria));
//	//	memoria3->ccia = EC;
//	memoria5->id = idMemoria;
//	list_add(listaMemorias, memoria5);
//	idMemoria++;
//
//	infoMemoria* memoria6 = malloc(sizeof(infoMemoria));
//	//	memoria3->ccia = EC;
//	memoria6->id = idMemoria;
//	list_add(listaMemorias, memoria6);
//	idMemoria++;
//
//	infoMemoria* memoria7 = malloc(sizeof(infoMemoria));
//	//	memoria3->ccia = EC;
//	memoria7->id = idMemoria;
//	list_add(listaMemorias, memoria7);
//	idMemoria++;

}

void imprimirCriterio(bool* criterio) {
	if (criterio[0]) {
		log_trace(log_master->logTrace, "Esta memoria tiene criterio SC");
	}
	if (criterio[1]) {
		log_trace(log_master->logTrace, "Esta memoria tiene criterio SHC");
	}
	if (criterio[2]) {
		log_trace(log_master->logTrace, "Esta memoria tiene criterio EC");
	}
	if (criterio[3]) {
		log_error(log_master->logError,
				"Error al asignar criterio a esta memoria. Por favor, intente nuevamente");
		criterio[3] = false;
	}
}

bool instruccionSeaSalir(char* operacion){
	return strcmp(operacion, "SALIR")==0;
}

void hardcodearListaMetadataTabla(){
	metadataTablas* metadata1 = malloc(sizeof(metadataTablas));
	metadata1->consistencia = SC;
	metadata1->nParticiones = 2;
	metadata1->nombreTabla = "TABLA1";
	list_add(listaMetadataTabla, metadata1);

	metadataTablas* metadata2 = malloc(sizeof(metadataTablas));
	metadata2->consistencia = SHC;
	metadata2->nParticiones = 3;
	metadata2->nombreTabla = "TABLA2";
	list_add(listaMetadataTabla, metadata2);

	metadataTablas* metadata3 = malloc(sizeof(metadataTablas));
	metadata3->consistencia = EC;
	metadata3->nParticiones = 4;
	metadata3->nombreTabla = "TABLA3";
	list_add(listaMetadataTabla, metadata3);
}

int contarLineasArchivo(FILE* fichero, char* path) {
	fichero = fopen(path, "rt");

	if(fichero==NULL)
		return 0;
	int caracter, num_lineas = 0;
	while ((caracter = fgetc(fichero)) != EOF)
		if (caracter == '\n')
			num_lineas++;
	fclose(fichero);
	return num_lineas;
}

void destruirArraySemaforos() {
	for (int i = 0; i < multiprocesamiento; i++)
		sem_destroy(&arraySemaforos[i]);

	free(arraySemaforos);
}
