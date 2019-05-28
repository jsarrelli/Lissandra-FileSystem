#include "API.h"
void procesarConsulta(char consulta[100]) {
	char* consultaAux=malloc(100);
	strcpy(consultaAux,consulta);
	char* comando = strtok(consultaAux, " ");

	if (strcmp(comando, "SELECT") == 0) {
		procesarSELECT(consulta);
	} else if (strcmp(comando, "INSERT") == 0) {
		//insert
	} else if (strcmp(comando, "CREATE") == 0) {
		//create
	} else if (strcmp(comando, "DESCRIBE") == 0) {
		//describe
	} else if (strcmp(comando, "DROP") == 0) {
		//drop
	} else if (strcmp(comando, "JOURNAL") == 0) {
		//journnal
	} else {
		puts("Comando no encontrado");
	}
}

void procesarSELECT(char consulta[100]) {
	char comando[10];
	char nombreTabla[20];
	int key;
	sscanf(consulta, "%s %s %d", comando, nombreTabla, &key);

	t_registro* registro = SELECT_MEMORIA(nombreTabla, key);
	if(registro!=NULL){
		printf(" %d : %s \n", registro->key, registro->value);
	}else{
		puts("Registro no encontrado");
	}

}
