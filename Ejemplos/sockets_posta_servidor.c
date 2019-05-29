/*
 ============================================================================
 Name        : sockets_posta_servidor.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include "../Libraries/sockets.h"

/*
 * Ahora creamos el servidor
 */

int main(void) {
	t_log *log_master = log_create("kernel.log(test)", "config", 1,
			LOG_LEVEL_INFO);

	// Paso 1: Iniciar Servidor

	struct sockaddr_in server;

	int socket_servidor = iniciar_servidor(4444, &server, log_master);

	// Paso 2: Recibir paquete

	int socket_cliente = aceptar_conexion(socket_servidor, log_master, 1); // accept

	// Paso 3: Veo que tipo es y tomo la decision (aunque nosotros no vamos a mandar mensajes de char*, a menos que
	//				tengamos que lidiar con una excepcion?) )

	/*
	 * Este switch es una idea de la API que se tiene que implementar en el tp
	 *
	 * Forkea la request a un camino y este le indica que hacer (ir a memoria, FS, buscar info en archivo, etc)
	 */

	t_list* lista;
	while (1) {
		int cod_op = recibir_operacion(socket_cliente);
		log_info(log_master, "El tipo de mensajes es %d", cod_op);
		switch (cod_op) {
		case MENSAJE:
			recibir_mensaje(socket_cliente, log_master);
			break;
		case PAQUETE_DEFAULT:
			lista = recibir_paquete(socket_cliente);
			printf("Me llegaron los siguientes valores:\n");
			list_iterate(lista, (void*) iterator);
			list_destroy(lista);

			/*********************************/

			enviar_por_socket(socket_cliente, log_master, PAQUETE_DEFAULT);

			break;
		case -1:
			log_error(log_master,
					"el cliente se desconecto. Terminando servidor");
			log_destroy(log_master); // por ahora
			return EXIT_FAILURE;
		default:
			log_warning(log_master,
					"Operacion desconocida. No quieras meter la pata");
			break;
		}
	}

	log_destroy(log_master);
	puts("!!!Hello World!!!"); /* prints !!!Hello World!!! */
	return EXIT_SUCCESS;
}
