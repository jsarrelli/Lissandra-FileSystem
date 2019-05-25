/*
 * sockets.c
 *
 *  Created on: 2 may. 2019
 *      Author: utnso
 */

#include "sockets.h"

void iterator(char* value) {
	printf("%s\n", value);
}

void* serializar_paquete(t_paquete* paquete, int bytes) {
	void * magic = malloc(bytes);
	int desplazamiento = 0;

	memcpy(magic + desplazamiento, &(paquete->codigo_operacion), sizeof(int));
	desplazamiento += sizeof(int);
	memcpy(magic + desplazamiento, &(paquete->buffer->size), sizeof(int));
	desplazamiento += sizeof(int);
	memcpy(magic + desplazamiento, paquete->buffer->stream,
			paquete->buffer->size);
	desplazamiento += paquete->buffer->size;

	return magic;
}

int recibir_operacion(int socket_cliente) {
	int cod_op;
	if (recv(socket_cliente, &cod_op, sizeof(int), MSG_WAITALL) != 0)
		return cod_op;
	else {
		close(socket_cliente);
		return -1;
	}
}

void crear_buffer(t_paquete* paquete) {
	paquete->buffer = malloc(sizeof(t_buffer));
	paquete->buffer->size = 0;
	paquete->buffer->stream = NULL;
}

void enviar_mensaje_string(int socket_cliente, t_log *logger, char *mensaje) { // A diferencia del procedimiento enviar aca no necesitamos pasar el tamaño del mensaje
	int tamanio_Mens = strlen(mensaje) + 1;

	void *buffer = malloc(HEADER_SIZE + tamanio_Mens * sizeof(char)); // Inicia serializacion

	memcpy(buffer, (void *) &tamanio_Mens, HEADER_SIZE);

	memcpy(buffer + HEADER_SIZE, mensaje, tamanio_Mens * sizeof(char));

	int ver_send = send(socket_cliente, buffer,
			HEADER_SIZE + tamanio_Mens * sizeof(char), 0); // Envio

	if (ver_send < 0) // Verifico
		log_error(logger, "Error: No se pudo enviar el mensaje");
	else {
		log_info(logger, "Se envio el mensaje");
	}
	free(buffer);
}

t_paquete* crear_paquete(op_code tipo) {
	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = tipo;
	crear_buffer(paquete);
	return paquete;
}

void agregar_a_paquete(t_paquete* paquete, void* valor, int tamanio) {
	paquete->buffer->stream = realloc(paquete->buffer->stream,
			paquete->buffer->size + tamanio + sizeof(int));

	memcpy(paquete->buffer->stream + paquete->buffer->size, &tamanio,
			sizeof(int));
	memcpy(paquete->buffer->stream + paquete->buffer->size + sizeof(int), valor,
			tamanio);

	paquete->buffer->size += tamanio + sizeof(int);
}

void enviar_paquete(t_paquete* paquete, int socket_cliente, t_log *logger) {
	int bytes = paquete->buffer->size + 2 * sizeof(int);
	void* a_enviar = serializar_paquete(paquete, bytes);

	int ver_send = send(socket_cliente, a_enviar, bytes, 0);
	if (ver_send < 0) // Verifico
		log_error(logger, "Error: No se pudo enviar el mensaje");
	else {
		log_info(logger, "Se envio el mensaje");
	}

	free(a_enviar);
}

void eliminar_paquete(t_paquete* paquete) {
	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);
}

void liberar_conexion(int socket_cliente) {
	close(socket_cliente);
}

void _leer_consola_haciendo(void (*accion)(char*)) {
	char* leido = readline(">");

	while (strncmp(leido, "", 1) != 0) {
		accion(leido);
		free(leido);
		leido = readline(">");
	}

	free(leido);
}

void leer_consola(t_log* logger) {
	void loggear(char* leido) {
		log_info(logger, leido);
	}

	_leer_consola_haciendo((void*) loggear);
}

