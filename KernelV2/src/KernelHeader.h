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
#include <unistd.h>
#include <readline/history.h>
#include <readline/readline.h>
#include <semaphore.h>
#include <Inotify.h>
#include <commons/string.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include <commons/collections/queue.h>
#include <commons/config.h>

// Incorporo la libraries
#include <Libraries.h>
#include <Sockets/Conexiones.h>
#include <Sockets/Serializacion.h>

#define INFO_KERNEL "/home/utnso/tp-2019-1c-Los-Sisoperadores/KernelV2/infoKernel.log"
#define ERRORES_KERNEL "/home/utnso/tp-2019-1c-Los-Sisoperadores/KernelV2/erroresKernel.log"
#define TRACE_KERNEL "/home/utnso/tp-2019-1c-Los-Sisoperadores/KernelV2/tracesKernel.log"
#define RUTA_CONFIG_KERNEL "/home/utnso/tp-2019-1c-Los-Sisoperadores/KernelV2/config/configKernel.cfg"

// Para el manejo de errores

#define TODO_OK 0
#define SUPER_ERROR 1

/*
 * ACLARACIONES DE USO:
 *
 * 		- Al salir del Kernel se tiene que ingresar el comando SALIR pero se tiene que hacer una vez que los procesadores terminen de procesar
 * 				Esto tiene sentido ya que el salir se encarga de guiar a los procesadores a cerrarse definitivamente
 */

// Estructuras de datos
typedef struct {
	int contadorRequests;
	t_list* script;
} procExec;

typedef struct {
	int id;
	char*ip;
	int puerto;
	// Ignorando el comando Metrics y obviamente Select e Insert:
	int cantSelectsEjecutados;
	int cantInsertEjecutados;
	t_list* criterios;
// La memoria en si no conoce que criterio tiene, solo el Kernel lo sabe para saber a donde mandar la informacion
	double memoryLoadUnaMemoria; // Para las metricas
} infoMemoria;

typedef struct {
	char* nombreTabla;
	t_consistencia consitencia;
} infoTabla;

typedef struct {
	char* IP_MEMORIA;
	int PUERTO_MEMORIA;
	int QUANTUM;
	int MULTIPROCESAMIENTO;
	int METADATA_REFRESH;
	int SLEEP_EJECUCION;
} t_config_kernel;

typedef struct {
	t_log* logInfo;
	t_log* logError;
	t_log* logTrace;
} logStruct;

typedef enum {
	OK, ERROR
} estadoProceso;

typedef struct {
	double readLatency;
	double writeLatency;
	double reads;
	double writes;
	// Para calculo auxiliar
//	t_list* memoryLoadMemorias;
	t_list* diferenciaDeTiempoReadLatency;
	t_list* diferenciaDeTiempoWriteLatency;
} t_metrics;

// Variables globales

t_config *kernelConfig;
t_queue* colaReady;
t_list* listaHilos;
t_list* listaInfoTablas;
t_list* listaMemorias;
t_config_kernel *config;
//int quantum;
int idMemoria;
int idHilo;
int multiprocesamiento;
int multiprocesamientoUsado;
int hilosActivos;
//int retardoEjecucion;
int cantSelects;
int cantInserts;
double timestampSelectAlIniciar;
double timestampSelectAlFinalizar;
double timestampInsertAlIniciar;
double timestampInsertAlFinalizar;
sem_t ejecutarHilos;
sem_t mutex_colaReadyPOP;
sem_t mutex_colaReadyPUSH;
sem_t mutex_id_proceso;
sem_t bin_main;
sem_t fin;
sem_t cantProcesosColaReady;
sem_t semMetricas;
pthread_mutex_t mutexListaMemorias;
//sem_t* arraySemaforos; // Borrar
logStruct* log_master;
bool puedeHaberRequests;
bool hayMetricas;
t_metrics metricas;
t_metrics copiaMetricas;

// Funciones extras, muchas son de la shared library pero todavia no las anexe para ver si funcionaba
int get_campo_config_int(t_config* archivo_configuracion, char* nombre_campo);
char* get_campo_config_string(t_config* archivo_configuracion, char* nombre_campo);
int cantidadParametros(char ** palabras);
void cargarConfigKernel();

// Funciones importantes
infoMemoria* obtenerMemoriaAlAzar();
void desbloquearHilos();
void crearProcesoYMandarloAReady(char* operacion);
int actualizarMetricasDeMemoria(int key, char* nombreTabla, t_protocolo protocolo, infoMemoria* memoriaAEnviar);
void destruirElementosMain(t_list* lista, t_queue* cola);
void destruirLogStruct(logStruct* log_master);
procExec* newProceso();
infoMemoria* newInfoMemoria(char* ip, int puert, int id);
void destruirProceso(procExec* proceso);
void deNewAReady(procExec* proceso);
void deReadyAExec();
void agregarRequestAlProceso(procExec* proceso, char* operacion);
void* iniciarMultiprocesamiento(void*args);
//void* funcionThread(void* args);
void inicializarLogStruct();
int funcionHash(t_list* memoriasEncontradas, int key);
infoMemoria* obtenerMemoria(char* nombreTabla, int key);
void destruirListaMemorias();

void freeConfigKernel();
void agregarMemoriaConocida(infoMemoria* memoria);
int asignarCriterioMemoria(infoMemoria* memoria, t_consistencia consistencia);

t_list* filterMemoriasByCriterio(t_consistencia criterio);
infoTabla* obtenerTablaByNombretabla(char* nombreTabla);
infoMemoria* obtenerMemoriaSegunCriterio(t_consistencia consistenciaDeTabla, int key);
bool haySC();

#endif /* KERNELHEADER_H_ */
