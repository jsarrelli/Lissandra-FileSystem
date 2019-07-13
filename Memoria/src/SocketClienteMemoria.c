#include "SocketClienteMemoria.h"

t_metadata_tabla* deserealizarTabla(Paquete* paquete) {
	char* mensaje = malloc(paquete->header.tamanioMensaje + 1);
	mensaje = strcpy(mensaje, (char*) paquete->mensaje);
	char** datos = string_split(mensaje, " ");

	t_consistencia consistencia = getConsistenciaByChar(datos[1]);
	int cantParticiones = atoi(datos[2]);
	int tiempoCompactacion = atoi(datos[3]);
	//creo la metadata
	t_metadata_tabla* metadata = malloc(sizeof(t_metadata_tabla));
	metadata->CONSISTENCIA = consistencia;
	metadata->CANT_PARTICIONES = cantParticiones;
	metadata->T_COMPACTACION = tiempoCompactacion;

	freePunteroAPunteros(datos);
	free(mensaje);
	return metadata;
}

t_metadata_tabla* describeSegmento(char* nombreSegmento) {
	int socketFileSystem = ConectarAServidor(configuracion->PUERTO_FS, configuracion->IP_FS);

	EnviarDatosTipo(socketFileSystem, MEMORIA, (void*) nombreSegmento, strlen(nombreSegmento) + 1, DESCRIBE);
	Paquete paquete;
	RecibirPaqueteCliente(socketFileSystem, FILESYSTEM, &paquete);

	if (atoi(paquete.mensaje) == 1) {
		free(paquete.mensaje);
		return NULL;
		//la tabla no existe
	}
	t_metadata_tabla* metaDataRecibida = deserealizarTabla(&paquete);
	free(paquete.mensaje);

	return metaDataRecibida;
}

void enviarRegistroAFileSystem(Pagina* pagina, char* nombreSegmento) {
	int socketFileSystem = ConectarAServidor(configuracion->PUERTO_FS, configuracion->IP_FS);
	t_registro* registro = pagina->registro;
	char consulta[150];
	sprintf(consulta, "%s %d \"%s\" %f", nombreSegmento, registro->key, registro->value, registro->timestamp);
	EnviarDatosTipo(socketFileSystem, MEMORIA, consulta, strlen(consulta) + 1, INSERT);
}

int eliminarSegmentoFileSystem(char* nombreSegmento) {
	int socketFileSystem = ConectarAServidor(configuracion->PUERTO_FS, configuracion->IP_FS);
	EnviarDatosTipo(socketFileSystem, MEMORIA, nombreSegmento, strlen(nombreSegmento) + 1, DROP);
	Paquete paquete;
	int succes = 0;
	if (RecibirPaqueteCliente(socketFileSystem, FILESYSTEM, &paquete) > 0) {
		succes = atoi(paquete.mensaje);
		free(paquete.mensaje);
	}

	return succes;
}

int enviarCreateAFileSystem(t_metadata_tabla* metadata, char* nombreTabla) {
	int socketFileSystem = ConectarAServidor(configuracion->PUERTO_FS, configuracion->IP_FS);
	char* consulta = string_new();
	string_append_with_format(consulta, "%s %s %d %d", nombreTabla, getConsistenciaCharByEnum(metadata->CONSISTENCIA),
			metadata->CANT_PARTICIONES, metadata->T_COMPACTACION);
	log_info(logger, "Enviando CREATE a fileSystem", consulta);
	EnviarDatosTipo(socketFileSystem, MEMORIA, consulta, strlen(consulta) + 1, CREATE);
	free(consulta);

	Paquete paquete;
	int succes = 0;
	if (RecibirPaqueteCliente(socketFileSystem, FILESYSTEM, &paquete) > 0) {
		succes = atoi(paquete.mensaje);
		free(paquete.mensaje);
	}

	return succes;
}

t_list* describeAllFileSystem() {
	int socketFileSystem = ConectarAServidor(configuracion->PUERTO_FS, configuracion->IP_FS);
	EnviarDatosTipo(socketFileSystem, MEMORIA, NULL, 0, DESCRIBE_ALL);

	t_list* segmentosRecibidos = list_create();
	Paquete paquete;

	while (RecibirPaqueteCliente(socketFileSystem, FILESYSTEM, &paquete) > 0) {
		if (strcmp(paquete.mensaje, "fin") == 0) {
			free(paquete.mensaje);
			break;
		}

		char* tablaSerializada = malloc(paquete.header.tamanioMensaje);
		strcpy(tablaSerializada, paquete.mensaje);
		list_add(segmentosRecibidos, tablaSerializada);
		free(paquete.mensaje);
	}

	return segmentosRecibidos;
}

