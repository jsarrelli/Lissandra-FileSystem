

#ifndef KERNEL_H_
#define KERNEL_H_
#include "Libraries.h" // Una de las librerias genericas
#include <commons/log.h>
#include <commons/collections/dictionary.h>
#include <commons/config.h>

typedef struct{
	int SC;
	t_list *SHC;
	t_list *EC;
}t_criterios;

typedef struct{
	char* IP_MEMORIA;
	int NUMERO_MEMORIA;
}t_memoria;

typedef struct {
	char* IP_MEMORIA;
	int PUERTO_MEMORIA;
	int QUANTUM;
	int MULTIPROCESAMIENTO;
	int METADATA_REFRESH;
	int SLEEP_EJECUCION;
} t_config_kernel;

typedef struct{
	char**script;
	int num_requests;
}infoProceso;

#include "API_kernel.h"
#include "ProcesoExec.h"

#include <Sockets/Serializacion.h>
#include <Sockets/Conexiones.h>
/*
typedef struct{
	char* CONSISTENCIA;
	int CANT_PARTICIONES;
	int T_COMPACTACION;
}t_metadata_tabla;
*/





infoProceso info;

t_dictionary *metadataTablas;
t_dictionary *poolMemorias;
t_criterios *criterios;

t_config_kernel *config;

t_list* listaInfoProcesos;

static const int ERROR = -1;

//t_config_kernel* cargarConfig (char* ruta);
//t_dictionary *describeGlobal(char* IP_MEMORIA);
//t_dictionary *conocerPoolMemorias(char* IP_MEMORIA);
//int obtenerMemSegunConsistencia(char *consistencia, int key);
//char * getConsistencia(char *nombreTabla);
//t_criterios * inicializarCriterios();
//int obtenerMemDestino(char *tabla, int key);
//void add(int numeroMem, char *criterio);


#endif /* KERNEL_H_ */