t_paquete* armar_paquete(op_code tipo) {
	t_paquete* paquete = crear_paquete(tipo);

	void _agregar(char* leido) {
		// Estamos aprovechando que podemos acceder al paquete
		// de la funcion exterior y podemos agregarle lineas!
		agregar_a_paquete(paquete, leido, strlen(leido) + 1);
	}

	/*
	 * Warning: para cada modulo hay que cambair esta funcion para que el paquete tenga lo que quieras
	 */

	_leer_consola_haciendo((void*) _agregar);

	return paquete;
}

//int iniciar_cliente(char *ip, char* puerto, t_log* logger) {
//	struct addrinfo hints;
//	struct addrinfo *server_info;
//
//	memset(&hints, 0, sizeof(hints));
//	hints.ai_family = AF_UNSPEC;
//	hints.ai_socktype = SOCK_STREAM;
//	hints.ai_flags = AI_PASSIVE;
//
//	getaddrinfo(ip, puerto, &hints, &server_info);
//
//	int socket_cliente = socket(server_info->ai_family,
//			server_info->ai_socktype, server_info->ai_protocol);
//
//	if (socket_cliente == -1)
//		log_error(logger, "Error: No se puede crear el socket");
//	else
//		log_info(logger, "Socket creado");
//
//	if (connect(socket_cliente, server_info->ai_addr, server_info->ai_addrlen)
//			== -1)
//		log_error(logger, "Error: Conexion fallida del socket al puerto");
//	else
//		log_info(logger, "Conexion creada");
//
//	freeaddrinfo(server_info);
//
//	return socket_cliente;
//}

struct hostent* configurar_cliente(char* ip, t_log *log_master) {
	/* informacion sobre la direccion del servidor */
	struct hostent *he;
	if ((he = gethostbyname(ip)) == NULL) {
		/* llamada a gethostbyname() */
		log_error(log_master, "Error: funcion gethostbyname");
		exit(-1);
	}
	log_info(log_master, "Ingreso de informacion ok");
	return he;
}

void connect_to_server(int fd, const struct sockaddr_in* server, t_log *log_master) {
	//Paso 3, conectarnos al servidor
	if (connect(fd, (struct sockaddr*) &*server, sizeof(struct sockaddr))
			== -1) {
		/* llamada a connect() */
		log_error(log_master, "Error: funcion connect");
		exit(-1);
	}
	log_info(log_master, "Connect ok");
}

void connect_server(struct sockaddr_in* server, int puerto, int fd,
		struct hostent* info, t_log *log_master) {

	//Datos del servidor
	server->sin_family = AF_INET;
	server->sin_port = htons(puerto);
	server->sin_addr = *((struct in_addr*)info->h_addr);
	bzero(&(server->sin_zero), 8);

	//Paso 3, conectarnos al servidor
	connect_to_server(fd, &*server, log_master);
}

int iniciar_cliente(struct sockaddr_in* server, int puerto, char* ip,
		t_log* log_master) {
	/* informacion sobre la direccion del servidor */
	struct hostent* info;

	/* estructura que recibira informacion sobre el nodo remoto */
	info = configurar_cliente(ip, log_master);

	//Paso 2, definicion de socket
	int socket_cliente = crear_socket(log_master);

	//Datos del servidor
	connect_server(&*server, puerto, socket_cliente, info, log_master);

	return socket_cliente;
}


int aceptar_conexion(int socket_servidor, t_log *logger,
		int numConexion) {
	struct sockaddr cliente_tam;

	unsigned int sin_size = sizeof(struct sockaddr_in);

	int cliente = accept(socket_servidor, &cliente_tam, &sin_size);

	if (cliente < 0) {
		log_info(logger, "Error: No se pudo aceptar la conexion %d",
				numConexion);
		exit(1);
	}
	log_info(logger, "Se acepto la conexion %d", numConexion);

	return cliente;
}

