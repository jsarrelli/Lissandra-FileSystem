#include "MemoriaPrincipal.h"

int LIBRE = 0;
int OCUPADO = 1;

int tamanioMemoria;
int cantFrames;

//inicializa el vector memoriaStatus
void inicializarEstadoMemoria() {
	memoriaStatus = list_create();
	for (int i = 0; i < cantFrames - 1; i++) {
		EstadoFrame* estadoMemoria = malloc(sizeof(EstadoFrame));
		estadoMemoria->estado = 0;
		estadoMemoria->fechaObtencion = 0;
		list_add(memoriaStatus, estadoMemoria);
	}
}

void inicializarMemoria(int valueMaximoRecibido, int tamanioMemoriaRecibido) {
	tamanioMemoria = tamanioMemoriaRecibido;
	cantFrames = tamanioMemoria / sizeof(t_registro);
	memoria = malloc(tamanioMemoria);
	valueMaximo = valueMaximoRecibido;

	segmentos = obtenerSegmentosDeFileSystem();
	inicializarEstadoMemoria();
}

t_list* obtenerSegmentosDeFileSystem() {
	/*uso la estrategia de fetch:eager, porque el enunciado no es claro
	 y por lo poco que dice creo que se refiere a este
	 y la realidad es que lo simplifica bastante*/
	segmentos = list_create();
	//aca pedile a kevin que nos pase las tablas que hay ahi
	return segmentos;
}

Segmento* insertarSegmentoEnMemoria(char nombreSegmento[20], t_metadata_tabla* metaData) {

	Segmento* segmento = malloc(sizeof(Segmento));
	segmento->paginas = list_create();
	strcpy(segmento->nombreTabla, nombreSegmento);
	segmento->nombreModificado = 0;
	segmento->metaData = metaData;
	list_add(segmentos, segmento);
	return segmento;

}

//inserta una pagina en la memoria y te devuelve la direccion de
//donde la puso
//el segmento tiene que existir
Pagina* insertarPaginaEnMemoria(int key, char value[112], Segmento* segmento) {
	Pagina* paginaNueva = buscarPaginaEnMemoria(segmento, key);
	if (paginaNueva == NULL) {

		t_registro registro;
		registro.key=key;
		registro.timestamp= getCurrentTime();
		strcpy(registro.value,value);

		void* marcoVacio = NULL;
		if (memoriaLlena()) {
			if (!todosModificados()) {
				marcoVacio = (void*) liberarUltimoUsado();
			} else {
				//journal
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

		EstadoFrame* estadoFrame = getEstadoFrame(paginaNueva);
		estadoFrame->estado = 1;
		estadoFrame->fechaObtencion = getCurrentTime();

		list_add(segmento->paginas, paginaNueva);
	} else {
		paginaNueva->registro->timestamp = getCurrentTime();
		strcpy(paginaNueva->registro->value, value);
		paginaNueva->modificado = 1;
	}

	return paginaNueva;
}

void* darMarcoVacio() {
	for (int i = 0; i < list_size(memoriaStatus); i++) {
		if (((EstadoFrame*) list_get(memoriaStatus, i))->estado
				== LIBRE) {
			return memoria + (i * sizeof(t_registro));
		}
	}
	return NULL;
}

Pagina* buscarPaginaEnMemoria(Segmento* segmento, int keyBuscada) {
	bool isPaginaBuscada(Pagina* pagina) {
		if (pagina->registro->key == keyBuscada) {
			EstadoFrame* estadoFrame = getEstadoFrame(pagina);
			estadoFrame->fechaObtencion = getCurrentTime();
			return true;
		}
		return false;
	}
	return list_find(segmento->paginas, (void*)isPaginaBuscada);
}

Pagina* buscarPagina(Segmento* segmento, int key) {
	Pagina* pagina = buscarPaginaEnMemoria(segmento, key);
	if (pagina == NULL) {

		t_registro* registro; //= pegale a kevin
		insertarPaginaEnMemoria(registro->key, registro->value, segmento);

	}
	return pagina;
}

Segmento* buscarSegmentoEnMemoria(char nombreSegmentoBuscado[20]) {

	bool isSegmentoBuscado(char nombreSegmento[20]) {
		if (strcmp(nombreSegmento, nombreSegmentoBuscado) == 0) {
			return true;
		}
		return false;
	}

	return list_find(segmentos, (void*) isSegmentoBuscado);

}

//te devuelve el estadoFrame de determinada pagina
//DUDOSISIMA
EstadoFrame* getEstadoFrame(Pagina* pagina) {
	int indiceFrame = ((int) (memoria - (void*) pagina->registro)) / sizeof(t_registro);
	return list_get(memoriaStatus, (int) indiceFrame);
}

/*cuando busca un segmento primero buscamos en memoria y si no lo tenemos
 lo vamos a buscar al fileSystem
 y si el fileSystem no lo tiene, lo creamos
 */
Segmento* buscarSegmento(char nombreSegmento[20]) {
	Segmento* segmento = buscarSegmentoEnMemoria(nombreSegmento);
	if (segmento == NULL) {
		//pegale al fileSystem
		//segmento= algo que nos devuelte el fileSystem

		insertarSegmentoEnMemoria(nombreSegmento, NULL);
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

bool estaLibre(EstadoFrame* estado) {
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
	EstadoFrame* frameMenosUtilizado=list_get(memoriaStatus,0);
	for (int i = 0; i < list_size(memoriaStatus); i++) {
		EstadoFrame* estadoMemoriaActual = list_get(memoriaStatus, i);
		if (estadoMemoriaActual->fechaObtencion <= frameMenosUtilizado->fechaObtencion) {
			frameMenosUtilizado=estadoMemoriaActual;
			indiceFrameMenosUsado = i;
		}
	}
	frameMenosUtilizado->estado = 0;
	return memoria + indiceFrameMenosUsado * sizeof(t_registro);
}

void eliminarSegmento(char nombreSegmento[20]) {
	Segmento* segmentoAEliminar = buscarSegmentoEnMemoria(nombreSegmento);
	if (segmentoAEliminar != NULL) {

		list_iterate(segmentoAEliminar->paginas, eliminarPagina);
		list_destroy_and_destroy_elements(segmentoAEliminar->paginas);
		list_remove_and_destroy_element()

	}

}

void eliminarPaginaDeMemoria(Pagina* pagina) {
	void* indiceRegistroEnMemoria = (&(pagina->registro) - &memoria) / sizeof(t_registro);
	EstadoFrame* estadoMemoria = list_get(memoriaStatus, indiceRegistroEnMemoria);
	estadoMemoria->estado = 0;
	estadoMemoria->fechaObtencion = 0;
}

