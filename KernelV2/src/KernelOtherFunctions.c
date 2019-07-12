/*
 * KernelOtherFunctions.c
 *
 *  Created on: 20 jun. 2019
 *      Author: utnso
 */

#include "KernelOtherFunctions.h"

consistencia procesarConsistencia(char* palabra) {
	if (strcmp(palabra, "SC") == 0)
		return SC;
	else if (strcmp(palabra, "SHC") == 0)
		return SHC;
	else if (strcmp(palabra, "EC") == 0)
		return EC;
	return ERROR_CONSISTENCIA;
}

bool verificarCriterio(bool* criterio, consistencia ccia) {
	switch (ccia) {
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

void hardcodearInfoMemorias() {

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

bool instruccionSeaSalir(char* operacion) {
	return strcmp(operacion, "SALIR") == 0;
}

void hardcodearListaMetadataTabla() {
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

	if (fichero == NULL)
		return 0;
	int caracter, num_lineas = 0;
	while ((caracter = fgetc(fichero)) != EOF)
		if (caracter == '\n')
			num_lineas++;
	fclose(fichero);
	return num_lineas + 1;
}

//void destruirArraySemaforos() {
//	for (int i = 0; i < multiprocesamiento; i++)
//		sem_destroy(&arraySemaforos[i]);
//
//	free(arraySemaforos);
//}

int instruccionSeaMetrics(char* operacion) {
	return strcmp(operacion, "METRICS") == 0;
}

void crearMetrica() {
	metricas.readLatency = 0;
	metricas.writeLatency = 0;
	metricas.reads = 0;
	metricas.writes = 0;
	metricas.diferenciaDeTiempoReadLatency = list_create();
	metricas.diferenciaDeTiempoWriteLatency = list_create();
	metricas.memoryLoadMemorias = list_create();
	hayMetricas = true;
}

void destruirMetrics() {
	list_destroy_and_destroy_elements(metricas.diferenciaDeTiempoReadLatency, free);
	list_destroy_and_destroy_elements(metricas.diferenciaDeTiempoWriteLatency, free);
	list_destroy_and_destroy_elements(metricas.memoryLoadMemorias, free);
}

void reiniciarMetrics() {
	// Ahora reinicio los valores:
	metricas.reads = 0;
	metricas.writes = 0;
	metricas.readLatency = 0;
	metricas.writeLatency = 0;
	list_clean(metricas.diferenciaDeTiempoReadLatency);
	list_clean(metricas.diferenciaDeTiempoWriteLatency);
	list_clean(metricas.memoryLoadMemorias);
	// Acordarse de borrar las metricas de las estructuras de las memorias -> Creo que no me interesa
	// Recordar que pueden seguir estando o no las memorias de los datos	-> MUY IMPORTANTE!!!!!
}

void calcularMetrics() {

	/*
	* Reads y Writes ya esta calculados
	*
	* (Se sacan solo con los contadores)
	*/

	// Calcular Read Latency

	t_list* listaFiltrada;
	double sumatoria = 0;
	double tamLista = list_size(metricas.diferenciaDeTiempoReadLatency);

	if (tamLista != 0) {
		for (int i = 0; i < tamLista; i++) {
			double* elemento = (double*) list_get(
					metricas.diferenciaDeTiempoReadLatency, i);
			sumatoria += (*elemento);
		}

		metricas.readLatency = sumatoria / tamLista;
	}

	// Calcular Write Latency: Idem al anterior -> Podria hacerlos una sola funcion pero... despues

	sumatoria = 0;
	tamLista = list_size(metricas.diferenciaDeTiempoWriteLatency);

	if (tamLista != 0) {
		for (int i = 0; i < tamLista; i++) {
			double* elemento = (double*) list_get(
					metricas.diferenciaDeTiempoWriteLatency, i);
			sumatoria += (*elemento);
		}

		metricas.writeLatency = sumatoria / tamLista;
	}

	// Calcular Memory Load

	double cantSelectsEInsertsTotales = cantSelects + cantInserts;

	void _calcularMemoryLoadUnaMemoria(infoMemoria* memoria) {
		double cantSelectsEInsertsMemoria = memoria->cantSelectsEjecutados
				+ memoria->cantInsertEjecutados;

		double* memoryLoadMemoria = malloc(sizeof(double));
		*memoryLoadMemoria = cantSelectsEInsertsMemoria
				/ cantSelectsEInsertsTotales;

		list_add(metricas.memoryLoadMemorias, memoryLoadMemoria);
	}

	if (cantSelectsEInsertsTotales != 0){
		bool _hizoSelectOInsert(void* memoria){
			return ((infoMemoria*)memoria)->cantInsertEjecutados !=0 || ((infoMemoria*)memoria)->cantSelectsEjecutados!=0;
		}

		listaFiltrada = list_filter(listaMemorias, _hizoSelectOInsert);
		list_iterate(listaFiltrada, (void*) _calcularMemoryLoadUnaMemoria);

//		tamLista = list_size(metricas.memoryLoadMemorias);
//		for (int i = 0; i < tamLista; i++) {
//			double* elemento = (double*) list_get(metricas.memoryLoadMemorias,
//					i);
//			double elemento2 = *elemento;
//			metricas.memoryLoad += elemento2;
//		}

		list_destroy(listaFiltrada);

	}

}

void imprimirMetrics() {

	log_info(log_master->logInfo, "Las metricas son: ");
	log_info(log_master->logInfo, "Read latency: %f", metricas.readLatency);
	log_info(log_master->logInfo, "Write latency: %f", metricas.writeLatency);
	log_info(log_master->logInfo, "Reads: %f", metricas.reads);
	log_info(log_master->logInfo, "Writes: %f", metricas.writes);
//	log_info(log_master->logInfo, "Memory load: %f\n", metricas.memoryLoad);

}