int HandshakeInicial() {
	log_info(logger, "Intentandose conectar a File System..");
	int socketFileSystem = ConectarAServidor(configuracion->PUERTO_FS, configuracion->IP_FS);
	if (socketFileSystem == -1) {
		log_info(logger, "Fallo la conexion a File System");
		return socketFileSystem;
	}
	log_info(logger, "Memoria conectada a File System");
	EnviarDatosTipo(socketFileSystem, MEMORIA, NULL, 0, CONEXION_INICIAL_FILESYSTEM_MEMORIA);
	Paquete paquete;
	if (RecibirPaqueteCliente(socketFileSystem, MEMORIA, &paquete) > 0) {
		valueMaximoPaginas = atoi(paquete.mensaje);
		free(paquete.mensaje);
	}

	log_info(logger, "Handshake inicial realizado. Value Maximo: %d", valueMaximoPaginas);
	return socketFileSystem;
}

void gossiping() {
	list_iterate(seeds, (void*) intercambiarTablasGossiping);

	log_info(logger, "Las memorias conocidas son");
	void mostrarTablaConocida(t_memoria* memoria) {
		log_info(logger, "Puerto:%s IP:%s MEMORY NUMBER:%d", memoria->ip, memoria->puerto, memoria->memoryNumber);
	}
	list_iterate(tablaGossiping, (void*) mostrarTablaConocida);
}

void enviarTablaGossiping(int socketMemoriaDestino) {

	void enviarMemoriaConocida(t_memoria* memoriaConocida) {
		char request[100];
		sprintf(request, "%s %s %d", memoriaConocida->ip, memoriaConocida->puerto, memoriaConocida->memoryNumber);
		EnviarDatosTipo(socketMemoriaDestino, MEMORIA, request, strlen(request) + 1, GOSSIPING);
	}
	list_iterate(tablaGossiping, (void*) enviarMemoriaConocida);
	EnviarDatosTipo(socketMemoriaDestino, MEMORIA, "fin", 4, GOSSIPING);
}

void intercambiarTablasGossiping(t_memoria* memoria) {
	int socketMemoria = ConectarAServidor(atoi(memoria->puerto), memoria->ip);

	if (socketMemoria == -1) {
		return;
	}
	enviarTablaGossiping(socketMemoria);

	Paquete paquete;
	while (true) {
		RecibirPaqueteCliente(socketMemoria, MEMORIA, &paquete);
		if (strcmp(paquete.mensaje, "fin") == 0) {
			free(paquete.mensaje);
			break;
		}
		t_memoria* memoriaRecibida = deserealizarMemoria(paquete.mensaje);
		agregarMemoriaNueva(memoriaRecibida);
		free(paquete.mensaje);
	}

}

t_registro* selectFileSystem(Segmento* segmento, int key) {
	log_info(logger, "Enviando consulta SELECT al fileSystem..");
	int socketFileSystem = ConectarAServidor(configuracion->PUERTO_FS, configuracion->IP_FS);
	while (socketFileSystem != -1) {
		socketFileSystem = ConectarAServidor(configuracion->PUERTO_FS, configuracion->IP_FS);
		usleep(100);
	}
	char* consulta = string_new();
	string_append_with_format(&consulta, "%s %d", segmento->nombreTabla, key);

	EnviarDatosTipo(socketFileSystem, MEMORIA, consulta, strlen(consulta) + 1, SELECT);
	free(consulta);

	Paquete paquete;
	RecibirPaqueteCliente(socketFileSystem, FILESYSTEM, &paquete);

	if (paquete.header.tipoMensaje == NOTFOUND) {
		return NULL;
	}
	char*registroAux = string_duplicate(paquete.mensaje);
	char** valores = string_split(paquete.mensaje, ";");

	t_registro* registro = malloc(sizeof(t_registro));
	registro->key = atoi(valores[0]);
	registro->value = string_duplicate(valores[1]);
	registro->timestamp = atof(valores[2]);

	freePunteroAPunteros(valores);
	free(registroAux);
	free(paquete.mensaje);
	log_info(logger, "Registro obtenido de fileSystem");
	return registro;
}
