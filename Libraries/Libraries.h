
#define LIBRARIES_H_

#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdint.h>
#include <stdio.h>
#include <errno.h>
#include <stdbool.h>
#include <string.h>
#include <commons/string.h>
#include <commons/error.h>
#include <commons/config.h>
#include <commons/collections/list.h>
#include <commons/bitarray.h>
#include <commons/txt.h>
#include <commons/log.h>
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
#include <signal.h>

typedef struct t_registro{
	int key;
	char* value;
	long timestamp;
}t_registro;


int get_campo_config_int(t_config* archivo_configuracion, char* nombre_campo);

char** get_campo_config_array(t_config* archivo_configuracion,
		char* nombre_campo);

char* get_campo_config_string(t_config* archivo_configuracion,
		char* nombre_campo);

void logErrorAndExit(char * mensaje);
void inicializarArchivoDeLogs(char * ruta);
int contarPunteroDePunteros(char ** puntero);
void liberarPunteroDePunterosAChar(char** palabras);
void crearHilo(pthread_t * nombreHilo, void * nombreFuncion, void * parametros);

t_log* logger;
t_log* loggerError;
