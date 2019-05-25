

#ifndef KERNEL_H_
#define KERNEL_H_

#include "Libraries.h"

typedef struct{
	char* IP_MEMORIA;
	int PUERTO_MEMORIA;
	int QUANTUM;
	int MULTIPROCESAMIENTO;
	int METADATA_REFRESH;
	int SLEEP_EJECUCION;
}t_config_kernel;
typedef struct{
	char* CONSISTENCIA;
	int CANT_PARTICIONES;
	int T_COMPACTACION;
}t_metadata_tabla;
typedef struct{
	char* IP_MEMORIA;
	int NUMERO_MEMORIA;
}t_memoria;
typedef struct{
	int SC;
	t_list *SHC;
	t_list *EC;
}t_criterios;

t_config_kernel* cargarConfig (char* ruta);

#endif /* KERNEL_H_ */
