#ifndef SERIALIZACION_H_
#define SERIALIZACION_H_
#include <stdlib.h>
#include <commons/string.h>
#include <commons/bitarray.h>
#include <commons/collections/list.h>
#include <readline/readline.h>
#include <stdbool.h>
#include <netinet/in.h>



//////////////////////////////////////////
//           Tipos de Mensajes          //
//////////////////////////////////////////
typedef enum t_protocolo {
	HANDSHAKE = 1,
	SELECT,
	INSERT,
	CREATE,
	DESCRIBE,
	DESCRIBE_ALL,
	DROP,
	JOURNAL,
	NOTFOUND,
	SELECT_TABLE,
	CONEXION_INICIAL_FILESYSTEM_MEMORIA,
	GOSSIPING,
	TABLA_GOSSIPING
} t_protocolo;

//////////////////////////////////////////
//               Procesos               //
//////////////////////////////////////////

typedef enum proceso {
	KERNEL = 1,
	FILESYSTEM,
	MEMORIA
} proceso;

//////////////////////////////////////////
//           Comunicacion Base          //
//////////////////////////////////////////

typedef struct {
	t_protocolo tipoMensaje;
	proceso quienEnvia;
	int tamanioMensaje;
}__attribute__((packed)) Header;

typedef struct {
	Header header;
	void* mensaje;
}__attribute__((packed)) Paquete;



//////////////////////////////////////////
//              Funciones               //
//////////////////////////////////////////

char* getNombreDelProceso(proceso proceso);

bool EnviarHandshake(int socketFD, proceso quienEnvia);

bool EnviarDatosTipo(int socketFD, proceso quienEnvia, void* datos, int tamDatos, t_protocolo tipoMensaje);

bool EnviarPaquete(int socketCliente, Paquete* paquete);

int RecibirDatos(void* paquete, int socketFD, uint32_t cantARecibir);

int RecibirPaqueteServidor(int socketFD, proceso quienEnvia, Paquete* paquete); //Responde al recibir un Handshake

int RecibirPaqueteCliente(int socketFD, proceso quienEnvia, Paquete* paquete); //No responde los Handshakes

int RecibirPaquete(int socketFD,Paquete* paquete);//Generico


#endif /* SERIALIZACION_H_ */
