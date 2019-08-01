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

	log_info(loggerInfo, "Intentando conectarse a FileSystem..");
	int socketFileSystem = ConectarAServidorPlus(configuracion->PUERTO_FS, configuracion->IP_FS);

	log_info(loggerInfo, "Conexion exitosa, enviando datos..");
	EnviarDatosTipo(socketFileSystem, MEMORIA, (void*) nombreSegmento, strlen(nombreSegmento) + 1, DESCRIBE);
	Paquete paquete;
	if (RecibirPaquete(socketFileSystem, &paquete) < 0) {
		log_error(loggerError, "Algo fallo en el describe de %s en fileSystem", nombreSegmento);
		close(socketFileSystem);
		return NULL;
	}

	t_metadata_tabla* metaDataRecibida = NULL;
	if (atoi(paquete.mensaje) != -1) {
		metaDataRecibida = deserealizarTabla(&paquete);
	}

	free(paquete.mensaje);
	close(socketFileSystem);
	return metaDataRecibida;
}

void enviarRegistroAFileSystem(Pagina* pagina, char* nombreSegmento) {
	log_info(loggerInfo, "Intentando conectarse a FileSystem..");
	int socketFileSystem = ConectarAServidorPlus(configuracion->PUERTO_FS, configuracion->IP_FS);

	t_registro_memoria* registro = pagina->registro;
	char * consulta = string_new();
	string_append_with_format(&consulta, "%s %d \"%s\" %f", nombreSegmento, registro->key, registro->value, registro->timestamp);
	log_info(loggerInfo, "Conexion exitosa, enviando datos..");
	EnviarDatosTipo(socketFileSystem, MEMORIA, consulta, strlen(consulta) + 1, INSERT);
	free(consulta);
	close(socketFileSystem);
}

int eliminarSegmentoFileSystem(char* nombreSegmento) {
	log_info(loggerInfo, "Intentando conectarse a FileSystem..");
	int socketFileSystem = ConectarAServidorPlus(configuracion->PUERTO_FS, configuracion->IP_FS);

	log_info(loggerInfo, "Conexion exitosa, enviando datos..");
	EnviarDatosTipo(socketFileSystem, MEMORIA, nombreSegmento, strlen(nombreSegmento) + 1, DROP);
	Paquete paquete;
	int succes = 1;
	if (RecibirPaquete(socketFileSystem, &paquete) > 0) {
		succes = atoi(paquete.mensaje);
		free(paquete.mensaje);
	}
	close(socketFileSystem);
	return succes;
}

int enviarCreateAFileSystem(t_metadata_tabla* metadata, char* nombreTabla) {
	log_info(loggerInfo, "Intentando conectarse a FileSystem..");
	int socketFileSystem = ConectarAServidorPlus(configuracion->PUERTO_FS, configuracion->IP_FS);

	char* consulta = string_new();
	string_append_with_format(&consulta, "%s %s %d %d", nombreTabla, getConsistenciaCharByEnum(metadata->CONSISTENCIA),
			metadata->CANT_PARTICIONES, metadata->T_COMPACTACION);
	log_info(loggerInfo, "Conexion exitosa, enviando datos..");
	EnviarDatosTipo(socketFileSystem, MEMORIA, consulta, strlen(consulta) + 1, CREATE);
	free(consulta);

	Paquete paquete;
	int succes = 1;
	if (RecibirPaquete(socketFileSystem, &paquete) > 0) {
		succes = atoi(paquete.mensaje);
		free(paquete.mensaje);
	}
	close(socketFileSystem);
	return succes;
}

char* describeAllFileSystem() {
	log_info(loggerInfo, "Intentando conectarse a FileSystem..");
	int socketFileSystem = ConectarAServidorPlus(configuracion->PUERTO_FS, configuracion->IP_FS);

	log_info(loggerInfo, "Conexion exitosa, enviando datos..");
	EnviarDatosTipo(socketFileSystem, MEMORIA, NULL, 0, DESCRIBE_ALL);

	Paquete paquete;
	char* response = NULL;
	if (RecibirPaquete(socketFileSystem, &paquete) > 0) {
		if (atoi(paquete.mensaje) != 1) {
			response = string_duplicate(paquete.mensaje);
		}
		free(paquete.mensaje);
	}
	close(socketFileSystem);
	return response;
}

