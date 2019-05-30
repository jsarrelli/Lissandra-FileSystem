/*
 * sockets.h
 *
 *  Created on: 2 may. 2019
 *      Author: utnso
 */

#ifndef SOCKETS_H_
#define SOCKETS_H_

#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<unistd.h>
#include<netdb.h>
#include"commons/log.h"
#include"commons/collections/list.h"
#include "commons/collections/queue.h"
#include<string.h>
#include<signal.h>
#include <fcntl.h>

#include <readline/readline.h>
#include <readline/history.h>

#include <pthread.h>
#include <semaphore.h>

#define HEADER_SIZE 4

typedef enum
{
	MENSAJE,
	PAQUETE_DEFAULT
}op_code;

typedef struct
{
	int size;
	void* stream;
} t_buffer;

typedef struct
{
	op_code codigo_operacion;
	t_buffer* buffer;
} t_paquete;

sem_t sem_socket;

/*
 * Funciones genericas de los sockets
 */
void iterator(char* value);
void enviar_info(int socket_cliente, t_log* log_master, op_code tipo, void* info);

void* serializar_paquete(t_paquete* paquete, int bytes);
int recibir_operacion(int socket_cliente);
void crear_buffer(t_paquete* paquete);
//void enviar_mensaje_string(int socket_cliente, t_log *logger, char *mensaje);
t_paquete* crear_paquete(op_code tipo);
void agregar_a_paquete(t_paquete* paquete, void* valor, int tamanio);
void enviar_paquete(t_paquete* paquete, int socket_cliente, t_log *logger);
void eliminar_paquete(t_paquete* paquete);
void liberar_conexion(int socket_cliente);
void _leer_consola_haciendo(void(*accion)(char*)) ;
void leer_consola(t_log* logger);
t_paquete* armar_paquete(op_code tipo);
//int iniciar_cliente(char *ip, char* puerto, t_log* logger);
struct hostent* configurar_cliente(char* ip, t_log *log_master);
void connect_to_server(int fd, const struct sockaddr_in* server, t_log *log_master);
void connect_server(struct sockaddr_in* server, int puerto, int fd, struct hostent* info, t_log *log_master);
int iniciar_cliente(struct sockaddr_in* server, int puerto, char* ip, t_log* log_master);
int aceptar_conexion(int socket_servidor, t_log *logger, int numConexion);
void* recibir_buffer(int* size, int socket_cliente);
void recibir_mensaje(int socket_cliente, t_log *logger);
t_list* recibir_paquete(int socket_cliente);
void configurar_server(int puerto, struct sockaddr_in* server);
int crear_socket(t_log* logger);
void bindear_server(int fd, const struct sockaddr_in* server,t_log* logger);
void listen_server(int fd, t_log* logger);
int iniciar_servidor(int puerto, struct sockaddr_in* server, t_log* logger);
void enviar_por_socket(int socket_cliente, t_log* log_master, op_code tipo);

int fork_tipo_paquete(int socket_cliente, t_log *log_master, t_list* lista);
int recv_to(int socket_cliente, t_list* lista, int len, int flags, int to, t_log *log_master);

/*
 * Este es el IP de la VM de la casa de mama: 192.168.0.249
 * 
 * Por ahora voy a usar el localHost: 127.0.0.1
 */

#endif /* SOCKETS_H_ */
