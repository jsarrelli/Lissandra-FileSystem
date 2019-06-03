/*
 ============================================================================
 Name        : Kernel.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include "Libraries.h"

#include "API_kernel.h"
#include "kernel.h"


// Cargo paths constantes

static const char* INFO_KERNEL =
		"/home/utnso/tp-2019-1c-Los-Sisoperadores/Kernel/infoKernel.log";
static const char* ERRORES_KERNEL =
		"/home/utnso/tp-2019-1c-Los-Sisoperadores/Kernel/erroresKernel.log";
static const char* RUTA_CONFIG_KERNEL =
		"/home/utnso/tp-2019-1c-Los-Sisoperadores/Kernel/src/config_kernel.cfg";



t_dictionary *describeGlobal(char* IP_MEMORIA){
	log_info(logger, "Pidiendo metadata global");
	printf("Memoria request %s",IP_MEMORIA);
	t_dictionary *diccionario = dictionary_create();
	//hardcodeo datos
	//tabla1
		t_metadata_tabla * metaTabla1 = malloc(sizeof(t_metadata_tabla));
		metaTabla1->CANT_PARTICIONES = 5;
		metaTabla1->T_COMPACTACION = 5000;
		metaTabla1->CONSISTENCIA = "SC";
		dictionary_put(diccionario,"tabla1",metaTabla1);
	//tabla2
		t_metadata_tabla * metaTabla2 = malloc(sizeof(t_metadata_tabla));
		metaTabla2->CANT_PARTICIONES = 10;
		metaTabla2->T_COMPACTACION = 4000;
		metaTabla2->CONSISTENCIA = "SHC";
		dictionary_put(diccionario,"tabla2",metaTabla2);
	//tabla3
		t_metadata_tabla * metaTabla3 = malloc(sizeof(t_metadata_tabla));
		metaTabla3->CANT_PARTICIONES = 40;
		metaTabla3->T_COMPACTACION = 3000;
		metaTabla3->CONSISTENCIA = "EC";
		dictionary_put(diccionario,"tabla3",metaTabla3);

	return diccionario;
}

t_dictionary *conocerPoolMemorias(char* IP_MEMORIA){
	log_info(logger, "Pidiendo pool de memorias");
	//printf("Memoria request %s",IP_MEMORIA);
	t_dictionary *diccionario = dictionary_create();
	//hardcodeo datos
	//mem1
		t_memoria * memoria1 = malloc(sizeof(t_memoria));
		memoria1->IP_MEMORIA = "192.168.1.2";
		memoria1->NUMERO_MEMORIA = 1;
		dictionary_put(diccionario,"1",memoria1);
	//mem2
		t_memoria * memoria2 = malloc(sizeof(t_memoria));
		memoria2->IP_MEMORIA = "192.168.1.3";
		memoria2->NUMERO_MEMORIA = 2;
		dictionary_put(diccionario,"2",memoria2);
	return diccionario;
}

int obtenerMemSegunConsistencia(char *consistencia, int key){
	log_info(logger, "Obteniendo memoria segun consistencia");

	int memDestino = -1;

	if (strcmp(consistencia, "SC") == 0) {
		memDestino = criterios->SC;
		puts("SC");
	} else if (strcmp(consistencia, "SHC") == 0) {
		//aplico el hash, seria key mod cantMemorias
		if (list_size(criterios->SHC) > 0) {
			int largo = list_size(criterios->SHC);
			//printf("la key es %d y largo %d",key,largo);
			memDestino = (key % largo) + 1;
		}
		puts("SHC");
	} else if (strcmp(consistencia, "EC") == 0) {
		puts("EC");
		if (list_size(criterios->EC) > 0) {
			srand(time(NULL));
			memDestino = (rand() % list_size(criterios->EC)) + 1;
		}

	}
	return memDestino;
}

char * getConsistencia(char *nombreTabla){
	char *consistencia;
	t_metadata_tabla * metaTabla = dictionary_get(metadataTablas, nombreTabla);
	if (metaTabla != NULL) {
		consistencia = metaTabla->CONSISTENCIA;
		return consistencia;
	} else {
		return NULL;
	}
}

t_criterios * inicializarCriterios(){
	t_criterios *criterios = malloc(sizeof(t_criterios));
	t_list *SHC = list_create();
	t_list *EC = list_create();

	criterios->SC = -1;
	criterios->SHC = SHC;
	criterios->EC = EC;

	return criterios;
}

int obtenerMemDestino(char *tabla, int key){
	char * consistencia;
	consistencia = getConsistencia(tabla);
	if (consistencia != NULL) {
		return obtenerMemSegunConsistencia(consistencia, key);
	} else {
		return -1;
	}
}

// Viene de ADD MEMORY 1 TO SC
void add(int numeroMem, char *criterio){
//	int i = -1;
	if (strcmp(criterio, "SC") == 0) {
		if (criterios->SC == -1) {
			criterios->SC = numeroMem;
		} else {
			printf("El criterio SC ya tiene la memoria %d asignada.",
					criterios->SC);
		}
	} else if (strcmp(criterio, "SHC") == 0) {
		//t_list * SHC = criterios->SHC;
//		i = list_add(criterios->SHC,numeroMem);
		list_add((t_list *) criterios->SHC, (void*) numeroMem);

	} else if (strcmp(criterio, "EC") == 0) {
//		i = list_add(criterios->EC,numeroMem);
		list_add((t_list *) criterios->EC, (void*) numeroMem);
	}
	/*
	//mostrar los elementos
	puts("\nMemorias en EC: ");
	for(i=0;i<list_size(criterios->EC);i++){
		m = list_get(criterios->EC,i);
		printf(" , %d ,",m);
	}
	puts("\nMemorias en SHC: ");
	for(i=0;i<list_size(criterios->SHC);i++){
		m = list_get(criterios->SHC,i);
		printf(" , %d ,",m);
	}
	free(i);
	*/
}

int main(void) {

	// Mandar esto a un header -> variable global
	t_config_kernel *config;

//	char *rutaConfig = RUTA_CONFIG;

	inicializarArchivoDeLogs((char*)ERRORES_KERNEL);
	inicializarArchivoDeLogs((char*)INFO_KERNEL);

	logger = log_create((char*)INFO_KERNEL, "Kernel Info Logs", 1, LOG_LEVEL_INFO);
	loggerError = log_create((char*)ERRORES_KERNEL, "Kernel Error Logs", 1,
			LOG_LEVEL_ERROR);

	config = cargarConfig((char*)RUTA_CONFIG_KERNEL);
	criterios = inicializarCriterios();

	//Conocer las memorias del pool
		poolMemorias = conocerPoolMemorias(config->IP_MEMORIA);
	//COMANDO ADD MEMORY [NÚMERO] TO [CRITERIO]
	//PRIMERO VERIFICAR SI EXISTE CADA MEMORIA

	//Cada METADATA_REFRESH hacer
		metadataTablas = describeGlobal(config->IP_MEMORIA);
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

	if(configurarSocketCliente()){

		INSERT(32, "juli", "tabla1");
	}
	close(serverSocket);
	*/

	consolaKernel();

	dictionary_destroy(poolMemorias);

	return EXIT_SUCCESS;
}
