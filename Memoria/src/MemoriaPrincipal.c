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

void inicializarMemoria(int valueMaximoRecibido, int tamanioMemoriaRecibido, int socketFileSystemRecibido) {
	tamanioRegistro = sizeof(int) + sizeof(double) + valueMaximoRecibido;
	tamanioMemoria = tamanioMemoriaRecibido;
	cantFrames = tamanioMemoria / tamanioRegistro;
	memoria = malloc(tamanioMemoria);
	valueMaximo = valueMaximoRecibido;
	socketFileSystem = socketFileSystemRecibido;

	segmentos = list_create();
	inicializarEstadoMemoria();
	//recemosle a Dios y a la virgen
	pthread_mutex_init(&lock, NULL);
}

Segmento* newSegmento(char* nombreSegmento) {
	Segmento* segmento = malloc(sizeof(Segmento));
	segmento->paginas = list_create();
	segmento->nombreTabla = malloc(strlen(nombreSegmento) + 1);
	strcpy(segmento->nombreTabla, nombreSegmento);
	return segmento;
}

Segmento* insertarSegmentoEnMemoria(char* nombreSegmento) {

	Segmento* segmento = newSegmento(nombreSegmento);
	list_add(segmentos, segmento);
	return segmento;

}

bool validarValueMaximo(char* value) {
	if (strlen(value) > valueMaximo) {
		log_info(logger, "No se puede insertar el registro ya que el value excede el tamanio maximo");
		return false;
	}
	return true;
}