int HandshakeInicial() {
	log_info(loggerInfo, "Intentandose conectar a File System..");
	int socketFileSystem = ConectarAServidor(configuracion->PUERTO_FS, configuracion->IP_FS);
	if (socketFileSystem == -1) {
		log_info(loggerInfo, "Fallo la conexion a File System");

		return -1;
	}
	log_info(loggerInfo, "Memoria conectada a File System");
	EnviarDatosTipo(socketFileSystem, MEMORIA, NULL, 0, CONEXION_INICIAL_FILESYSTEM_MEMORIA);
	Paquete paquete;
	if (RecibirPaquete(socketFileSystem, &paquete) > 0) {
		valueMaximo = atoi(paquete.mensaje);
		free(paquete.mensaje);
	} else {
		log_error(loggerError, "Algo fallo en la conexion en la Conexion con fileSystem");
	}

	log_info(loggerInfo, "Handshake inicial realizado. Value Maximo: %d", valueMaximo);
	close(socketFileSystem);
	return 1;
}

void enviarTablaGossiping(int socketMemoriaDestino) {
	char* memoriasSerializadas = string_new();

	void serializarMemoria(t_memoria* memoriaConocida) {
		string_append_with_format(&memoriasSerializadas, "%s %s %d /", memoriaConocida->ip, memoriaConocida->puerto,
				memoriaConocida->memoryNumber);
	}
	list_iterate(tablaGossiping, (void*) serializarMemoria);
	EnviarDatosTipo(socketMemoriaDestino, MEMORIA, memoriasSerializadas, strlen(memoriasSerializadas) + 1, GOSSIPING);
	free(memoriasSerializadas);

}

void intercambiarTablasGossiping(t_memoria* memoriaSeed) {
	int socketMemoriaSeed = ConectarAServidor(atoi(memoriaSeed->puerto), memoriaSeed->ip);
	if (socketMemoriaSeed == -1) {
		return;
	}

	enviarTablaGossiping(socketMemoriaSeed);

	Paquete paquete;

	if (RecibirPaquete(socketMemoriaSeed, &paquete) > 0) {

		int i = 0;
		char** memorias = string_split(paquete.mensaje, "/");
		while (memorias[i] != NULL) {
			t_memoria* memoriaRecibida = deserealizarMemoria(memorias[i]);

			agregarMemoriaNueva(memoriaRecibida);
			i++;
		}
		freePunteroAPunteros(memorias);
		free(paquete.mensaje);

	}
	close(socketMemoriaSeed);

}
t_registro* selectFileSystem(Segmento* segmento, int key) {
	log_info(loggerInfo, "Enviando consulta SELECT al fileSystem..");
	int socketFileSystem = ConectarAServidorPlus(configuracion->PUERTO_FS, configuracion->IP_FS);

	char* consulta = string_new();
	string_append_with_format(&consulta, "%s %d", segmento->nombreTabla, key);
	log_info(loggerInfo, "Conexion exitosa, enviando datos..");

	EnviarDatosTipo(socketFileSystem, MEMORIA, consulta, strlen(consulta) + 1, SELECT);
	free(consulta);

	Paquete paquete;
	if (RecibirPaquete(socketFileSystem, &paquete) <= 0) {
		close(socketFileSystem);
		return NULL;
	}

	if (paquete.header.tipoMensaje == NOTFOUND) {
		close(socketFileSystem);
		free(paquete.mensaje);
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
	log_info(loggerInfo, "Registro obtenido de fileSystem");
	close(socketFileSystem);
	return registro;
}

bool isMemoriaCaida(t_memoria* memoria) {
	if (memoria->memoryNumber == configuracion->MEMORY_NUMBER) {
		return false; //es esta misma
	}
	int socket;
	int i = 0;
	do {
		socket = ConectarAServidor(atoi(memoria->puerto), memoria->ip);
		if (socket != -1) {
			break;
		}
		usleep(10);
		i++;
	} while (i < 20);

	bool caida;
	if (socket == -1) {
		log_error(loggerError, "Se cayo la memoria %s %s %d", memoria->ip, memoria->puerto, memoria->memoryNumber);
		caida = true;
	} else {
		caida = false;
		close(socket);
	}

	return caida;
}
