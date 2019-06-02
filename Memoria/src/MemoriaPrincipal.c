#include "MemoriaPrincipal.h"

int LIBRE = 0;
int OCUPADO = 1;

int tamanioMemoria;
int cantFrames;

//inicializa el vector memoriaStatus
void inicializarEstadoMemoria() {
	memoriaStatus = list_create();
	int i;
	for (i = 0; i < cantFrames - 1; i++) {
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

	segmentos = list_create();
	inicializarEstadoMemoria();
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
Pagina* insertarPaginaEnMemoria(int key, char value[112], Segmento* segmento, double timeStamp) {
	Pagina* paginaNueva = buscarPaginaEnMemoria(segmento, key);
	if (paginaNueva == NULL) {

		t_registro registro;
		registro.key = key;

		registro.timestamp = timeStamp;
		strcpy(registro.value, value);

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

		list_add(segmento->paginas, paginaNueva);
	} else {
		paginaNueva->registro->timestamp = timeStamp;
		strcpy(paginaNueva->registro->value, value);
		paginaNueva->modificado = 1;
	}

	EstadoFrame* estadoFrame = getEstadoFrame(paginaNueva);
	estadoFrame->estado = 1;
	estadoFrame->fechaObtencion = timeStamp;

	return paginaNueva;
}

void* darMarcoVacio() {
	int i;
	for (i = 0; i < list_size(memoriaStatus); i++) {
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
	return list_find(segmento->paginas, (void*) isPaginaBuscada);
}

Pagina* buscarPagina(Segmento* segmento, int key) {
	Pagina* pagina = buscarPaginaEnMemoria(segmento, key);
	if (pagina == NULL) {

		t_registro* registro; //= pegale a kevin
		insertarPaginaEnMemoria(registro->key, registro->value, segmento,getCurrentTime());

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

	}
	return segmento;
}

bool memoriaLlena() {
	bool estaLibre(EstadoFrame* estado) {
		return estado->estado == LIBRE;
	}
	return !list_any_satisfy(memoriaStatus, (void*) estaLibre);

}

bool todosModificados() {
	int i;
	for (i = 0; i < list_size(segmentos); i++) {
		Segmento* segmento = list_get(segmentos, i);
		t_list* paginasSegmento = segmento->paginas;

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
	EstadoFrame* frameMenosUtilizado = list_get(memoriaStatus, 0);
	Pagina* paginaMenosUtilizada;
	Segmento* segmentoPaginaMenosUtilizada;
	Segmento* segmentoActual;

	void LRUPagina(Pagina* pagina) {
		EstadoFrame* frameDePagina = getEstadoFrame(pagina);
		if (frameDePagina->fechaObtencion < frameMenosUtilizado->fechaObtencion &&
				frameDePagina->estado != 1) {
			frameMenosUtilizado = frameDePagina;
			segmentoPaginaMenosUtilizada = segmentoActual;
			paginaMenosUtilizada = pagina;
		}
	}
	void iterarEntrePaginas(Segmento* segmento) {
		segmentoActual = segmento;
		list_iterate(segmento->paginas, (void*) LRUPagina);

	}

	list_iterate(segmentos, (void*) iterarEntrePaginas);

	eliminarPaginaDeMemoria(paginaMenosUtilizada, segmentoPaginaMenosUtilizada);

	return frameMenosUtilizado;
}

void eliminarPaginaDeMemoria(Pagina* paginaAEliminar, Segmento* segmento) {

	EstadoFrame* estadoFrame = getEstadoFrame(paginaAEliminar);
	estadoFrame->estado = 0;

	bool isPagina(Pagina* pagina) {
		return pagina->registro->key == paginaAEliminar->registro->key;
	}

	void freePagina(Pagina* pagina) {
		free(pagina);
	}

	list_remove_and_destroy_by_condition(segmento->paginas, (void*) isPagina, (void*) freePagina);

}

void eliminarSegmentoDeMemoria(char nombreSegmento[20]) {
	Segmento* segmentoAEliminar = buscarSegmentoEnMemoria(nombreSegmento);

	if (segmentoAEliminar != NULL) {
		//podria usar aca un list_clean and destroy pero voy a estar cargando mas funciones parecidas
		void eliminarPaginaSegmento(Pagina* pagina) {
			eliminarPaginaDeMemoria(pagina, segmentoAEliminar);
		}
		list_iterate(segmentoAEliminar->paginas, (void*) eliminarPaginaSegmento);
		list_destroy(segmentoAEliminar->paginas);
		if (segmentoAEliminar->metaData != NULL) {
			free(segmentoAEliminar->metaData);
		}
		free(segmentoAEliminar);

	}

}

