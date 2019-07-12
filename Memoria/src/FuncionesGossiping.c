#include "FuncionesGossiping.h"

void cargarListaSeeds() {
	int i = 0;
	while (configuracion->PUERTOS_SEEDS[i] != NULL) {
		t_memoria* seedMemoria = malloc(sizeof(t_memoria));
		seedMemoria->ip = string_duplicate(configuracion->IP_SEEDS[i]);
		seedMemoria->puerto = string_duplicate(configuracion->PUERTOS_SEEDS[i]);
		list_add(seeds, seedMemoria);
		i++;
	}
}

void cargarEstructurasGossiping() {
	seeds = list_create();
	cargarListaSeeds();

	//La memoria en una primera instancia se conoce a si misma
	tablaGossiping = list_create();
	t_memoria* memoria = malloc(sizeof(t_memoria));
	memoria->ip = string_duplicate(configuracion->IP_ESCUCHA);
	memoria->puerto = string_duplicate(configuracion->PUERTO_ESCUCHA);

	list_add(tablaGossiping, memoria);
}

bool isMemoriaYaConocida(t_memoria* memoriaRecibida) {
	bool equalsMemoria(t_memoria* memoriaConocida) {
		if (strcmp(memoriaRecibida->ip, memoriaConocida->ip) == 0 && strcmp(memoriaRecibida->puerto, memoriaConocida->puerto) == 0) {
			free(memoriaRecibida->ip);
			free(memoriaRecibida->puerto);
			return true;
		}
		return false;
	}
	return list_any_satisfy(tablaGossiping, (void*) equalsMemoria);
}

void agregarMemoriaNueva(t_memoria* memoriaRecibida) {
	if (!isMemoriaYaConocida(memoriaRecibida)) {
		list_add(tablaGossiping, memoriaRecibida);
	} else {
		free(memoriaRecibida);
	}
}

t_memoria* deserealizarMemoria(char* mensaje) {
	char* memoriaChar = string_duplicate(mensaje);
	char** datos = string_split(mensaje, " ");
	t_memoria* memoriaRecibida = malloc(sizeof(t_memoria));
	memoriaRecibida->ip = string_duplicate(datos[0]);
	memoriaRecibida->puerto = string_duplicate(datos[1]);
	freePunteroAPunteros(datos);
	free(memoriaChar);
	return memoriaRecibida;
}
