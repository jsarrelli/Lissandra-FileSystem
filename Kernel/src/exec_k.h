/*
 * exec_k.h
 *
 *  Created on: 29 abr. 2019
 *      Author: utnso
 */

#ifndef EXEC_K_H_
#define EXEC_K_H_

#include "api_kernel.h"

typedef struct estructura_p_thread{
	int id;
	char*script;
	pthread_t parte_thread;
} t_p_thread;

#define CANT_ELEM_QUEUE 7 // Constante de ejemplo
#define CANT_THREADS_EXEC 3 // Constante de ejemplo
// Esto deberia ser leido de un archivo de configuracion

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
void from_new_to_ready(t_log *log_master);

#endif /* EXEC_K_H_ */
