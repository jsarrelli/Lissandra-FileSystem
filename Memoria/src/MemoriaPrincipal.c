#include "MemoriaPrincipal.h"

int tamanioMemoria;
int cantFrames;
t_list* segmentos;

//inicializa el vector memoriaStatus
void inicializarEstadoMemoria() {
	memoriaStatus = list_create();
	int i;
	for (i = 0; i < cantFrames; i++) {
		EstadoFrame* estadoMemoria = malloc(sizeof(EstadoFrame));
		estadoMemoria->estado = LIBRE;
		estadoMemoria->fechaObtencion = 0;
		list_add(memoriaStatus, estadoMemoria);
	}
}

void inicializarMemoria(int tamanioMemoriaRecibido) {
	tamanioRegistro = sizeof(int) + sizeof(double) + valueMaximo;
	tamanioMemoria = tamanioMemoriaRecibido;
	cantFrames = tamanioMemoria / tamanioRegistro;
	memoria = malloc(tamanioMemoria);

	segmentos = list_create();
	inicializarEstadoMemoria();
	//recemosle a Dios y a la virgen
	pthread_mutex_init(&lockMemoria, NULL);
}

Segmento* newSegmento(char* nombreSegmento) {
	Segmento* segmento = malloc(sizeof(Segmento));
	segmento->paginas = list_create();
	segmento->nombreTabla = string_duplicate(nombreSegmento);
	return segmento;
}

Segmento* insertarSegmentoEnMemoria(char* nombreSegmento) {
	Segmento* segmento = newSegmento(nombreSegmento);
	list_add(segmentos, segmento);
	return segmento;
}

bool validarValueMaximo(char* value) {
	if (strlen(value) > valueMaximo) {
		log_error(loggerError, "No se puede insertar el registro ya que el value excede el tamanio maximo");
		return false;
	}
	return true;
}

//inserta una pagina en la memoria y te devuelve la direccion de
//donde la puso
Pagina* insertarPaginaEnMemoria(int key, char* value, double timeStamp, Segmento* segmento, bool modificado) {

	log_info(loggerInfo, "Insertando pagina en memoria..");
	Pagina* paginaNueva = buscarPaginaEnMemoria(segmento, key);
	if (paginaNueva == NULL) {
		log_info(loggerInfo, "Insertando registro en memoria..");
		void* marcoVacio = NULL;
		if (memoriaLlena()) {
			log_info(loggerInfo, "Memoria llena");
			if (!todosModificados()) {
				marcoVacio = (void*) liberarUltimoUsado();
			} else {
				journalMemoria();
			}
		}

		if (marcoVacio == NULL) {
			marcoVacio = darMarcoVacio();
		}

		t_registro_memoria* registro = malloc(tamanioRegistro);
		registro->key = key;

		strcpy(registro->value, value);

		registro->timestamp = timeStamp;
		t_registro_memoria* registroEnMemoria = memcpy(marcoVacio, registro, tamanioRegistro);

		paginaNueva = malloc(sizeof(Pagina));
		paginaNueva->modificado = modificado;
		paginaNueva->registro = registroEnMemoria;

		list_add(segmento->paginas, paginaNueva);
	} else {
		log_info(loggerInfo, "Esta key ya se encuentra en el sistema, se actualizara");
		strcpy(paginaNueva->registro->value, value);
		paginaNueva->registro->timestamp = timeStamp;
		paginaNueva->modificado = modificado;
	}

	EstadoFrame* estadoFrame = getEstadoFrame(paginaNueva);
	estadoFrame->estado = OCUPADO;
	estadoFrame->fechaObtencion = getCurrentTime();

	return paginaNueva;
}

void* darMarcoVacio() {
	int i;
	//no hay algun list_find_first?
	for (i = 0; i < list_size(memoriaStatus); i++) {
		if (((EstadoFrame*) list_get(memoriaStatus, i))->estado == LIBRE) {
			return memoria + (i * tamanioRegistro);
		}
	}
	return NULL;
}

Pagina* buscarPaginaEnMemoria(Segmento* segmento, int keyBuscada) {
	log_info(loggerInfo, "Buscando pagina en memoria..");
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
	log_info(loggerInfo, "Buscando pagina..");
	Pagina* pagina = buscarPaginaEnMemoria(segmento, key);
	if (pagina == NULL) {

		t_registro* registro = selectFileSystem(segmento, key);
		if (registro == NULL) {
			return NULL;
		}
		pagina = insertarPaginaEnMemoria(registro->key, registro->value, registro->timestamp, segmento, false);
		freeRegistro(registro);
	}
	return pagina;
}

Segmento* buscarSegmentoEnMemoria(char* nombreSegmentoBuscado) {
	log_info(loggerInfo, "Buscando segmento: %s en memoria", nombreSegmentoBuscado);
	bool isSegmentoBuscado(Segmento* segmentoActual) {
		if (strcmp(segmentoActual->nombreTabla, nombreSegmentoBuscado) == 0) {
			return true;
		}
		return false;
	}

	return (Segmento*) list_find(segmentos, (void*) isSegmentoBuscado);

}

//te devuelve el estadoFrame de determinada pagina
//HERMOSA
EstadoFrame* getEstadoFrame(Pagina* pagina) {
	int calculo = (void*) pagina->registro - memoria;
	int indiceFrame = calculo / tamanioRegistro;
	return list_get(memoriaStatus, (int) indiceFrame);
}

/*cuando busca un segmento primero buscamos en memoria y si no lo tenemos
 lo creamos
 */
