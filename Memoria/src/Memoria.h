#include <Libraries.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include <commons/string.h>
#include <stdio.h>
#include <stdlib.h>


#include "SocketServidor.h"


typedef struct MEMORIA_configuracion{
	char* PUERTO_ESCUCHA;
	char* IP_FS;
	int PUERTO_FS;
	char* IP_SEEDS;
	char* PUERTOS_SEEDS;
	int RETARDO_MEMORIA;
	int RETARDO_FS;
	int TAM_MEMORIA;
	int TIEMPO_JOURNAL;
	int TIEMPO_GOSSIPING;
	int MEMORY_NUMBER;
}MEMORIA_configuracion;

MEMORIA_configuracion configuracion;

MEMORIA_configuracion get_configuracion();

//char* pathMEMConfig = "/home/utnso/tp-2019-1c-Los-Sisoperadores/Memoria/configMEM.cfg";
char* pathMEMConfig = "configMEM.cfg";

t_log* logger;


