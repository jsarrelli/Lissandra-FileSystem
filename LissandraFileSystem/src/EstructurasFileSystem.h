#ifndef EstructurasFileSystem_H_
#define EstructurasFileSystem_H_

#include <commons/log.h>
#include <commons/collections/list.h>

typedef struct {
	char* PUERTO_ESCUCHA;
	char* PUNTO_MONTAJE;
	int RETARDO;
	int TAMANIO_VALUE;
	int TIEMPO_DUMP;
} t_configuracion_LFS;

typedef struct {
	char tabla[100];
	t_list* registros;
} t_tabla_memtable;

typedef struct {
	int TAMANIO;
	t_list* BLOQUES;
	int cantBloques;
} t_archivo;

t_list* memtable;
t_log* logger;
t_log* loggerError;
t_configuracion_LFS* config;
t_configuracion_LFS* cargarConfig(char* ruta);

#endif /* EstructurasFileSystem_H_*/
