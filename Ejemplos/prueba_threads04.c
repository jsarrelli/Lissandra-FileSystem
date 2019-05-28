/*
 ============================================================================
 Name        : prueba_threads04.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h> // Para Threads
#include <unistd.h> // Para algunas syscalls, ej: sleep(1)
#include <commons/string.h>
#include <commons/collections/list.h>
#include <commons/collections/queue.h>
#include <semaphore.h> // Para Semaforos

#include "../Libraries/sockets.h"

#define NUMERO 5
#define CANT_ELEM_QUEUE 7
#define CANT_THREADS_EXEC 3
#define VALOR_CUALQUIERA 50

typedef struct estructura_p_thread{
	int id;
	char**script;
	pthread_t parte_thread;
} t_p_thread;

int contadorGlobal =0;

/*
 * Aclaracion: estoy declarando el semaforo como variable
 * global
 * Para los casos posta va a tener que ir como parametro.
 */

sem_t pausa;


//sem_t consumidor;

void* funcion_thread(void* args){
	//int ejemplo = (int) args;
	printf("El contador ahora vale %d\n", contadorGlobal);
	contadorGlobal++;
//	sleep(1);

	/*
	 * Aca pienso agregar un semaforo para que ponerlos "en pausa"
	 */

//	sem_wait(&pausa);
//
//	printf("Si llega hasta aca, es porque todo salio como esperaba\n");

//	sem_post(&pausa);

	return NULL;
}

void* funcion_thread2(void*args){
	t_log *log_master = log_create("kernel.log(test)", "config", 1,
			LOG_LEVEL_INFO);

//	char*ip = "127.0.0.1";
//	int puerto = 4444;
//
//	struct sockaddr_in* server;
//
//	int socket_cliente = iniciar_cliente(server, &puerto, ip, log_master);
//
//	enviar_por_socket(socket_cliente, log_master, PAQUETE_DEFAULT);

	return NULL;
}

void inicializar_lista_threads(t_p_thread *lt, int tamanio){
	for(int i=0; i<tamanio;i++){
		(lt+i)->id=0;
	}
}

void extracted() {
	t_p_thread lista_threads[NUMERO];
	inicializar_lista_threads(&lista_threads, NUMERO);
	puts("!!!Hello World!!!"); /* prints !!!Hello World!!! */
	//	puts(string_itoa(lista_threads[0].id));
	for (int i = 0; i < NUMERO; i++) {
		printf("El id de cada struct es %d\n", lista_threads[i].id);
		pthread_create(&(lista_threads[i].parte_thread), NULL, funcion_thread,
				NULL);
		/*
		 * Warning: posible race condition
		 */
		//		puts("Seguro genera race condition ?)");
		//		for(int j=0;j < 10;j++)
		//			printf("Numeros: %d\n", j);
		//		sleep(5);
		pthread_join(lista_threads[i].parte_thread, NULL);
	}
}

void ejemplo2(){
	/*
	 * Primera parte de la idea de exec
	 */

	printf("Inicializando valores\n");

	t_queue* queue_hilo = queue_create();
//	sem_init(&pausa, 0, 1);

//	t_p_thread* hilo = malloc(sizeof(t_p_thread));
//	inicializar_lista_threads(hilo, 1);
//
//	// Pushea
//	queue_push((t_queue*)queue_hilo, (t_p_thread*) hilo);

	/*
	 * Desarrollo importante
	 */

	// Agregar mas elementos a la pila
	for(int j=0;j < CANT_ELEM_QUEUE; j++){
		t_p_thread* hilo = malloc(sizeof(t_p_thread));
		inicializar_lista_threads(hilo, 1);

		// Pushea
		queue_push((t_queue*)queue_hilo, (t_p_thread*) hilo);

//		free(hilo);
	}

	int size = queue_size(queue_hilo);
	printf ("El tamaño del queue es: %d\n", size);
//	sem_wait(&pausa);
//	printf("Si llega hasta aca, es porque todo salio como esperaba\n");

	/*
	 * Buscar forma de que se pueda ver los value de los semaforos
	 */

//	int* value_sem=0;
//	sem_getvalue((sem_t *) &pausa, (int*)value_sem);
//	printf("El valor del semaforo (en teoria) es: %d\n", value_sem);

	t_list* lista_threads_exec = list_create();
	for(int i=0; i < CANT_THREADS_EXEC;i++){
		t_p_thread* elem_a_agregar = queue_pop(queue_hilo);
		pthread_create(&(elem_a_agregar->parte_thread), NULL, funcion_thread2, NULL);
		pthread_join(elem_a_agregar->parte_thread, NULL);
		list_add(lista_threads_exec, elem_a_agregar);
	}

	printf("Los elementos en la memoria son %d\n", list_size(lista_threads_exec));
	/*
	 * Liberamos memoria
	 */

	//Libero pila
	for(int k=0;k<(CANT_ELEM_QUEUE - CANT_THREADS_EXEC);k++){
		t_p_thread* a_eliminar = queue_pop(queue_hilo);
		free(a_eliminar);
	}
//	for (int k = 0; k < CANT_THREADS_EXEC; k++) {
//		t_p_thread* a_eliminar = queue_pop(queue_hilo);
//		free(a_eliminar);
//	}

	queue_destroy(queue_hilo);
//	free(hilo);
//	sem_destroy(&pausa);
//	list_destroy(lista_threads_exec);
	list_destroy_and_destroy_elements(lista_threads_exec, (void*) free);
	printf("Memoria liberada\n");

}

int main(void) {
//	extracted();
	ejemplo2();
	return EXIT_SUCCESS;
}
