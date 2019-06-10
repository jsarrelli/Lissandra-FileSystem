/*
 ============================================================================
 Name        : Kernel.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include "Kernel.h"

// Cargo paths constantes

static const char* INFO_KERNEL =
		"/home/utnso/tp-2019-1c-Los-Sisoperadores/Kernel/infoKernel.log";
static const char* ERRORES_KERNEL =
		"/home/utnso/tp-2019-1c-Los-Sisoperadores/Kernel/erroresKernel.log";
static const char* RUTA_CONFIG_KERNEL =
		"/home/utnso/tp-2019-1c-Los-Sisoperadores/Kernel/src/config_kernel.cfg";

int main(void) {

	inicializarArchivoDeLogs((char*) ERRORES_KERNEL);
	inicializarArchivoDeLogs((char*) INFO_KERNEL);

	listaInfoProcesos = list_create();

	logger = log_create((char*) INFO_KERNEL, "Kernel Info Logs", 1, LOG_LEVEL_INFO);
	loggerError = log_create((char*) ERRORES_KERNEL, "Kernel Error Logs", 1,
			LOG_LEVEL_ERROR);

	config = cargarConfig((char*) RUTA_CONFIG_KERNEL);
	criterios = inicializarCriterios();

	//Conocer las memorias del pool, por gossiping -> Ejemplo hardcodeado
	poolMemorias = conocerPoolMemorias(config->IP_MEMORIA);
	//COMANDO ADD MEMORY [NÚMERO] TO [CRITERIO]
	//PRIMERO VERIFICAR SI EXISTE CADA MEMORIA

	//Cada METADATA_REFRESH hacer
	metadataTablas = describeGlobal(config->IP_MEMORIA);
	// -> Ejemplo hardcodeado
	//t_metadata_tabla * metaTabla1 = dictionary_get(metadataTablas,"tabla1");

	//Comando RUN <path>, por ahora simple, despues aplicar RR
	//Levanto LQL de <path>
	//INSERT TABLA1 1 “Mi nombre es Lissandra”
	//Fijarme a que memoria mandarlo segun la consistencia de TABLA1
	/*
	 key=2;

	 memDestino = obtenerMemDestino("tabla1",key);
	 if(memDestino != -1){
	 printf("La memoria destino es la %d\n", memDestino);
	 }else{
	 printf("No se pudo obtener la memoria destino.");
	 }
	 */

	// Empezar a conectarse con memoria

//	log_info(logger, "Conectandose a memoria..");
//	int socketMemoria = ConectarAServidor(config->PUERTO_MEMORIA, config->IP_MEMORIA);
//	log_info(logger, "Memoria conectada");
//
//	char* consulta ="INSERT TABLA1 3 'CASA'";
//	EnviarDatosTipo(socketMemoria, KERNEL, consulta, strlen(consulta),INSERT);


	deNewAReady(logger);

//	printf("El multiprocesamiento extraido del archivo de configuracion es: %d\n", config->MULTIPROCESAMIENTO);

	consolaKernel(socketMemoria);

	printf("%d", info.num_requests);

	// Libero memoria

//	dictionary_destroy(poolMemorias);
	dictionary_destroy_and_destroy_elements(metadataTablas, (void*) free);
//	log_destroy(logger);
//	log_destroy(loggerError);
	free(config);
	list_destroy(listaInfoProcesos);

	return EXIT_SUCCESS;
}
