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
//	consistencia ccia;
	bool criterios[4];
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

typedef struct{
	t_log* logInfo;
	t_log* logError;
	t_log* logTrace;
}logStruct;


// Variables globales
#define INFO_KERNEL "/home/utnso/tp-2019-1c-Los-Sisoperadores/KernelV2/infoKernel.log"
#define ERRORES_KERNEL "/home/utnso/tp-2019-1c-Los-Sisoperadores/KernelV2/erroresKernel.log"
#define TRACE_KERNEL "/home/utnso/tp-2019-1c-Los-Sisoperadores/KernelV2/tracesKernel.log"
#define RUTA_CONFIG_KERNEL "/home/utnso/tp-2019-1c-Los-Sisoperadores/KernelV2/configKernel.cfg"

t_queue* colaReady;
t_list* listaHilos;
t_list* listaMetadataTabla;
t_list* listaMemorias;
t_config_kernel *config;
//t_log* logger;
//t_log* loggerError;
int quantum;
int cantRequestsEjecutadas;
sem_t ejecutarHilos;
sem_t mutex_colaReady;
logStruct* log_master;
int idMemoria;
bool haySC;


// Funciones extras, muchas son de la shared library pero todavia no las anexe para ver si funcionaba
int get_campo_config_int(t_config* archivo_configuracion, char* nombre_campo);
char* get_campo_config_string(t_config* archivo_configuracion, char* nombre_campo);
int cantidadParametros(char ** palabras);
t_config_kernel *cargarConfig(char *ruta);
infoMemoria* obtenerMemoriaAlAzar();


void hardcodearInfoMemorias();
void hardcodearListaMetadataTabla();
//void procesarInput(char* linea);


// Funciones importantes
void obtenerMemoriaSegunTablaYKey(int key, char* nombreTabla);
void destruirElementosMain(t_list* lista, t_queue* cola);
bool verificarCriterio(bool* criterio, consistencia ccia);
void imprimirCriterio(bool* criterio);
void destruirLogStruct(logStruct* log_master);
bool instruccionSeaSalir(char* operacion);
procExec* newProceso();
infoMemoria* newInfoMemoria();
void destruirProceso(procExec* proceso);
//void destruirProcesoExec(procExec* proceso);
consistencia mejorCriterioMemoria(bool* criterios);
void deNewAReady(procExec* proceso);
void deReadyAExec();
void asignarCriterioMemoria(infoMemoria* memoria, consistencia cons);
void comandoAdd(int id, consistencia cons);
infoMemoria* obtenerMemoriaAlAzarParaFunciones();
void procesarAdd(char*argumento);
void consolaInsert(char*request);
void consolaSelect(char*argumentos);
void consolaCreate(char*argumentos);
void consolaDescribe(char*nombreTabla);
void consolaDrop(char*nombreTabla);
void agregarRequestAlProceso(procExec* proceso, char* operacion);
void* funcionThread(void* args);
void agregarHiloAListaHilosEInicializo(t_list* hilos);
void ejecutarProcesos();
void inicializarLogStruct();
int funcionHash(t_list* memoriasEncontradas, int key);
infoMemoria* resolverUsandoFuncionHash(t_list* memoriasEncontradas, int key);
infoMemoria* resolverAlAzar(t_list* memoriasEncontradas);
infoMemoria* obtenerMemoria(char* nombreTabla, int key);
consistencia obtenerConsistenciaDe(char* nombreTabla);
infoMemoria* obtenerMemoriaSegunConsistencia(consistencia consistenciaDeTabla, int key);
void destruirListaMemorias();


#endif /* KERNELHEADER_H_ */
