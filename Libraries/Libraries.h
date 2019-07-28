#ifndef LIBRARIES_H_
#define LIBRARIES_H_

#include <stdlib.h>
#include <malloc.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdint.h>
#include <stdio.h>
#include <errno.h>
#include <stdbool.h>
#include <string.h>
#include "commons/string.h"
#include "commons/error.h"
#include <commons/config.h>
#include "commons/collections/list.h"
#include "commons/bitarray.h"
#include "commons/txt.h"
#include "commons/log.h"
#include <unistd.h>
#include <readline/readline.h>
#include <sys/time.h>
#include <assert.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/if_link.h>
#include <ifaddrs.h>
#include <stdarg.h>
#include <math.h>
#include <pthread.h>
#include <dirent.h>
#include <signal.h>

typedef struct {
	int key;
	char* value;
	double timestamp;

}__attribute__((packed)) t_registro;

typedef enum t_consistencia {
	EVENTUAL = 1,
	STRONG,
	STRONG_HASH
} t_consistencia;

typedef struct {
	t_consistencia CONSISTENCIA;
	int CANT_PARTICIONES;
	int T_COMPACTACION;
} t_metadata_tabla;

int get_campo_config_int(t_config* archivo_configuracion, char* nombre_campo);
char** get_campo_config_array(t_config* archivo_configuracion, char* nombre_campo);
char* get_campo_config_string(t_config* archivo_configuracion, char* nombre_campo);
void logErrorAndExit(char * mensaje);
void inicializarArchivoDeLogs(char * ruta);
int contarPunteroDePunteros(char ** puntero);
void liberarPunteroDePunterosAChar(char** palabras);
void crearHilo(pthread_t * nombreHilo, void * nombreFuncion, void * parametros);
int cantidadParametros(char ** palabras);
int tamanioArchivo(FILE*archivo);
double getCurrentTime();
t_consistencia getConsistenciaByChar(char* consistenciaChar);
char* getConsistenciaCharByEnum(t_consistencia consistencia);
//SOCKETS

int crear_socket_cliente(char * ip, char * puerto);
int recibirConexion(int socket_servidor);
int crear_socket_servidor(char *puerto);
char* recibir_string(int socket_aceptado);
void enviar_string(int socket, char* mensaje);
void enviar(int socket, void* cosaAEnviar, int tamanio);
void* recibir(int socket);
void *get_in_addr(struct sockaddr *sa);
void list_iterate2(t_list* self, void (*closure)(void*,void*), void* segundoParametro);
char * obtenerUltimoElementoDeUnSplit(char ** palabras);
void freePunteroAPunteros(char** palabras);
t_registro* registro_new(char** valores);
t_registro* registro_duplicate(t_registro* registro);
void freeRegistro(t_registro* registro);

t_log* loggerInfo;
t_log* loggerError;

#endif /* LIBRARIES_H_*/