//inserta una pagina en la memoria y te devuelve la direccion de
//donde la puso
Pagina* insertarPaginaEnMemoria(int key, char* value, double timeStamp, Segmento* segmento) {
log_info(logger, "Insertando pagina en memoria..");
	Pagina* paginaNueva = buscarPaginaEnMemoria(segmento, key);
	if (paginaNueva == NULL) {
		log_info(logger, "Insertando registro en memoria..");
		t_registro registro;
		registro.key = key;
		registro.timestamp = timeStamp;
		registro.value = malloc(valueMaximo);
		strcpy(registro.value, value);

		void* marcoVacio = NULL;
		if (memoriaLlena()) {
			log_info(logger, "Memoria llena");
			if (!todosModificados()) {
				marcoVacio = (void*) liberarUltimoUsado();
			} else {
				journalMemoria();
				//fijate aca
				if(list_is_empty(segmentos)){
					log_info(logger, "Tenes vacia la lista de segmentos pancho");
					log_info(logger, "%s",segmento->nombreTabla);
					list_add(segmentos,segmento);
				}
			}
		}

		if (marcoVacio == NULL) {
			marcoVacio = darMarcoVacio();
		}

		t_registro* registroEnMemoria = memcpy(marcoVacio, &registro, tamanioRegistro);

		paginaNueva = malloc(sizeof(Pagina));
		paginaNueva->modificado = MODIFICADO;
		paginaNueva->registro = registroEnMemoria;

		list_add(segmento->paginas, paginaNueva);
	} else if(paginaNueva->registro->timestamp < timeStamp) {
		log_info(logger, "Esta key ya se encuentra en el sistema con un timeStamp menor, se actualizara");
		free(paginaNueva->registro->value);
		paginaNueva->registro->value = string_duplicate(value);
		paginaNueva->registro->timestamp = timeStamp;
		paginaNueva->modificado = MODIFICADO;
	}else{
		log_info(logger, "Ya se encuentra en sistema un registro mas actualizado");
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
	log_info(logger, "Buscando pagina en memoria..");
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
	log_info(logger, "Buscando pagina");
	Pagina* pagina = buscarPaginaEnMemoria(segmento, key);
	if (pagina == NULL) {

		t_registro* registro = selectFileSystem(segmento, key);
		if (registro == NULL) {
			return NULL;
		}
		pagina = insertarPaginaEnMemoria(registro->key, registro->value, registro->timestamp, segmento);
		freeRegistro(registro);
	}

	EstadoFrame* estadoFrame = getEstadoFrame(pagina);
	estadoFrame->fechaObtencion = getCurrentTime();

	return pagina;
}

Segmento* buscarSegmentoEnMemoria(char* nombreSegmentoBuscado) {
log_info(logger, "Buscando segmento: %s en memoria",nombreSegmentoBuscado);
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
 lo vamos a buscar al fileSystem
 y si el fileSystem no lo tiene, lo creamos (no la crea aca)
 */
Segmento* buscarSegmento(char* nombreSegmento) {
	log_info(logger, "Buscando segmento..");
	Segmento* segmento = buscarSegmentoEnMemoria(nombreSegmento);

	if (segmento == NULL) {

		t_metadata_tabla* metaData = describeSegmento(nombreSegmento);
		if (metaData != NULL) {
			segmento = insertarSegmentoEnMemoria(nombreSegmento);
			list_add(segmentos, segmento);
			free(metaData);
		}

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
	log_info(logger, "Aplicando algortimo LRU");
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
	log_info(logger, "Se eliminara la pagina con key: %d y timeStamp:%f por ser la menos accedida", paginaMenosUtilizada->registro->key,
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
	log_info(logger, "Liberando frame");
	EstadoFrame* estadoFrame = getEstadoFrame(paginaAEliminar);
	estadoFrame->estado = LIBRE;
}

void freePagina(Pagina* pagina) {
	if (pagina->registro != NULL) {
	//	freeRegistro(pagina->registro);
		log_info(logger, "Haciendo free de registro");
	}

	//free(pagina);
	log_info(logger, "Pagina liberada, ponele..");
}

void eliminarPaginaDeMemoria(Pagina* paginaAEliminar) {

	clearFrameDePagina(paginaAEliminar);
	freePagina(paginaAEliminar);

}

void eliminarSegmentoDeMemoria(Segmento* segmentoAEliminar) {

	list_destroy_and_destroy_elements(segmentoAEliminar->paginas, (void*) eliminarPaginaDeMemoria);
	log_info(logger, "Paginas de %s eliminadas",segmentoAEliminar->nombreTabla);
	freeSegmento(segmentoAEliminar);
}

void freeSegmento(Segmento* segmentoAEliminar) {
	log_info(logger, "Haciendo free Segmento");
	if (segmentoAEliminar->nombreTabla != NULL) {
		free(segmentoAEliminar->nombreTabla);
	}
	free(segmentoAEliminar);
}

void vaciarMemoria() {
	log_info(logger, "Vaciando memoria..");
	list_clean_and_destroy_elements(segmentos, (void*) eliminarSegmentoDeMemoria);
	log_info(logger, "Memoria Vacia");
}

void finalizarMemoria() {
	vaciarMemoria();
	//list_destroy(segmentos);
}

bool isModificada(Pagina* pagina) {
	EstadoFrame* estadoFrame = getEstadoFrame(pagina);
	return estadoFrame->estado == (int) MODIFICADO;
}

t_list* obtenerPaginasModificadasFromSegmento(Segmento* segmento) {

	t_list* paginasModificadas = list_filter(segmento->paginas, (void*) isModificada);
	return paginasModificadas;
}

void journalMemoria() {
	pthread_mutex_lock(&lock);
	log_info(logger, "Realizando Journal..");

	void enviarSiEstaModificada(Pagina* pagina, Segmento* segmento) {
		if (isModificada(pagina)) {
			enviarRegistroAFileSystem(pagina, segmento->nombreTabla);
			log_info(logger, "Enviando registro a FileSystem");
		}
	}

	void journalPaginasModificadasBySegmento(Segmento* segmento) {
		if (existeSegmentoFS(segmento->nombreTabla)) {
			list_iterate2(segmento->paginas, (void*) enviarSiEstaModificada, segmento);
		} else {
			log_info(logger, "La informacion del segmento  %s no se cargo en FS ya que el mismo no existia", segmento->nombreTabla);
		}

	}


	list_iterate(segmentos, (void*) journalPaginasModificadasBySegmento);

	vaciarMemoria();
	log_info(logger, "Journal finalizado con exito");
	pthread_mutex_unlock(&lock);
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

