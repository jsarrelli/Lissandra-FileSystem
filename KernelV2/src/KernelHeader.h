/*
 * KernelHeader.h
 *
 *  Created on: 17 jun. 2019
 *      Author: utnso
 */

#ifndef KERNELHEADER_H_
#define KERNELHEADER_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <pthread.h>
#include <readline/history.h>
#include <readline/readline.h>
#include <semaphore.h>
#include <commons/string.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include <commons/collections/queue.h>
#include <commons/config.h>

// Estructuras de datos

typedef struct{
	t_list* script;
//	pthread_t* hilo; // Esto se va a hacer al principio, por eso no es necesario ahora
//	bool estaEjecutandose; // Esto tampoco
}procExec;

typedef enum{
	SC,
	SHC,
	EC,
	ERROR_CONSISTENCIA
}consistencia;

typedef struct{
	int id;
	char*ip;
	consistencia ccia;
}infoMemoria;

typedef struct{
	char*nombreTabla;
	consistencia consistencia;
	int nParticiones;
}metadataTablas;

typedef struct {
	char* IP_MEMORIA;
	int PUERTO_MEMORIA;
	int QUANTUM;
	int MULTIPROCESAMIENTO;
	int METADATA_REFRESH;
	int SLEEP_EJECUCION;
} t_config_kernel;



// Variables globales
t_queue* colaReady;
t_list* listaHilos;
t_list* listaMetadataTabla;
t_list* listaMemorias;
t_config_kernel *config;
t_log* logger;
t_log* loggerError;
int quantum;
sem_t ejecutarHilos;
sem_t mutex_colaReady;


// Funciones importantes
void destruirElementosMain(t_list* lista, t_queue* cola, t_log* logger);
bool instruccionSeaSalir(char* operacion);
procExec* newProceso();
void destruirProcesoExec(procExec* proceso);
void deNewAReady(procExec* proceso);
void deReadyAExec();
void comandoAdd(int id, consistencia cons);
void procesarAdd(char*argumento);
void consolaInsert(char*request);
void agregarRequestAlProceso(procExec* proceso, char* operacion);
void* funcionThread(void* args);
void agregarHiloAListaHilosEInicializo(t_list* hilos);
void ejecutarProcesos();


// Funciones extras, muchas son de la shared library pero todavia no las anexe para ver si funcionaba
int get_campo_config_int(t_config* archivo_configuracion, char* nombre_campo);
char* get_campo_config_string(t_config* archivo_configuracion, char* nombre_campo);
int cantidadParametros(char ** palabras);
t_config_kernel *cargarConfig(char *ruta);

void hardcodearInfoMemorias();
void hardcodearListaMetadataTabla();
//void procesarInput(char* linea);


#endif /* KERNELHEADER_H_ */
