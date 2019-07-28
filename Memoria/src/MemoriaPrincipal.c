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
	cantFrames = (int) round(tamanioMemoria / tamanioRegistro);
	memoria = malloc(tamanioMemoria);

	segmentos = list_create();
	inicializarEstadoMemoria();
	//recemosle a Dios y a la virgen
	pthread_mutex_init(&mutexJournal, NULL);
	pthread_mutex_init(&mutexAsignacionMarco, NULL);
	pthread_mutex_init(&mutexSelect, NULL);
	pthread_mutex_init(&mutexInsert, NULL);
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

void* asignarMarco() {

	pthread_mutex_lock(&mutexAsignacionMarco);

	log_info(loggerInfo, "Asignando marco libre..");
	if (memoriaLlena()) {
		log_info(loggerInfo, "Memoria llena");
		if (!todosModificados()) {
			liberarUltimoUsado();
		} else {
			journalMemoria();
		}
	}

	void* marcoVacio = darMarcoVacio();
	log_info(loggerInfo, "Marco asignado ");
	pthread_mutex_unlock(&mutexAsignacionMarco);

	return marcoVacio;
}

//inserta una pagina en la memoria y te devuelve la direccion de
//donde la puso
Pagina* insertarPaginaEnMemoria(int key, char* value, double timeStamp, Segmento* segmento, bool modificado) {

	log_info(loggerInfo, "Insertando pagina en memoria..");
	Pagina* paginaNueva = buscarPaginaEnMemoria(segmento, key);
	if (paginaNueva == NULL) {
		log_info(loggerInfo, "Insertando registro %d \"%s\" %f en memoria..", key, value, timeStamp);

		void* marcoVacio = asignarMarco();

		t_registro_memoria* registro = malloc(tamanioRegistro);
		registro->key = key;
		strcpy(registro->value, value);
		registro->timestamp = timeStamp;

		t_registro_memoria* registroEnMemoria = memcpy(marcoVacio, registro, tamanioRegistro);

		paginaNueva = malloc(sizeof(Pagina));
		paginaNueva->modificado = modificado;
		paginaNueva->registro = registroEnMemoria;

		free(registro);

		list_add(segmento->paginas, paginaNueva);
	} else {
		log_info(loggerInfo, "Esta key ya se encuentra en el sistema, se actualizara");
		strcpy(paginaNueva->registro->value, value);
		paginaNueva->registro->timestamp = timeStamp;
	}

	if (modificado) {
		paginaNueva->modificado = MODIFICADO;
	}

	EstadoFrame* estadoFrame = getEstadoFrame(paginaNueva);
	estadoFrame->estado = OCUPADO;
	estadoFrame->fechaObtencion = getCurrentTime();
	log_info(loggerInfo, "Registro %d \"%s\" %f insertado en memoria", key, value, timeStamp);

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

void liberarUltimoUsado() {

	log_info(loggerInfo, "Aplicando algortimo LRU");
	EstadoFrame* estadoFrameMenosUtilizado = NULL;
	Pagina* paginaMenosUtilizada = NULL;
	Segmento* segmentoPaginaMenosUtilizada = NULL;

	void buscarPaginaLRU(Pagina* pagina, Segmento* segmento) {
		if (pagina->modificado) {
			return;
		}
		EstadoFrame* estadoFrameActual = getEstadoFrame(pagina);
		if (paginaMenosUtilizada == NULL || estadoFrameActual->fechaObtencion < estadoFrameMenosUtilizado->fechaObtencion) {

			paginaMenosUtilizada = pagina;
			estadoFrameMenosUtilizado = estadoFrameActual;
			segmentoPaginaMenosUtilizada = segmento;

		}
	}

	void recorrerSegmentos(Segmento* segmento) {
		list_iterate2(segmento->paginas, (void*) buscarPaginaLRU, (void*) segmento);
	}

	list_iterate(segmentos, (void*) recorrerSegmentos);

	log_trace(loggerTrace, "Se eliminara la pagina con key: %d y timeStamp:%f por ser la menos accedida",
			paginaMenosUtilizada->registro->key, paginaMenosUtilizada->registro->timestamp);

	reemplazarPagina(paginaMenosUtilizada, segmentoPaginaMenosUtilizada);

}

void reemplazarPagina(Pagina* pagina, Segmento* segmento) {
	pthread_mutex_lock(&mutexJournal);
	log_info(loggerInfo, "Pagina de registro %d, %s de tabla %s reemplazada", pagina->registro->key, pagina->registro->value,
			segmento->nombreTabla);
	bool isPaginaEliminar(Pagina* paginaAEliminar) {
		return pagina->registro->key == paginaAEliminar->registro->key;
	}
	list_remove_and_destroy_by_condition(segmento->paginas, (void*) isPaginaEliminar, (void*) eliminarPaginaDeMemoria);
	pthread_mutex_unlock(&mutexJournal);

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

void journalMemoria() {

	log_info(loggerInfo, "Realizando Journal..");
	t_list* tablas = obtenerTablasFileSystem();

	void enviarSiEstaModificada(Pagina* pagina, Segmento* segmento) {

		if (isModificada(pagina)) {
			enviarRegistroAFileSystem(pagina, segmento->nombreTabla);
			log_info(loggerInfo, "Enviando registro a FileSystem..");
		}

	}

	void journalPaginasModificadasBySegmento(Segmento* segmento) {
		log_info(loggerInfo, "Realizando Journal de %s", segmento->nombreTabla);
		if (existeSegmentoFS(segmento->nombreTabla, tablas)) {
			list_iterate2(segmento->paginas, (void*) enviarSiEstaModificada, segmento);
		} else {
			log_info(loggerInfo, "La informacion del segmento  %s no se cargo en FS ya que el mismo no existia", segmento->nombreTabla);
		}

		list_clean_and_destroy_elements(segmento->paginas, (void*) eliminarPaginaDeMemoria);
	}

	list_iterate(segmentos, (void*) journalPaginasModificadasBySegmento);
	list_destroy_and_destroy_elements(tablas, free);
	log_info(loggerInfo, "Journal finalizado con exito");

}

t_list* obtenerTablasFileSystem() {
	char* tablasSerializadas = describeAllFileSystem();
	t_list* listaTablas = list_create();

	if (tablasSerializadas == NULL) {
		return listaTablas;
	}
	char** tablasAux = string_split(tablasSerializadas, "/");
	int i = 0;

	while (tablasAux[i] != NULL) {

		char* tablaAux = string_duplicate(tablasAux[i]);
		char** valores = string_split(tablaAux, " ");
		char* nombreTabla = string_duplicate(valores[0]);

		list_add(listaTablas, nombreTabla);

		freePunteroAPunteros(valores);
		free(tablaAux);

		i++;
	}
	freePunteroAPunteros(tablasAux);
	free(tablasSerializadas);

	return listaTablas;
}

bool existeSegmentoFS(char* nombreSegmento, t_list* tablas) {

	bool mismaTabla(char* nombreTablaActual) {
		return strcmp(nombreTablaActual, nombreSegmento) == 0;
	}
	return list_any_satisfy(tablas, (void*) mismaTabla);
}

