#include "Parser.h"
void procesarConsulta(char* consulta) {
	char** comandos = string_n_split(consulta, 2, " ");
	char* operacion = comandos[0];
	char* argumentos = comandos[1];

	if (strcmp(operacion, "SELECT/0") == 0) {
		 procesarSELECT(argumentos);
	} else if (strcmp(operacion, "INSERT") == 0) {
		 procesarINSERT(argumentos);
	} else if (strcmp(operacion, "CREATE") == 0) {
		 procesarCREATE(argumentos);
	} else if (strcmp(operacion, "DESCRIBE") == 0) {
		 procesarDESCRIBE(argumentos);
	} else if (strcmp(operacion, "DROP") == 0) {
		procesarDROP(argumentos);
	} else if (strcmp(operacion, "JOURNAL") == 0) {
		journalMemoria();
	} else {
		puts("Comando no encontrado");
	}
	free(operacion);
	free(comandos);

}

void* procesarSELECT(char* argumentos) {
	char** valores = string_split(argumentos, " ");
	char* nombreTabla = valores[0];
	int key = atoi(valores[1]);

	t_registro* registro = SELECT_MEMORIA(nombreTabla, key);
	if (registro != NULL) {
		printf(" %d : %s \n", registro->key, registro->value);
	} else {
		puts("Registro no encontrado");
	}

	freePunteroAPunteros(valores);
	return registro;

}

void* procesarINSERT(char* consulta) {
	char** valores = string_split(consulta, "\""); //34 son las " en ASCII
	char** valoresAux = string_split(valores[0], " ");
	char* nombreTabla = valoresAux[0];
	int key = atoi(valoresAux[1]);
	char* value = valores[1];
	double timeStamp;
	if (valores[2] == NULL) {
		timeStamp = getCurrentTime();
	} else {
		timeStamp = atof(valores[2]);
	}

	freePunteroAPunteros(valores);
	return INSERT_MEMORIA(nombreTabla, key, value, timeStamp);

}

int procesarCREATE(char* consulta) {
	char** valores = string_split(consulta, " ");
	char* nombreTabla = valores[0];
	char* consistenciaChar = valores[1];
	int cantParticiones = atoi(valores[2]);
	int tiempoCompactacion = atoi(valores[3]);
	t_consistencia consistencia = getConsistenciaByChar(consistenciaChar);
	return CREATE_MEMORIA(nombreTabla, consistencia, cantParticiones, tiempoCompactacion);

}

void procesarDROP(char* nombreTabla) {
	DROP_MEMORIA(nombreTabla);
	printf("Se ha eliminado la tabla: %s", nombreTabla);
}

void procesarDESCRIBE(char* nombreTabla) {

	if (nombreTabla == NULL) {
		t_list* tablas = DESCRIBE_ALL_MEMORIA();
		list_destroy(tablas);

	} else {
		t_metadata_tabla* metaData = DESCRIBE_MEMORIA(nombreTabla);
		free(metaData);

	}

}