Segmento* buscarSegmento(char* nombreSegmento) {
	log_info(loggerInfo, "Buscando segmento..");
	Segmento* segmento = buscarSegmentoEnMemoria(nombreSegmento);

	if (segmento == NULL) {
		segmento = insertarSegmentoEnMemoria(nombreSegmento);
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
	bool todosModificados = true;

	void tienePaginasModificadas(Segmento* segmento) {
		todosModificados = list_all_satisfy(segmento->paginas, (void*) isModificada);
		if (todosModificados == false) {
			return;
		}
	}
	list_iterate(segmentos, (void*) tienePaginasModificadas);
	return todosModificados;
}

void* liberarUltimoUsado() {
	log_info(loggerInfo, "Aplicando algortimo LRU");
	EstadoFrame* frameMenosUtilizado = list_get(memoriaStatus, 0);
	Pagina* paginaMenosUtilizada;
	Segmento* segmentoPaginaMenosUtilizada;
	Segmento* segmentoActual;

	void LRUPagina(Pagina* pagina) {
		EstadoFrame* frameDePagina = getEstadoFrame(pagina);
		if (frameDePagina->fechaObtencion < frameMenosUtilizado->fechaObtencion && frameDePagina->estado == LIBRE) {
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
	log_info(loggerInfo, "Se eliminara la pagina con key: %d y timeStamp:%f por ser la menos accedida", paginaMenosUtilizada->registro->key,
			paginaMenosUtilizada->registro->timestamp);

	reemplazarPagina(paginaMenosUtilizada, segmentoPaginaMenosUtilizada);

	return frameMenosUtilizado;
}

void reemplazarPagina(Pagina* pagina, Segmento* segmento) {
	bool isPaginaEliminar(Pagina* paginaAEliminar) {
		return pagina->registro->key == paginaAEliminar->registro->key;
	}
	list_remove_and_destroy_by_condition(segmento->paginas, (void*) isPaginaEliminar, (void*) eliminarPaginaDeMemoria);

}

void clearFrameDePagina(Pagina* paginaAEliminar) {
	log_info(loggerInfo, "Liberando frame");
	EstadoFrame* estadoFrame = getEstadoFrame(paginaAEliminar);
	estadoFrame->estado = LIBRE;
}

void freePagina(Pagina* pagina) {

	free(pagina->registro->value);
	log_info(loggerInfo, "Haciendo free de registro");

	free(pagina);
	log_info(loggerInfo, "Pagina liberada, ponele..");
}

void eliminarPaginaDeMemoria(Pagina* paginaAEliminar) {

	clearFrameDePagina(paginaAEliminar);
	free(paginaAEliminar);

}

void eliminarSegmentoDeMemoria(Segmento* segmentoAEliminar) {
	log_info(loggerInfo, "Eliminando segmento: %s de memoria..", segmentoAEliminar->nombreTabla);

	list_destroy_and_destroy_elements(segmentoAEliminar->paginas, (void*) eliminarPaginaDeMemoria);
	log_info(loggerInfo, "Paginas de %s eliminadas", segmentoAEliminar->nombreTabla);

	bool isSegmentoBuscado(Segmento* segmentoActual) {
		return strcmp(segmentoActual->nombreTabla, segmentoAEliminar->nombreTabla) == 0;
	}
	list_remove_and_destroy_by_condition(segmentos, (void*) isSegmentoBuscado, (void*) freeSegmento);
	log_info(loggerInfo, "Segmento eliminado");
}

void freeSegmento(Segmento* segmentoAEliminar) {
	log_info(loggerInfo, "Haciendo free Segmento");
	if (segmentoAEliminar->nombreTabla != NULL) {
		free(segmentoAEliminar->nombreTabla);
	}
	free(segmentoAEliminar);
}

/*te deja la memoria con una lista de segmentos vacia
 y todos los frames en estado disponible*/
void vaciarMemoria() {
	log_info(loggerInfo, "Vaciando memoria..");
	list_clean_and_destroy_elements(segmentos, (void*) eliminarSegmentoDeMemoria);
	log_info(loggerInfo, "Memoria Vacia");
}

void finalizarMemoria() {
	vaciarMemoria();
	list_destroy(segmentos);

}

bool isModificada(Pagina* pagina) {
	return pagina->modificado == MODIFICADO;
}

t_list* obtenerPaginasModificadasFromSegmento(Segmento* segmento) {

	t_list* paginasModificadas = list_filter(segmento->paginas, (void*) isModificada);
	return paginasModificadas;
}

void journalMemoria() {
	pthread_mutex_lock(&lockMemoria);

	log_info(loggerInfo, "Realizando Journal..");

	void enviarSiEstaModificada(Pagina* pagina, Segmento* segmento) {

		if (isModificada(pagina)) {
			enviarRegistroAFileSystem(pagina, segmento->nombreTabla);
			log_info(loggerInfo, "Enviando registro a FileSystem..");
		}
		clearFrameDePagina(pagina);
	}

	void journalPaginasModificadasBySegmento(Segmento* segmento) {
		if (existeSegmentoFS(segmento->nombreTabla)) {
			list_iterate2(segmento->paginas, (void*) enviarSiEstaModificada, segmento);
		} else {
			log_info(loggerInfo, "La informacion del segmento  %s no se cargo en FS ya que el mismo no existia", segmento->nombreTabla);
		}

		list_clean_and_destroy_elements(segmento->paginas, free);
	}

	list_iterate(segmentos, (void*) journalPaginasModificadasBySegmento);

	log_info(loggerInfo, "Journal finalizado con exito");
	pthread_mutex_unlock(&lockMemoria);
}

bool existeSegmentoFS(char* nombreSegmento) {
	t_metadata_tabla* metadata = describeSegmento(nombreSegmento);
	if (metadata != NULL) {
		free(metadata);
		return true;
	}
	free(metadata);
	return false;
}

