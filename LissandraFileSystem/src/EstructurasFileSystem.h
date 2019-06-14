#ifndef EstructurasFileSystem_H_
#define EstructurasFileSyste_H_


typedef struct{
	char* PUERTO_ESCUCHA;
	char* PUNTO_MONTAJE;
	int RETARDO;
	int TAMANIO_VALUE;
	int TIEMPO_DUMP;
}t_configuracion_LFS;

typedef struct{
	char tabla[100];
	t_list* registros;
}t_tabla_memtable;

typedef struct{
	int TAMANIO;
	char **BLOQUES;
	int cantBloques;
}t_archivo;

t_list* memtable;
t_log* logger;
t_log* loggerError;
t_configuracion_LFS* config;
t_configuracion_LFS* cargarConfig (char* ruta);

#endif /* EstructurasFileSystem_H_*/
