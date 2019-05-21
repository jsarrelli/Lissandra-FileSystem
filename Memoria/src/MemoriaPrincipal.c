#include "MemoriaPrincipal.h"

int LIBRE = 0;
int OCUPADO = 1;

int tamanioMemoria;
int tamanioPagina = 128;
int cantPaginas;

//inicializa el vector memoriaStatus
void inicializarEstadoMemoria() {
	memoriaStatus = list_create();
	for (int i = 0; i < cantPaginas - 1; i++) {
		EstadoMemoria* estadoMemoria = malloc(sizeof(EstadoMemoria));
		estadoMemoria->estado = 0;
		estadoMemoria->fechaObtencion = 0;
		list_add(memoriaStatus, estadoMemoria);
	}
}

void inicializarMemoria(int valueMaximoRecibido, int tamanioMemoriaRecibido) {
	tamanioMemoria = tamanioMemoriaRecibido;
	cantPaginas = tamanioMemoria / tamanioPagina;
	memoria = malloc(tamanioMemoria);
	valueMaximo = valueMaximoRecibido;
	segmentos = list_create();
	inicializarEstadoMemoria();
}

void insertarSegmentoEnMemoria(char nombreSegmento[20]) {

	Segmento* segmento = malloc(sizeof(Segmento));
	segmento->paginas = list_create();
	strcpy(segmento->nombreTabla, nombreSegmento);
	segmento->nombreModificado = 0;
	list_add(segmentos, segmento);

}

//inserta una pagina en la memoria y te devuelve la direccion de
//donde la puso
Pagina* insertarPaginaEnMemoria(int key, char value[112], char nombreSegmento[20]) {
	Segmento* segmento = buscarSegmentoEnMemoria(nombreSegmento);
	Pagina* paginaNueva = NULL;
	if (existeSegmentoEnMemoria(nombreSegmento) && !existePaginaEnMemoria(segmento, key)) {
		t_registro registro = { .key = key, .value = value, .timestamp = getCurrentTime() };
		void* marcoVacio = NULL;
		if (memoriaLlena()) {
			if (!todosModificados()) {
				marcoVacio = (void*) liberarUltimoUsado();
			} else {
				//jouranal
			}
		}

		if (marcoVacio == NULL) {
			marcoVacio = darMarcoVacio();
		}

		t_registro* registroEnMemoria = memcpy(marcoVacio, &registro,
				sizeof(t_registro));

		paginaNueva = malloc(sizeof(Pagina));
		paginaNueva->modificado = 0;
		paginaNueva->registro = registroEnMemoria;

		list_add(segmento->paginas, paginaNueva);
	}
	return paginaNueva;
}

void* darMarcoVacio() {
	for (int i = 0; i < list_size(memoriaStatus); i++) {
		if (((EstadoMemoria*) list_get(memoriaStatus, i))->estado
				== LIBRE) {
			return memoria + i * sizeof(t_registro);
		}
	}
	return NULL;
}

Pagina* buscarPaginaEnMemoria(Segmento* segmento, int key) {
	if (existeSegmentoEnMemoria(segmento->nombreTabla)) {
		for (int i = 0; i < list_size(segmento->paginas); i++) {
			Pagina* paginaActual = (Pagina*) list_get(segmento->paginas, i);
			if (paginaActual->registro->key == key) {
				return paginaActual;
			}
		}
	}
	return NULL;
}

Pagina* buscarPagina(Segmento* segmento, int key) {
	Pagina* pagina = buscarPaginaEnMemoria(segmento, key);
	if (pagina == NULL) {

		t_registro* registro; //= pegale a kevin
		insertarPaginaEnMemoria(registro->key, registro->value, segmento->nombreTabla);

	}
	return pagina;
}

Segmento* buscarSegmentoEnMemoria(char nombreSegmento[20]) {
	for (int i = 0; i < list_size(segmentos); i++) {
		Segmento* segmentoActual = (Segmento*) list_get(segmentos, i);
		if (strcmp(segmentoActual->nombreTabla, nombreSegmento)) {
			return segmentoActual;
		}
	}
	return NULL;
}

//cuando busca un segmento primero buscamos en memoria y si no lo tenemos
//lo vamos a buscar al fileSystem
Segmento* buscarSegmento(char nombreSegmento[20]) {
	Segmento* segmento = buscarSegmentoEnMemoria(nombreSegmento);
	if (segmento == NULL) {
		//pegale al fileSystem
		//segmento= algo que nos devuelte el fileSystem
		insertarSegmentoEnMemoria(nombreSegmento);
	}
	return segmento;
}

bool existeSegmentoEnMemoria(char nombreSegmento[20]) {
	return buscarSegmentoEnMemoria(nombreSegmento) != NULL;
}

bool existePaginaEnMemoria(Segmento* segmento, int key) {
	return buscarPaginaEnMemoria(segmento, key) != NULL;
}

bool memoriaLlena() {
	return list_is_empty(list_filter(memoriaStatus, (void*) estaLibre));
}

bool estaLibre(EstadoMemoria* estado) {
	return estado->estado == LIBRE;
}

bool todosModificados() {
	for (int i = 0; i < list_size(segmentos); i++) {
		t_list* paginasSegmento = list_get(segmentos, i);

		for (int j = 0; j < list_size(paginasSegmento); j++) {
			Pagina* pagina = list_get(paginasSegmento, j);
			if (pagina->modificado == 0) {
				return false;
			}
		}
	}
	return true;
}

void* liberarUltimoUsado() {
	int indiceFrameMenosUsado = 0;
	EstadoMemoria* frameMenosUtilizado;
	for (int i = 0; i < list_size(memoriaStatus); i++) {
		EstadoMemoria* estadoMemoriaActual = list_get(memoriaStatus, i);
		frameMenosUtilizado = list_get(memoriaStatus, indiceFrameMenosUsado);
		if (estadoMemoriaActual->fechaObtencion <= frameMenosUtilizado->fechaObtencion) {
			indiceFrameMenosUsado = i;
		}
	}
	frameMenosUtilizado->estado = 0;
	return memoria + indiceFrameMenosUsado * sizeof(t_registro);
}

