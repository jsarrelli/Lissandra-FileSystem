#include "Parser.h"
void* procesarConsulta(char* consulta) {
	char** comandos = string_n_split(consulta, 2, " ");
	char* operacion = comandos[0];
	char* argumentos = comandos[1];

	if (strcmp(operacion, "SELECT/0") == 0) {
		return procesarSELECT(argumentos);
	} else if (strcmp(operacion, "INSERT") == 0) {
		 return procesarINSERT(argumentos);
	} else if (strcmp(operacion, "CREATE") == 0) {
		return procesarCREATE(argumentos);
	} else if (strcmp(operacion, "DESCRIBE") == 0) {
		return procesarDESCRIBE(argumentos);
	} else if (strcmp(operacion, "DROP") == 0) {
		return procesarDROP(argumentos);
	} else if (strcmp(operacion, "JOURNAL") == 0) {
		journalMemoria();
	} else {
		puts("Comando no encontrado");
	}
	free(comandos);
	return NULL;
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
	return NULL;

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

	INSERT_MEMORIA(nombreTabla, key, value, timeStamp);
	printf("Se ha insertado el siguiente registro: %d %s en la tabla %s \n", key, value, nombreTabla);
	return NULL;
}

void* procesarCREATE(char* consulta) {
	char** valores = string_split(consulta, " ");
	char* nombreTabla = valores[0];
	char* consistenciaChar = valores[1];
	int cantParticiones = atoi(valores[2]);
	int tiempoCompactacion = atoi(valores[3]);
	t_consistencia consistencia = getConsistenciaByChar(consistenciaChar);
	CREATE_MEMORIA(nombreTabla, consistencia, cantParticiones, tiempoCompactacion);

	return NULL;

}

void* procesarDROP(char* nombreTabla) {
	DROP_MEMORIA(nombreTabla);
	printf("Se ha eliminado la tabla: %s", nombreTabla);
	return NULL;
}

void* procesarDESCRIBE(char* nombreTabla) {

	void mostrarMetadata(char* nombreSegmento, t_metadata_tabla* metadata) {
		printf("Segmento: %s \n", nombreSegmento);
		printf("Consistencia: %s / cantParticiones: %d / tiempoCompactacion: %d \n", getConsistenciaCharByEnum(metadata->CONSISTENCIA),
				metadata->CANT_PARTICIONES, metadata->T_COMPACTACION);
	}

	void mostrarMetadataSerializada(char* tablaSerializada) {
		char* tablaSerializadaAux= malloc(strlen(tablaSerializada)+1);
		strcpy(tablaSerializadaAux,tablaSerializada);
		char** valores= string_split(tablaSerializada, " ");

			printf("Segmento: %s \n", valores[0]);
			printf("Consistencia: %s / cantParticiones: %s / tiempoCompactacion: %s \n", valores[1],
					valores[2], valores[3]);
			free(valores);
		}


	if (nombreTabla==NULL) {
		t_list* tablas = DESCRIBE_ALL_MEMORIA();
		list_iterate(tablas, (void*) mostrarMetadataSerializada);


	} else {
		t_metadata_tabla* metaData = DESCRIBE_MEMORIA(nombreTabla);
		if(metaData!=NULL){
			mostrarMetadata(nombreTabla,metaData);
		}else{
			printf("La tabla: %s no se encuentra en sistema" ,nombreTabla);
		}


	}

	return NULL;

}

t_registro procesarRegistro(char* cadenaRecibida){
	char** datos=string_split(cadenaRecibida, " ");
	int key=atoi(datos[0]);
	char* valueRecibido=datos[1];
	double timeStamp = atof(datos[2]);

	t_registro registro;
	registro.key=key;
	strcpy(registro.value,valueRecibido);
	registro.timestamp=timeStamp;
	return registro;

}



