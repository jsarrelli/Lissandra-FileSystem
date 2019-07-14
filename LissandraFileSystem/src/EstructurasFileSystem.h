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
	char* tabla;
	t_list* registros;
} t_tabla_memtable;

typedef struct {
	int TAMANIO;
	t_list* BLOQUES;
	int cantBloques;
} t_archivo;

t_list* memtable;
t_log* loggerInfo;
t_log* loggerError;



#endif /* EstructurasFileSystem_H_*/
