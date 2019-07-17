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

	infoMemoria* memoria1 = newInfoMemoria("127.0.0.1", 35615, 1);
	list_add(listaMemorias, memoria1);

	infoMemoria* memoria2 = newInfoMemoria("127.0.0.1", 35615, 2);
	list_add(listaMemorias, memoria2);

	infoMemoria* memoria3 = newInfoMemoria("127.0.0.1", 35615, 3);
	list_add(listaMemorias, memoria3);

	infoMemoria* memoria4 = newInfoMemoria("127.0.0.1", 35615, 4);
	list_add(listaMemorias, memoria4);

	infoMemoria* memoria5 = newInfoMemoria("127.0.0.1", 35615, 5);
	list_add(listaMemorias, memoria5);

	infoMemoria* memoria6 = newInfoMemoria("127.0.0.1", 35615, 6);
	list_add(listaMemorias, memoria6);

	infoMemoria* memoria7 = newInfoMemoria("127.0.0.1", 35615, 7);
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
	metadataTabla* metadata1 = malloc(sizeof(metadataTabla));
	metadata1->consistencia = SC;
	metadata1->nParticiones = 2;
	metadata1->nombreTabla = "TABLA1";
	list_add(listaMetadataTabla, metadata1);

	metadataTabla* metadata2 = malloc(sizeof(metadataTabla));
	metadata2->consistencia = SHC;
	metadata2->nParticiones = 3;
	metadata2->nombreTabla = "TABLA2";
	list_add(listaMetadataTabla, metadata2);

	metadataTabla* metadata3 = malloc(sizeof(metadataTabla));
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

int instruccionSeaMetrics(char* operacion) {
	return strcmp(operacion, "METRICS") == 0;
}

void crearMetrics(t_metrics* metrica) {
	metrica->readLatency = 0;
	metrica->writeLatency = 0;
	metrica->reads = 0;
	metrica->writes = 0;
	metrica->diferenciaDeTiempoReadLatency = list_create();
	metrica->diferenciaDeTiempoWriteLatency = list_create();
//	metricas.memoryLoadMemorias = list_create();
	hayMetricas = false;
}

void destruirMetrics(t_metrics* metrica) {
	if (list_is_empty(metrica->diferenciaDeTiempoReadLatency)
			&& list_is_empty(metrica->diferenciaDeTiempoWriteLatency)) {
		list_destroy(metrica->diferenciaDeTiempoReadLatency);
		list_destroy(metrica->diferenciaDeTiempoWriteLatency);
	} else {
		list_destroy_and_destroy_elements(
				metrica->diferenciaDeTiempoReadLatency, free);
		list_destroy_and_destroy_elements(
				metrica->diferenciaDeTiempoWriteLatency, free);
	}
//	list_destroy_and_destroy_elements(metricas.memoryLoadMemorias, free);
}

void reiniciarMetrics(t_metrics* metrica) {
	// Ahora reinicio los valores:
	metrica->reads = 0;
	metrica->writes = 0;
	metrica->readLatency = 0;
	metrica->writeLatency = 0;
	list_clean_and_destroy_elements(metrica->diferenciaDeTiempoReadLatency, free);
	list_clean_and_destroy_elements(metrica->diferenciaDeTiempoWriteLatency, free);
//	list_clean(metricas.memoryLoadMemorias);
	// TODO:
	// Acordarse de borrar la info de todas las memorias
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

//		list_add(metricas.memoryLoadMemorias, memoryLoadMemoria); // Este es un problema. Que pasa si se me cae una memoria??
		// Hay que guardar este dato en cada memoria, y si se me cae la memoria, pierdo el dato (esta ok), porque recalculo siempre
		memoria->memoryLoadUnaMemoria = *memoryLoadMemoria;
		free(memoryLoadMemoria);
	}

	if (cantSelectsEInsertsTotales != 0) {
		bool _hizoSelectOInsert(void* memoria) {
			return ((infoMemoria*) memoria)->cantInsertEjecutados != 0
					|| ((infoMemoria*) memoria)->cantSelectsEjecutados != 0;
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

void copiarMetrics(){
	copiaMetricas.readLatency = metricas.readLatency;
	copiaMetricas.writeLatency = metricas.writeLatency;
	copiaMetricas.reads = metricas.reads;
	copiaMetricas.writes = metricas.writes;
	list_add_all(copiaMetricas.diferenciaDeTiempoReadLatency, metricas.diferenciaDeTiempoReadLatency);
	list_add_all(copiaMetricas.diferenciaDeTiempoWriteLatency, metricas.diferenciaDeTiempoWriteLatency);
//	copiaMetricas.diferenciaDeTiempoReadLatency = metricas.diferenciaDeTiempoReadLatency;
//	copiaMetricas.diferenciaDeTiempoWriteLatency = metricas.diferenciaDeTiempoWriteLatency;

}

void imprimirMetrics(t_metrics metrica) {

	void _imprimirMemoryLoadsDeMemorias(void* memoria) {
		if (((infoMemoria*) memoria)->memoryLoadUnaMemoria != 0)
			log_info(log_master->logInfo, "El memory load de memoria %d es: %f",
					((infoMemoria*) memoria)->id,
					((infoMemoria*) memoria)->memoryLoadUnaMemoria);
	}

	log_info(log_master->logInfo, "Las metricas son: ");
	log_info(log_master->logInfo, "Read latency: %f", metrica.readLatency);
	log_info(log_master->logInfo, "Write latency: %f", metrica.writeLatency);
	log_info(log_master->logInfo, "Reads: %f", metrica.reads);
	log_info(log_master->logInfo, "Writes: %f", metrica.writes);
//	log_info(log_master->logInfo, "Memory load: %f\n", metrica.memoryLoad);
	list_iterate(listaMemorias, _imprimirMemoryLoadsDeMemorias);

}

bool memoriaTieneALgunCriterio(infoMemoria* memoria) {
	return (memoria->criterios)[0] || (memoria->criterios)[1]
			|| (memoria->criterios)[2];
}
