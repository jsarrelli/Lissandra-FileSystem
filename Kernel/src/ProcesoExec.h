/*
 * ProcesoExec.h
 *
 *  Created on: 3 jun. 2019
 *      Author: utnso
 */

#ifndef PROCESOEXEC_H_
#define PROCESOEXEC_H_

#include "API_kernel.h"
#include "commons/collections/queue.h"


#define CANT_ELEM_QUEUE 7 // Constante de ejemplo, recordar que tiene que ser una cola infinita
#define CANT_THREADS_EXEC 1 // Constante de ejemplo
// Esto deberia ser leido de un archivo de configuracion

typedef struct estructura_p_thread {
	int id;
	char*request; // Tendria que ser un script, pero por ahora nos conformamos con esto
	pthread_t parte_thread;
} t_p_thread;

int cantElemColaReady;

void* funcion_thread(void* args);
//void inicializar_una_thread(t_p_thread* hilo);
//void join_cola_thread(t_p_thread* hilo);
//void iterate_cola_threads(t_queue* cola_threads, void(*funcion)(void*));
//void crear_cola_threads(t_p_thread* thread);
//void agregar_hilo_cola(t_p_thread* thread, t_queue* cola_threads);
void ejucutar_api_kernel(char*request, t_log* log_master);
void imprimir_string(char* value);
void inicializar_lista_threads(t_p_thread *lt, int tamanio);
void delete_threads_in_list(t_queue* queue_hilo); // Esta funcion se podria modificar para usarse en un list_iterate
void deNewAReady(t_log *log_master);

#endif /* PROCESOEXEC_H_ */
