#include "Parser.h"
void procesarConsulta(char* consulta) {
	char** comandos = string_n_split(consulta, 2, " ");
	char* operacion = comandos[0];
	char* argumentos = comandos[1];

	if (strcmp(operacion, "SELECT") == 0) {
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
	freePunteroAPunteros(comandos);
	free(consulta);

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
	char* value = string_duplicate(valores[1]);

	double timeStamp;
	if (valores[2] == NULL) {
		timeStamp = getCurrentTime();
	} else {
		timeStamp = atof(valores[2]);
	}

	t_registro* registro = INSERT_MEMORIA(nombreTabla, key, value, timeStamp);
	freePunteroAPunteros(valoresAux);
	freePunteroAPunteros(valores);
	free(value);
	return registro;

}

int procesarCREATE(char* consulta) {
	char** valores = string_split(consulta, " ");
	char* nombreTabla = valores[0];
	char* consistenciaChar = valores[1];
	int cantParticiones = atoi(valores[2]);
	int tiempoCompactacion = atoi(valores[3]);
	t_consistencia consistencia = getConsistenciaByChar(consistenciaChar);

	int succes = CREATE_MEMORIA(nombreTabla, consistencia, cantParticiones, tiempoCompactacion);
	freePunteroAPunteros(valores);
	return succes;
}

int procesarDROP(char* nombreTabla) {
	int success = DROP_MEMORIA(nombreTabla);
	if (success == 0) {
		printf("Se ha eliminado la tabla: %s \n \n", nombreTabla);
	} else {
		printf("No se ha podido eliminar la tabla: %s \n \n", nombreTabla);
	}
	return success;

}

void procesarDESCRIBE(char* nombreTabla) {

	if (nombreTabla == NULL) {
		t_list* tablas = DESCRIBE_ALL_MEMORIA();
		list_destroy_and_destroy_elements(tablas, free);

	} else {
		t_metadata_tabla* metaData = DESCRIBE_MEMORIA(nombreTabla);
		free(metaData);
	}

}

