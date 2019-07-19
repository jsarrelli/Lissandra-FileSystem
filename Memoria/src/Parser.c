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
		JOURNAL_MEMORIA();
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

	t_registro_memoria* registro = SELECT_MEMORIA(nombreTabla, key);
	freePunteroAPunteros(valores);
	return registro;

}

void* procesarINSERT(char* consulta) {
	log_info(loggerInfo, "INSERT : %s",consulta);
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

	t_registro_memoria* registro = INSERT_MEMORIA(nombreTabla, key, value, timeStamp);
	freePunteroAPunteros(valoresAux);
	freePunteroAPunteros(valores);
	free(value);
	return registro;

}

int procesarCREATE(char* consulta) {
	log_info(loggerInfo, "Parseando CREATE %s",consulta);
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
	return success;

}

void procesarDESCRIBE(char* nombreTabla) {

	if (nombreTabla == NULL) {
		char* tablasSerializadas = DESCRIBE_ALL_MEMORIA();
		free(tablasSerializadas);

	} else {
		t_metadata_tabla* metaData = DESCRIBE_MEMORIA(nombreTabla);
		free(metaData);
	}

}

