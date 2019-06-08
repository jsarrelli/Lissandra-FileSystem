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
	tamanioRegistro= sizeof(int)+sizeof(double)+valueMaximoRecibido;
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
	segmento->nombreModificado = 0;
	segmento->metaData = metaData;
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

		t_registro* registroEnMemoria = memcpy(marcoVacio, &registro,
				tamanioRegistro);

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
		if (((EstadoFrame*) list_get(memoriaStatus, i))->estado
				== LIBRE) {
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

	}else{
		EstadoFrame* estadoFrame=getEstadoFrame(pagina);
		estadoFrame->fechaObtencion=getCurrentTime();
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
		t_consistencia consistencia=getConsistenciaByChar(datos[1]);
		int cantParticiones = atoi(datos[2]);
		int tiempoCompactacion = atoi(datos[3]);

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
	t_list* paginasModificadas= obtenerPaginasModificadas();
	return list_size(paginasModificadas)==list_size(memoriaStatus);
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
				frameDePagina->estado == LIBRE) {
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
	estadoFrame->estado = LIBRE;

	bool isPagina(Pagina* pagina) {
		return pagina->registro->key == paginaAEliminar->registro->key;
	}

	void freePagina(Pagina* pagina) {
		free(pagina);
	}

	list_remove_and_destroy_by_condition(segmento->paginas, (void*) isPagina, (void*) freePagina);

}

void eliminarSegmentoDeMemoria(Segmento* segmentoAEliminar) {

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
t_list* obtenerPaginasModificadas() {
	t_list* paginasModificadas = list_create();
	bool isModificada(Pagina* pagina) {
		EstadoFrame* estadoFrame = getEstadoFrame(pagina);
		return estadoFrame->estado == (int) MODIFICADO;
	}
	void buscarPaginasModificadas(Segmento* segmento) {
		t_list* paginasModificadasSegmento = list_filter(segmento->paginas, (void*) isModificada);
		list_add_all(paginasModificadas, paginasModificadasSegmento);

	}
	list_iterate(segmentos, (void*) buscarPaginasModificadas);
	return paginasModificadas;
}

void enviarRegistro(Pagina* pagina) {
	t_registro* registro = pagina->registro;
	char* consulta = malloc(strlen("INSERT") + sizeof(registro->key) + strlen(registro->value) + sizeof(registro->timestamp) + 3);//mas 3 espacios
	sprintf(consulta, "INSERT %d %s %f", registro->key, registro->value, registro->timestamp);
	EnviarDatosTipo(socketFileSystem, MEMORIA, consulta, strlen(consulta), INSERT);
}

void journalMemoria() {
	//algun semaforo
	log_info(logger, "Realizando Journal");
	t_list* paginasModificadas = obtenerPaginasModificadas();

	list_iterate(paginasModificadas, (void*) enviarRegistro);

	list_iterate(segmentos,(void*) eliminarSegmentoDeMemoria);

}

void eliminarSegmentoFileSystem(char* nombreSegmento){
	char* consulta=malloc(strlen("DROP")+strlen(nombreSegmento)+1);// +1 por el espacio
	sprintf(consulta,"DROP %s",nombreSegmento);
	EnviarDatosTipo(socketFileSystem, MEMORIA, consulta, strlen(consulta), DROP);
}