void* recibir_buffer(int* size, int socket_cliente) {
	void * buffer;

	recv(socket_cliente, size, sizeof(int), MSG_WAITALL);
	buffer = malloc(*size);
	recv(socket_cliente, buffer, *size, MSG_WAITALL);

	return buffer;
}

void recibir_mensaje(int socket_cliente, t_log *logger) {
	int size;
	char* buffer = recibir_buffer(&size, socket_cliente);
	log_info(logger, "Me llego el mensaje %s", buffer);
	free(buffer);
}

//podemos usar la lista de valores para poder hablar del for y de como recorrer la lista
t_list* recibir_paquete(int socket_cliente) {
	int size;
	int desplazamiento = 0;
	void * buffer;
	t_list* valores = list_create();
	int tamanio;

	buffer = recibir_buffer(&size, socket_cliente);
	while (desplazamiento < size) {
		memcpy(&tamanio, buffer + desplazamiento, sizeof(int));
		desplazamiento += sizeof(int);
		char* valor = malloc(tamanio);
		memcpy(valor, buffer + desplazamiento, tamanio);
		desplazamiento += tamanio;
		list_add(valores, valor);
//		free(valor);
	}
	free(buffer);
	return valores;
	return NULL;
}

//int iniciar_servidor(char *ip, char* puerto, t_log* logger) {
//	int socket_servidor;
//
//	struct addrinfo hints, *servinfo, *p;
//
//	memset(&hints, 0, sizeof(hints));
//	hints.ai_family = AF_UNSPEC;
//	hints.ai_socktype = SOCK_STREAM;
//	hints.ai_flags = AI_PASSIVE;
//
//	getaddrinfo(ip, puerto, &hints, &servinfo);
//
//	for (p = servinfo; p != NULL; p = p->ai_next) {
//		if ((socket_servidor = socket(p->ai_family, p->ai_socktype,
//				p->ai_protocol)) == -1) {
//			log_error(logger, "Error: No se pudo crear el socket");
//			continue;
//		} else
//			log_info(logger, "Socket creado");
//
//		int activado = 1;
//		setsockopt(socket_servidor, SOL_SOCKET, SO_REUSEADDR, &activado,
//				sizeof(activado));
//		// Para no tener inconvenientes a la hora de reabrir sockets
//
//		if (bind(socket_servidor, p->ai_addr, p->ai_addrlen) == -1) {
//			log_error(logger, "Error: No se pudo bindear el socket");
//			close(socket_servidor);
//			continue;
//		} else
//			log_info(logger, "Socket bindeado");
//		break;
//	}
//
//	listen(socket_servidor, SOMAXCONN);
//
//	freeaddrinfo(servinfo);
//
//	log_trace(logger, "Listo para escuchar a mi cliente");
//
//	return socket_servidor;
//}

void configurar_server(int puerto, struct sockaddr_in* server) {
	//Configuracion del servidor
	server->sin_family = AF_INET; //Familia TCP/IP
	server->sin_port = htons(puerto); //Puerto
	server->sin_addr.s_addr = INADDR_ANY; //Cualquier cliente puede conectarse
	bzero(&(server->sin_zero), 8); //Funcion que rellena con 0's
}

int crear_socket(t_log* logger) {
	int fd;
	if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		log_error(logger, "Error en funcion socket");
		exit(-1);
	}
	log_info(logger, "Socket creado");
	return fd;
}

void bindear_server(int fd, const struct sockaddr_in* server, t_log* logger) {

	int activado = 1;
	setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &activado, sizeof(activado));

	if (bind(fd, (struct sockaddr*) &*server, sizeof(struct sockaddr)) == -1) {
		log_error(logger, "Error en funcion bind");
		exit(-1);
	}
	log_info(logger, "Server bindeado");
}

void listen_server(int fd, t_log* logger) {
	//Paso 4, establecer el socket en modo escucha
	if (listen(fd, SOMAXCONN) == -1) {
		log_error(logger, "Error en funcion bind");
		exit(-1);
	}
	log_info(logger, "Server escuchando");
}

