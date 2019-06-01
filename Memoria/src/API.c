#include "API.h"
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
		//journnal
	} else {
		puts("Comando no encontrado");
	}
}

void procesarSELECT(char* argumentos) {
	char** valores = string_split(argumentos, " ");
	char* nombreTabla = valores[0];
	int key = atoi(valores[1]);

	t_registro* registro = SELECT_MEMORIA(nombreTabla, key);
	if (registro != NULL) {
		printf(" %d : %s \n", registro->key, registro->value);
	} else {
		puts("Registro no encontrado");
	}

}

void procesarINSERT(char* consulta) {
	char** valores = string_split(consulta, "'");
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

	INSERT_MEMORIA(nombreTabla, key, value, timeStamp);
	printf("Se ha insertado el siguiente registro: %d %s en la tabla %s", key, value, nombreTabla);
}

void procesarCREATE(char* consulta) {
	char** valores = string_split(consulta, " ");
	char* nombreTabla = valores[0];
	char* consistencia = valores[1];
	int cantParticiones = atoi(valores[2]);
	int tiempoCompactacion = atoi(valores[3]);

	CREATE_MEMORIA(nombreTabla, consistencia, cantParticiones, tiempoCompactacion);
	printf("Se ha creado la tabla %s", nombreTabla);

}

void procesarDROP(char* nombreTabla) {
	DROP_MEMORIA(nombreTabla);
	printf("Se ha eliminado la tabla: %s", nombreTabla);
}

void procesarDESCRIBE(char* nombreTabla) {

	void mostrarMetadata(t_metadata_tabla* metadata) {
		printf("Consistencia: %s / cantParticiones: %d / tiempoCompactacion: %d", metadata->CONSISTENCIA,
				metadata->CANT_PARTICIONES, metadata->T_COMPACTACION);
	}

	if (strcmp(nombreTabla, "")) {
		t_list* tablas = DESCRIBE_ALL_MEMOROIA();
		list_iterate(tablas, (void*) mostrarMetadata);
	} else {
		t_metadata_tabla* metadata = DESCRIBE_MEMORIA(nombreTabla);
		mostrarMetadata(metadata);
	}

}

