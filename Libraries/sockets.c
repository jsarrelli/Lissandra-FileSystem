/*
 * sockets.c
 *
 *  Created on: 2 may. 2019
 *      Author: utnso
 */


#include "sockets.h"


void* serializar_paquete(t_paquete* paquete, int bytes)
{
	void * magic = malloc(bytes);
	int desplazamiento = 0;

	memcpy(magic + desplazamiento, &(paquete->codigo_operacion), sizeof(int));
	desplazamiento+= sizeof(int);
	memcpy(magic + desplazamiento, &(paquete->buffer->size), sizeof(int));
	desplazamiento+= sizeof(int);
	memcpy(magic + desplazamiento, paquete->buffer->stream, paquete->buffer->size);
	desplazamiento+= paquete->buffer->size;

	return magic;
}

int recibir_operacion(int socket_cliente)
{
	int cod_op;
	if(recv(socket_cliente, &cod_op, sizeof(int), MSG_WAITALL) != 0)
		return cod_op;
	else
	{
		close(socket_cliente);
		return -1;
	}
}

void crear_buffer(t_paquete* paquete)
{
	paquete->buffer = malloc(sizeof(t_buffer));
	paquete->buffer->size = 0;
	paquete->buffer->stream = NULL;
}

void enviar_mensaje_string(int socket_cliente, t_log *logger, char *mensaje)
{ // A diferencia del procedimiento enviar aca no necesitamos pasar el tamaño del mensaje
	int tamanio_Mens = strlen(mensaje) + 1;

	void *buffer = malloc(HEADER_SIZE + tamanio_Mens * sizeof(char)); // Inicia serializacion

	memcpy(buffer, (void *) &tamanio_Mens, HEADER_SIZE);

	memcpy(buffer + HEADER_SIZE, mensaje, tamanio_Mens * sizeof(char));

	int ver_send = send(socket_cliente, buffer, HEADER_SIZE + tamanio_Mens * sizeof(char), 0); // Envio

	if (ver_send < 0) // Verifico
		log_error(logger, "Error: No se pudo enviar el mensaje");
	else {
		log_info(logger, "Se envio el mensaje");
	}
	free(buffer);
}

t_paquete* crear_paquete(op_code tipo)
{
	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = tipo;
	crear_buffer(paquete);
	return paquete;
}

void agregar_a_paquete(t_paquete* paquete, void* valor, int tamanio)
{
	paquete->buffer->stream = realloc(paquete->buffer->stream, paquete->buffer->size + tamanio + sizeof(int));

	memcpy(paquete->buffer->stream + paquete->buffer->size, &tamanio, sizeof(int));
	memcpy(paquete->buffer->stream + paquete->buffer->size + sizeof(int), valor, tamanio);

	paquete->buffer->size += tamanio + sizeof(int);
}

void enviar_paquete(t_paquete* paquete, int socket_cliente, t_log *logger)
{
	int bytes = paquete->buffer->size + 2*sizeof(int);
	void* a_enviar = serializar_paquete(paquete, bytes);

	int ver_send = send(socket_cliente, a_enviar, bytes, 0);
	if (ver_send < 0) // Verifico
		log_error(logger, "Error: No se pudo enviar el mensaje");
	else {
		log_info(logger, "Se envio el mensaje");
	}

	free(a_enviar);
}

void eliminar_paquete(t_paquete* paquete)
{
	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);
}

void liberar_conexion(int socket_cliente)
{
	close(socket_cliente);
}

void _leer_consola_haciendo(void(*accion)(char*)) {
	char* leido = readline(">");

	while(strncmp(leido, "", 1) != 0) {
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

int iniciar_cliente(char *ip, char* puerto, t_log* logger)
{
	struct addrinfo hints;
	struct addrinfo *server_info;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(ip, puerto, &hints, &server_info);

	int socket_cliente = socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol);

	if(socket_cliente==-1)
		log_error(logger, "Error: No se puede crear el socket");
	else
		log_info(logger, "Socket creado");

	if(connect(socket_cliente, server_info->ai_addr, server_info->ai_addrlen) == -1)
		log_error(logger, "Error: Conexion fallida del socket al puerto");
	else
		log_info(logger, "Conexion creada");

	freeaddrinfo(server_info);

	return socket_cliente;
}

int esperando_una_conexion(int socket_servidor, t_log *logger, int numConexion)
{
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

void* recibir_buffer(int* size, int socket_cliente)
{
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
t_list* recibir_paquete(int socket_cliente)
{
	int size;
	int desplazamiento = 0;
	void * buffer;
	t_list* valores = list_create();
	int tamanio;

	buffer = recibir_buffer(&size, socket_cliente);
	while(desplazamiento < size)
	{
		memcpy(&tamanio, buffer + desplazamiento, sizeof(int));
		desplazamiento+=sizeof(int);
		char* valor = malloc(tamanio);
		memcpy(valor, buffer+desplazamiento, tamanio);
		desplazamiento+=tamanio;
		list_add(valores, valor);
//		free(valor);
	}
	free(buffer);
	return valores;
	return NULL;
}

int iniciar_servidor(char *ip, char* puerto, t_log* logger)
{
	int socket_servidor;

    struct addrinfo hints, *servinfo, *p;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    getaddrinfo(ip, puerto, &hints, &servinfo);

    for (p=servinfo; p != NULL; p = p->ai_next)
    {
        if ((socket_servidor = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1){
        	log_error(logger, "Error: No se pudo crear el socket");
        	continue;
        }
        else
        	log_info(logger, "Socket creado");


        int activado = 1;
        setsockopt(socket_servidor,SOL_SOCKET,SO_REUSEADDR,&activado,sizeof(activado));
        // Para no tener inconvenientes a la hora de reabrir sockets

        if (bind(socket_servidor, p->ai_addr, p->ai_addrlen) == -1) {
        	log_error(logger, "Error: No se pudo bindear el socket");
        	close(socket_servidor);
            continue;
        }
        else
        	log_info(logger, "Socket bindeado");
        break;
    }

	listen(socket_servidor, SOMAXCONN);

    freeaddrinfo(servinfo);

    log_trace(logger, "Listo para escuchar a mi cliente");

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