int iniciar_servidor(int puerto, struct sockaddr_in* server, t_log* logger) {

	//Configuracion del servidor

	configurar_server(puerto, &*server);

	//Paso 2, definicion de socket

	int socket_servidor = crear_socket(logger);

	//Paso 3, avisar al sistema que se creo un socket
	bindear_server(socket_servidor, &*server, logger);

	//Paso 4, establecer el socket en modo escucha
	listen_server(socket_servidor, logger);

	return socket_servidor;
}

void enviar_por_socket(int socket_cliente, t_log* log_master, op_code tipo) {
	// Segundo paso: armar el paquete

	/*
	 * Se tiene que modificar armar paquete de acuerdo a lo que se quiera
	 */

	t_paquete* paquete = armar_paquete(tipo);
	// Tercer paso : mandarlo
	enviar_paquete(paquete, socket_cliente, log_master);

	eliminar_paquete(paquete);
}

void enviar_info(int socket_cliente, t_log* log_master, op_code tipo, void* info){
	t_paquete* paquete = crear_paquete(tipo);
	agregar_a_paquete(paquete, info, sizeof(info));
	enviar_paquete(paquete, socket_cliente, log_master);

	eliminar_paquete(paquete);
}

int fork_tipo_paquete(int socket_cliente, t_log *log_master, t_list* lista) {
//	int socket_cliente = esperando_una_conexion(fd, log_master, 1); // accept
//	printf("El socket_cliente es: %d\n", socket_cliente);

//	int socket_cliente = esperando_una_conexion(fd, log_master, 1); // accept
	int cod_op = recibir_operacion(socket_cliente);
	log_info(log_master, "El tipo de mensajes es %d", cod_op);
	switch (cod_op) {
	case MENSAJE:
		recibir_mensaje(socket_cliente, log_master);
		// Nunca entra aca
		break;
	case PAQUETE_DEFAULT:
		lista = recibir_paquete(socket_cliente);
		printf("Me llegaron los siguientes valores:\n");
		list_iterate(lista, (void*) iterator);
		list_destroy(lista);

		/*********************************/

		//			enviar_por_socket(socket_cliente, log_master, PAQUETE_DEFAULT);
		break;
	case -1:
		log_error(log_master, "el cliente se desconecto. Terminando servidor");
		log_destroy(log_master); // por ahora
		return EXIT_FAILURE;
	default:
		log_warning(log_master,
				"Operacion desconocida. No quieras meter la pata");
		break;
	}
	return EXIT_SUCCESS;

}

int recv_to(int socket_cliente, t_list* lista, int len, int flags, int to,
		t_log *log_master) {
	fd_set readset;
	int result, iof = -1;
	struct timeval tv;

	// Initialize the set
	FD_ZERO(&readset);
	FD_SET(socket_cliente, &readset);

	// Initialize time out struct
	tv.tv_sec = 0;
	tv.tv_usec = to * 1000;
	// select()

	sem_wait(&sem_socket);

	result = select(socket_cliente + 1, &readset, NULL, NULL, &tv);

	// Check status
	if (result < 0)
		return -1;
	else if (result > 0 && FD_ISSET(socket_cliente, &readset)) {
		// Set non-blocking mode
			if ((iof = fcntl(socket_cliente, F_GETFL, 0)) != -1)
				fcntl(socket_cliente, F_SETFL, iof | O_NONBLOCK);

			// receive
			//         result = recv(fd, buffer, len, flags);
			result = fork_tipo_paquete(socket_cliente, log_master, lista);

			// set as before
			if (iof != -1)
				fcntl(socket_cliente, F_SETFL, iof);

			//		sem_post(&sem_socket);
//			do{
//				result = select(socket_cliente + 1, &readset, NULL, NULL, &tv);
//			}while(result > 0);

		if (result < 0)
			return -1;

		return result; // Creo que siempre daria 0 en este caso
	}
	return -2;
}
