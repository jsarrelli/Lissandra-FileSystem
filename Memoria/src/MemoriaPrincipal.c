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

void inicializarMemoria(int valueMaximoRecibido, int tamanioMemoriaRecibido, int socketFileSystemRecibido) {
	tamanioMemoria = tamanioMemoriaRecibido;
	cantFrames = tamanioMemoria / sizeof(t_registro);
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
	segmento->nombreModificado = 0;
	segmento->metaData = metaData;
	list_add(segmentos, segmento);
	return segmento;

}

//inserta una pagina en la memoria y te devuelve la direccion de
//donde la puso
//el segmento tiene que existir
Pagina* insertarPaginaEnMemoria(int key, char value[112], double timeStamp, Segmento* segmento) {
	Pagina* paginaNueva = buscarPaginaEnMemoria(segmento, key);
	if (paginaNueva == NULL) {

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
		log_info(logger, "Esta key ya se encuentra en el sistema, se actualizara");
		paginaNueva->registro->timestamp = timeStamp;
		strcpy(paginaNueva->registro->value, value);
		paginaNueva->modificado = 1;
	}

	EstadoFrame* estadoFrame = getEstadoFrame(paginaNueva);
	estadoFrame->estado = 1;
	estadoFrame->fechaObtencion = getCurrentTime();

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

		//queria INSERT TABLA KEY, le sumo dos por los espacios
		char* consulta = malloc(strlen(strlen("INSERT") + segmento->nombreTabla) + 4 + 2);
		sprintf(consulta, "INSERT &s &d", segmento->nombreTabla, key);

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
 y si el fileSystem no lo tiene, lo creamos
 */
Segmento* buscarSegmento(char* nombreSegmento) {
	Segmento* segmento = buscarSegmentoEnMemoria(nombreSegmento);

	if (segmento == NULL) {

		EnviarDatosTipo(socketFileSystem, MEMORIA, (void*) nombreSegmento, strlen(nombreSegmento), SELECT_TABLE);
		Paquete paquete;
		RecibirPaqueteCliente(socketFileSystem, FILESYSTEM, &paquete);

		if (paquete.header.tipoMensaje == NOTFOUND) {
			return NULL;
		}
		//esto deberia sacarlo en una funcion, pero es muy compeljo y no justifica
		void*cadenaRecibida = malloc(paquete.header.tamanioMensaje);
		char** datos = string_split(cadenaRecibida, " ");
		char* nombreSegmento = datos[0];
		char* consistenciaChar = datos[1];
		int cantParticiones = atoi(datos[2]);
		int tiempoCompactacion = atoi(datos[3]);

		t_consistencia consistencia;
		if (strcmp(consistenciaChar, "EC") == 0) {
			consistencia = EVENTUAL;
		}
		t_metadata_tabla* metadata = malloc(sizeof(t_metadata_tabla));
		metadata->CONSISTENCIA = consistencia;
		metadata->CANT_PARTICIONES = cantParticiones;
		metadata->T_COMPACTACION = tiempoCompactacion;

		segmento = insertarSegmentoEnMemoria(nombreSegmento, metadata);
		list_add(segmentos, segmento);

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
	log_info(logger, "Aplicando algortimo LRU");
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
	log_info(logger, "Se eliminara la pagina con key: %d y timeStamp:%f por ser la menos accedida",
			paginaMenosUtilizada->registro->key, paginaMenosUtilizada->registro->timestamp);
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

void eliminarSegmentoDeMemoria(char* nombreSegmento) {
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

