/*
 * ProcesoExec.c
 *
 *  Created on: 3 jun. 2019
 *      Author: utnso
 */


#include "ProcesoExec.h"

int contadorGlobal =0; // Solo se usa en funcion_thread, como ejemṕlo


void* funcion_thread(void* args){
	//int ejemplo = (int) args;

	/*
	 * Esta es la funcion donde voy a ejecutar los execs
	 *
	 * Aca voy a redireccionar todos los threads a la api_kernel
	 *
	 */

	contadorGlobal++;
	printf("El contador ahora vale %d\n", contadorGlobal);
	return NULL;
}

//void ejucutar_api_kernel(char*request, t_log* log_master){
//	identificar_request(request, log_master);
//}


void imprimir_string(char* value){
	printf("%s\n", value);
}

void inicializar_lista_threads(proc_exec *lt, int tamanio){
	for(int i=0; i<tamanio;i++){
		(lt+i)->id=0;
	}
}

void delete_threads_in_list(t_queue* queue_hilo) {
	/*
	 * Liberamos memoria
	 */
	for (int k = 0; k < CANT_ELEM_QUEUE; k++) {
		proc_exec* a_eliminar = queue_pop(queue_hilo);
		free(a_eliminar);
	}
}

/*
 * Primera parte de la idea de exec
 */

void deNewAReady(t_log *log_master, t_queue* colaReady, t_list* lista_threads_exec) {

//	log_info(log_master, "\nInicializando valores\n");

	proc_exec* hilo = malloc(sizeof(proc_exec));

	inicializar_lista_threads(hilo, 1);

	// Creo la cola de ready

	queue_push((t_queue*) colaReady, (proc_exec*) hilo);

	// Agregar mas elementos a la pila, pero por ahora no es necesario ya que solo usamos un solo procesador
	cantElemColaReady = config->MULTIPROCESAMIENTO;
	for (int j = 0; j < cantElemColaReady - 1; j++) {
		proc_exec* hilo = malloc(sizeof(proc_exec));
		inicializar_lista_threads(hilo, 1);

		queue_push((t_queue*) colaReady, (proc_exec*) hilo);
	}

	int size = queue_size(colaReady);
	printf("\nEL tamaño de la cola de ready es: %d\n", size);


	for (int i = 0; i < cantElemColaReady; i++) {
		proc_exec* elem_a_agregar = queue_pop(colaReady);
		pthread_create(&(elem_a_agregar->parte_thread), NULL, funcion_thread,
				NULL);
		pthread_join(elem_a_agregar->parte_thread, NULL);
		list_add(lista_threads_exec, elem_a_agregar);
	}

	/*
	 * Liberamos memoria, por ahora no lo hacemos porque necesito los datos
	 */

//	delete_threads_in_list(colaReady);
//
//	queue_destroy(colaReady);
//
//	list_destroy_and_destroy_elements(lista_threads_exec, (void*) free);
//
//	log_info(log_master, "Memoria liberada\n");
}
