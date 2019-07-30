/*
 * KernelOtherFunctions.c
 *
 *  Created on: 20 jun. 2019
 *      Author: utnso
 */

#include "KernelOtherFunctions.h"

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

void imprimirCriterio(infoMemoria* infoMemoria) {

	char* criterios = string_new();

	void concatenarCriterios(t_consistencia consistencia) {
		string_append_with_format(&criterios, "%s ", getConsistenciaCharByEnum(consistencia));
	}
	list_iterate(infoMemoria->criterios, (void*) concatenarCriterios);
	log_info(log_master->logInfo, "La memoria %d tiene criterios %s", infoMemoria->id, criterios);
	free(criterios);
}

bool instruccionSeaSalir(char* operacion) {
	return strcmp(operacion, "SALIR") == 0;
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
	hayMetricas = false;
}

void destruirMetrics(t_metrics* metrica) {
	if (list_is_empty(metrica->diferenciaDeTiempoReadLatency) && list_is_empty(metrica->diferenciaDeTiempoWriteLatency)) {
		list_destroy(metrica->diferenciaDeTiempoReadLatency);
		list_destroy(metrica->diferenciaDeTiempoWriteLatency);
	} else {
		list_destroy_and_destroy_elements(metrica->diferenciaDeTiempoReadLatency, free);
		list_destroy_and_destroy_elements(metrica->diferenciaDeTiempoWriteLatency, free);
	}
}

void reiniciarMetrics(t_metrics* metrica) {
	// Ahora reinicio los valores:
	metrica->reads = 0;
	metrica->writes = 0;
	metrica->readLatency = 0;
	metrica->writeLatency = 0;
	list_clean(metrica->diferenciaDeTiempoReadLatency);
	list_clean(metrica->diferenciaDeTiempoWriteLatency);
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
			double* elemento = (double*) list_get(metricas.diferenciaDeTiempoReadLatency, i);
			sumatoria += (*elemento);
		}

		metricas.readLatency = sumatoria / tamLista;
	}

	// Calcular Write Latency: Idem al anterior -> Podria hacerlos una sola funcion pero... despues

	sumatoria = 0;
	tamLista = list_size(metricas.diferenciaDeTiempoWriteLatency);

	if (tamLista != 0) {
		for (int i = 0; i < tamLista; i++) {
			double* elemento = (double*) list_get(metricas.diferenciaDeTiempoWriteLatency, i);
			sumatoria += (*elemento);
		}

		metricas.writeLatency = sumatoria / tamLista;
	}

	// Calcular Memory Load

	double cantSelectsEInsertsTotales = cantSelects + cantInserts;

	void _calcularMemoryLoadUnaMemoria(infoMemoria* memoria) {
		double cantSelectsEInsertsMemoria = memoria->cantSelectsEjecutados + memoria->cantInsertEjecutados;

		double* memoryLoadMemoria = malloc(sizeof(double));
		*memoryLoadMemoria = cantSelectsEInsertsMemoria / cantSelectsEInsertsTotales;

		memoria->memoryLoadUnaMemoria = *memoryLoadMemoria;
		free(memoryLoadMemoria);
	}

	if (cantSelectsEInsertsTotales != 0) {
		bool _hizoSelectOInsert(void* memoria) {
			return ((infoMemoria*) memoria)->cantInsertEjecutados != 0 || ((infoMemoria*) memoria)->cantSelectsEjecutados != 0;
		}

		listaFiltrada = list_filter(listaMemorias, _hizoSelectOInsert);
		list_iterate(listaFiltrada, (void*) _calcularMemoryLoadUnaMemoria);

		list_destroy(listaFiltrada);

	}

}

void copiarMetrics() {
	copiaMetricas.readLatency = metricas.readLatency;
	copiaMetricas.writeLatency = metricas.writeLatency;
	copiaMetricas.reads = metricas.reads;
	copiaMetricas.writes = metricas.writes;
	list_add_all(copiaMetricas.diferenciaDeTiempoReadLatency, metricas.diferenciaDeTiempoReadLatency);
	list_add_all(copiaMetricas.diferenciaDeTiempoWriteLatency, metricas.diferenciaDeTiempoWriteLatency);

}

void imprimirMetrics(t_metrics metrica) {

	void _imprimirMemoryLoadsDeMemorias(void* memoria) {
		if (((infoMemoria*) memoria)->memoryLoadUnaMemoria != 0)
			log_info(log_master->logInfo, "El memory load de memoria %d es: %f", ((infoMemoria*) memoria)->id,
					((infoMemoria*) memoria)->memoryLoadUnaMemoria);
	}

	log_info(log_master->logInfo, "Las metricas son: ");
	log_info(log_master->logInfo, "Read latency: %f", metrica.readLatency);
	log_info(log_master->logInfo, "Write latency: %f", metrica.writeLatency);
	log_info(log_master->logInfo, "Reads: %f", metrica.reads);
	log_info(log_master->logInfo, "Writes: %f", metrica.writes);
	list_iterate(listaMemorias, _imprimirMemoryLoadsDeMemorias);

}
