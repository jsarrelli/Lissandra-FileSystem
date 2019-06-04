

#ifndef KERNEL_H_
#define KERNEL_H_

#include "Libraries.h" // Una de las librerias genericas
#include "API_kernel.h"

typedef struct{
	char* IP_MEMORIA;
	int PUERTO_MEMORIA;
	int QUANTUM;
	int MULTIPROCESAMIENTO;
	int METADATA_REFRESH;
	int SLEEP_EJECUCION;
}t_config_kernel;

/*
typedef struct{
	char* CONSISTENCIA;
	int CANT_PARTICIONES;
	int T_COMPACTACION;
}t_metadata_tabla;
*/

typedef struct{
	char* IP_MEMORIA;
	int NUMERO_MEMORIA;
}t_memoria;

typedef struct{
	int SC;
	t_list *SHC;
	t_list *EC;
}t_criterios;

t_dictionary *metadataTablas;
t_dictionary *poolMemorias;
t_criterios *criterios;

static const int ERROR = -1;

t_config_kernel* cargarConfig (char* ruta);
t_dictionary *describeGlobal(char* IP_MEMORIA);
t_dictionary *conocerPoolMemorias(char* IP_MEMORIA);
int obtenerMemSegunConsistencia(char *consistencia, int key);
char * getConsistencia(char *nombreTabla);
t_criterios * inicializarCriterios();
int obtenerMemDestino(char *tabla, int key);
void add(int numeroMem, char *criterio);


#endif /* KERNEL_H_ */
