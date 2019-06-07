/*
 * ConfigKernel.c
 *
 *  Created on: 3 jun. 2019
 *      Author: utnso
 */


#include "ConfigKernel.h"

t_config_kernel *cargarConfig(char *ruta){
	puts("!!!Hello World!!!");

	log_info(logger,
			"Levantando archivo de configuracion del proceso Kernel \n");

	t_config_kernel* config = malloc(sizeof(t_config_kernel));
	t_config *kernelConfig = config_create(ruta);

	if (kernelConfig == NULL) {
		perror("Error ");

		log_error(loggerError, "Problema al abrir el archivo");
	}

	config->IP_MEMORIA = get_campo_config_string(kernelConfig, "IP_MEMORIA");
	config->PUERTO_MEMORIA = get_campo_config_int(kernelConfig,
			"PUERTO_MEMORIA");
	config->QUANTUM = get_campo_config_int(kernelConfig, "QUANTUM");
	config->MULTIPROCESAMIENTO = get_campo_config_int(kernelConfig,
			"MULTIPROCESAMIENTO");
	config->METADATA_REFRESH = get_campo_config_int(kernelConfig,
			"METADATA_REFRESH");
	config->SLEEP_EJECUCION = get_campo_config_int(kernelConfig,
			"SLEEP_EJECUCION");

	log_info(logger,
			"Archivo de configuracion del proceso Kernel levantado \n");

//	config_destroy(kernelConfig);  // Si lo ponemos, se pierden los datos

	return config;
}

t_dictionary *describeGlobal(char* IP_MEMORIA){
	log_info(logger, "Pidiendo metadata global");
	printf("Memoria request %s",IP_MEMORIA);
	t_dictionary *diccionario = dictionary_create();

	//hardcodeo datos

	//tabla1
		t_metadata_tabla * metaTabla1 = malloc(sizeof(t_metadata_tabla));
		metaTabla1->CANT_PARTICIONES = 5;
		metaTabla1->T_COMPACTACION = 5000;
		metaTabla1->CONSISTENCIA = STRONG;
		dictionary_put(diccionario,"tabla1",metaTabla1);
	//tabla2
		t_metadata_tabla * metaTabla2 = malloc(sizeof(t_metadata_tabla));
		metaTabla2->CANT_PARTICIONES = 10;
		metaTabla2->T_COMPACTACION = 4000;
		metaTabla2->CONSISTENCIA = STRONG_HASH;
		dictionary_put(diccionario,"tabla2",metaTabla2);
	//tabla3
		t_metadata_tabla * metaTabla3 = malloc(sizeof(t_metadata_tabla));
		metaTabla3->CANT_PARTICIONES = 40;
		metaTabla3->T_COMPACTACION = 3000;
		metaTabla3->CONSISTENCIA =EVENTUAL;
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

int obtenerMemSegunConsistencia(t_consistencia consistencia, int key){
	log_info(logger, "Obteniendo memoria segun consistencia");

	int memDestino = ERROR;

	if (consistencia==STRONG) {
		memDestino = criterios->SC;
		puts("SC");
	} else if (consistencia==STRONG_HASH) {
		//aplico el hash, seria key mod cantMemorias
		if (list_size(criterios->SHC) > 0) {
			int largo = list_size(criterios->SHC);
			//printf("la key es %d y largo %d",key,largo);
			memDestino = (key % largo) + 1;
		}
		puts("SHC");
	} else if (consistencia==EVENTUAL) {
		puts("EC");
		if (list_size(criterios->EC) > 0) {
			srand(time(NULL));
			memDestino = (rand() % list_size(criterios->EC)) + 1;
		}

	}
	return memDestino;
}

t_consistencia getConsistencia(char *nombreTabla){
	t_consistencia consistencia;
	t_metadata_tabla * metaTabla = dictionary_get(metadataTablas, nombreTabla);
	if (metaTabla != NULL) {
		consistencia = metaTabla->CONSISTENCIA;
		return consistencia;
	} else {
		return 0;
	}
}

t_criterios * inicializarCriterios(){
	t_criterios *criterios = malloc(sizeof(t_criterios));
	t_list *SHC = list_create();
	t_list *EC = list_create();

	criterios->SC = ERROR;
	criterios->SHC = SHC;
	criterios->EC = EC;

	return criterios;
}

int obtenerMemDestino(char *tabla, int key){
	t_consistencia consistencia;
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
		if (criterios->SC == ERROR) {
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

void escriboComandosHardcodeados(FILE* fichero, char* path) {
	//ESCRITURA
	fichero = fopen(path, "wt");
	fprintf(fichero, "%s %c", "Nueva prueba de fgets (03)", '\n');
	fprintf(fichero, "%s %c", "Veamos si lee los siguientes datos", '\n');
	fprintf(fichero, "%s %c", "SELECT TABLA1 3", '\n');
	fprintf(fichero, "%s %c", "CREATE TABLA1 SC 4 60000", '\n');
	fprintf(fichero, "%s %c", "DESCRIBE", '\n');
	fprintf(fichero, "%s %c", "DESCRIBE TABLA1", '\n');
	fprintf(fichero, "%s %c", "JOURNAL", '\n');
	fprintf(fichero, "%s %c", "METRICS", '\n');
	fprintf(fichero, "%s %c", "ADD MEMORY 3 TO SC", '\n');
	fclose(fichero);
}

int contarLineasArchivo(FILE* fichero, char* path) {
	fichero = fopen(path, "rt");

	if(fichero==NULL)
		return 0;
	int caracter, num_lineas = 0;
	while ((caracter = fgetc(fichero)) != EOF)
		if (caracter == '\n')
			num_lineas++;
	fclose(fichero);
	return num_lineas;
}

void comandoRun(char*path){
	info.script = NULL;
//	char**info.script = NULL;
	FILE* fd = NULL;
//	escriboComandosHardcodeados(fd, path2);
	int num_lineas = contarLineasArchivo(fd, path);

	// Inicializar puntero
	info.script = (char**) malloc(num_lineas * sizeof(char*));

	if(num_lineas==0)
		printf("El path no es correcto o el archivo esta vacio\n");
	else{
		fd = fopen(path, "rt");
			for (int i = 0; i < num_lineas; i++) {

				// Inicializo punteros del puntero matriz
				char ejemplo[200];
				fgets(ejemplo, sizeof(ejemplo), fd);
				int tam = strlen(ejemplo) + 1;
				info.script[i] = (char*) malloc(tam);
				strcpy(info.script[i], ejemplo);

				printf("El valor es: %s", ejemplo);
		//		list_add(listaInfoProcesos, info);
			}
			fclose(fd);

			// Libero memoria, por ahora no la libero a proposito porque necesito la info

		//	for (int i = 0; i < num_lineas; i++) {
		//		free(info.script[i]);
		//	}
		//	free(info.script);
		//	info.script=NULL;

	}
}

