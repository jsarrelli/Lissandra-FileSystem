#include "MemoriaPrincipal.h"

int tamanioMemoria;
int cantFrames;

//inicializa el vector memoriaStatus
void inicializarEstadoMemoria() {
	memoriaStatus = list_create();
	int i;
	for (i = 0; i < cantFrames - 1; i++) {
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
	logger = log_create("MEM_logs.txt", "MEMORIA Logs", true, LOG_LEVEL_DEBUG);
}

Segmento* insertarSegmentoEnMemoria(char* nombreSegmento, t_metadata_tabla* metaData) {

	Segmento* segmento = malloc(sizeof(Segmento));
	segmento->paginas = list_create();
	segmento->nombreTabla = malloc(strlen(nombreSegmento));
	strcpy(segmento->nombreTabla, nombreSegmento);
	segmento->metaData = malloc(sizeof(t_metadata_tabla));
	memcpy(segmento->metaData, metaData, sizeof(t_metadata_tabla));
	if (metaData != NULL) {
		free(metaData);
	}

	list_add(segmentos, segmento);
	return segmento;

}

//inserta una pagina en la memoria y te devuelve la direccion de
//donde la puso
Pagina* insertarPaginaEnMemoria(int key, char value[112], double timeStamp, Segmento* segmento) {
	Pagina* paginaNueva = buscarPaginaEnMemoria(segmento, key);
	if (paginaNueva == NULL) {
		log_info(logger, "Insertando registro en memoria");
		t_registro registro;
		registro.key = key;
		registro.timestamp = timeStamp;
		strcpy(registro.value, value);

		void* marcoVacio = NULL;
		if (memoriaLlena()) {
			log_info(logger, "Memoria llena");
			if (!todosModificados()) {
				marcoVacio = (void*) liberarUltimoUsado();
			} else {
				journalMemoria();
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
	} else {
		log_info(logger, "Esta key ya se encuentra en el sistema, se actualizara");
		paginaNueva->registro->timestamp = timeStamp;
		strcpy(paginaNueva->registro->value, value);
		paginaNueva->modificado = MODIFICADO;
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

		//quedaria INSERT TABLA KEY , le sumo dos por los espacios
		char* consulta = malloc(strlen(strlen("SELECT") + segmento->nombreTabla) + 4 + 2);
		sprintf(consulta, "SELECT %s %d", segmento->nombreTabla, key);

		EnviarDatosTipo(socketFileSystem, MEMORIA, consulta, strlen(consulta), SELECT);
		free(consulta);

		Paquete paquete;
		RecibirPaqueteCliente(socketFileSystem, FILESYSTEM, &paquete);

		if (paquete.header.tipoMensaje == NOTFOUND) {
			return NULL;
		}
		void* datos = malloc(paquete.header.tamanioMensaje);
		datos = paquete.mensaje;

		t_registro registro = procesarRegistro(datos);
		insertarPaginaEnMemoria(registro.key, registro.value, registro.timestamp, segmento);

	} else {
		EstadoFrame* estadoFrame = getEstadoFrame(pagina);
		estadoFrame->fechaObtencion = getCurrentTime();
	}
	return pagina;
}

Segmento* buscarSegmentoEnMemoria(char* nombreSegmentoBuscado) {

	bool isSegmentoBuscado(Segmento* segmentoActual) {
		if (strcmp(segmentoActual->nombreTabla, nombreSegmentoBuscado) == 0) {
			return true;
		}
		return false;
	}

	return list_find(segmentos, (void*) isSegmentoBuscado);

}

//te devuelve el estadoFrame de determinada pagina
//HERMOSA
EstadoFrame* getEstadoFrame(Pagina* pagina) {
	int calculo = (void*) pagina->registro - memoria;
	int indiceFrame = calculo / sizeof(t_registro);
	return list_get(memoriaStatus, (int) indiceFrame);
}

/*cuando busca un segmento primero buscamos en memoria y si no lo tenemos
 lo vamos a buscar al fileSystem
 y si el fileSystem no lo tiene, lo creamos (no la crea aca)
 */
Segmento* buscarSegmento(char* nombreSegmento) {
	Segmento* segmento = buscarSegmentoEnMemoria(nombreSegmento);

	if (segmento == NULL) {

		Segmento* segmentoAux = buscarSegmentoEnFileSystem(nombreSegmento);
		if (segmentoAux != NULL) {
			segmento = insertarSegmentoEnMemoria(segmentoAux->nombreTabla, segmentoAux->metaData);
			list_add(segmentos, segmento);
			freeSegmento(segmentoAux);
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
	eliminarPaginaDeMemoria(paginaMenosUtilizada, segmentoPaginaMenosUtilizada);

	return frameMenosUtilizado;
}

void eliminarPaginaDeMemoria(Pagina* paginaAEliminar, Segmento* segmento) {

	EstadoFrame* estadoFrame = getEstadoFrame(paginaAEliminar);
	estadoFrame->estado = LIBRE;

	bool isPagina(Pagina* pagina) {
		return pagina->registro->key == paginaAEliminar->registro->key;
	}

	void freePagina(Pagina* pagina) {
		free(pagina);
	}

	list_remove_and_destroy_by_condition(segmento->paginas, (void*) isPagina, (void*) freePagina);

}

void freeSegmento(Segmento* segmentoAEliminar) {
	if (list_size(segmentoAEliminar->paginas) > 0) {
		list_destroy(segmentoAEliminar->paginas);
	}

	if (segmentoAEliminar->metaData != NULL) {
		free(segmentoAEliminar->metaData);
	}
	free(segmentoAEliminar);
}

void eliminarSegmentoDeMemoria(Segmento* segmentoAEliminar) {

	//podria usar aca un list_clean and destroy pero voy a estar cargando mas funciones parecidas
	void eliminarPaginaSegmento(Pagina* pagina) {
		eliminarPaginaDeMemoria(pagina, segmentoAEliminar);
	}
	list_iterate(segmentoAEliminar->paginas, (void*) eliminarPaginaSegmento);
	freeSegmento(segmentoAEliminar);
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
	//algun semaforo
	log_info(logger, "Realizando Journal");

	void enviarSiEstaModificada(Pagina* pagina, Segmento* segmento) {
		if (isModificada(pagina)) {
			enviarRegistroAFileSystem(pagina, segmento->nombreTabla);
		}
	}

	void journalPaginasModificadasBySegmento(Segmento* segmento) {
		if (existeSegmentoFS(segmento)) {
			list_iterate2(segmento->paginas, (void*) enviarSiEstaModificada, segmento);
		} else {
			log_info (logger, "La informacion del segmento  %s no se cargo en FS ya que el mismo no existia", segmento->nombreTabla);
		}

	}

	list_iterate(segmentos, (void*) journalPaginasModificadasBySegmento);

	list_iterate(segmentos, (void*) eliminarSegmentoDeMemoria);

}

bool existeSegmentoFS(Segmento* segmento) {
	if (segmento->metaData != NULL) {
		return true;
	} else if (buscarSegmentoEnFileSystem(segmento->nombreTabla) != NULL) {
		return true;
	}
	return false;
}

