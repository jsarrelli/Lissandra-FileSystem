#ifndef MEMORIA_H_
#define MEMORIA_H_

#include <Libraries.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include <commons/string.h>
#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <commons/config.h>
#include "MemoriaPrincipal.h"
#include "AdministradorDeConsultas.h"
#include "Sockets/Conexiones.h"
#include "Sockets/Serializacion.h"
#include "Parser.h"
#include "SocketServidorMemoria.h"
#include <Sockets/Conexiones.h>
#include "FuncionesGossiping.h"
#include <Inotify.h>

typedef struct MEMORIA_configuracion {
	char* PUERTO_ESCUCHA;
	char* IP_ESCUCHA;
	char* IP_FS;
	int PUERTO_FS;
	char** IP_SEEDS;
	char** PUERTOS_SEEDS;
	int RETARDO_MEMORIA;
	int RETARDO_FS;
	int TAM_MEMORIA;
	int TIEMPO_JOURNAL;
	int TIEMPO_GOSSIPING;
	int MEMORY_NUMBER;
} MEMORIA_configuracion;

char* archivoConfig;
MEMORIA_configuracion* configuracion;
pthread_t serverThread;
pthread_t intTemporalJournal;
pthread_t intTemporalGossiping;
t_config* archivo_configuracion;
int listenningSocket;
int valueMaximoPaginas;
t_list* tablaGossiping;
t_list* seeds;

void cargarConfiguracion();
void* leerConsola();
void procesoTemporalJournal();
void procesoTemporalGossiping();
void iniciarSocketServidor();
void liberarVariables();
void liberarDatosConfiguracion();
void seleccionarArchivoConfig();

#endif /*MEMORIA_H_ */
