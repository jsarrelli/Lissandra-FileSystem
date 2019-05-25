/*
 ============================================================================
 Name        : sockets_posta.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */


#define VALOR_CUALQUIERA 50 // Pero puede ser ilimitado

#include "../../sockets/sockets.h"

/*
 * En este archivo se prueba el cliente
 */

int main(void) {
	t_log *log_master = log_create("kernel.log(test)", "config", 1,
			LOG_LEVEL_INFO);

	char*ip = "127.0.0.1";
	int puerto = 4444;
	struct sockaddr_in server;

	// Primer paso: crear el cliente
	int socket_cliente = iniciar_cliente(&server, puerto, ip, log_master);

	for (int i = 0; i < VALOR_CUALQUIERA; i++) {

		enviar_por_socket(socket_cliente, log_master, PAQUETE_DEFAULT);
		sem_post(&sem_socket);
//		sem_post(&sem_socket);

		t_list* lista;
		int cod_op = recibir_operacion(socket_cliente);
		log_info(log_master, "El tipo de mensajes es %d", cod_op);
		switch (cod_op) {
		case MENSAJE:
			recibir_mensaje(socket_cliente, log_master);
			break;
		case PAQUETE_DEFAULT:
			/*
			 * Se tiene que modificar recibir paquete de acuerdo a lo que se quiera
			 */
			lista = recibir_paquete(socket_cliente);
			printf("Me llegaron los siguientes valores:\n");
			list_iterate(lista, (void*) iterator);
			list_destroy(lista);
			break;
		case -1:
			log_error(log_master,
					"el cliente se desconecto. Terminando servidor");
			log_destroy(log_master); // por ahora
//			return EXIT_FAILURE;
			return EXIT_SUCCESS;
		default:
			log_warning(log_master,
					"Operacion desconocida. No quieras meter la pata");
			break;
		}
	}

	// Quinto paso: liberar memoria

	liberar_conexion(socket_cliente);

	log_destroy(log_master);
	puts("!!!Hello World!!!"); /* prints !!!Hello World!!! */
	return EXIT_SUCCESS;
}
