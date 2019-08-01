#include "SocketServidor.h"

#include <commons/log.h>
#include <commons/string.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdbool.h>

#include "AdministradorConsultasLFS.h"

void escuchar(int listenningSocket) {
	listen(listenningSocket, SOMAXCONN); // es una syscall bloqueante

	while (true) {

		printf("Escuchando.. \n");
		struct sockaddr_in datosConexionCliente; // Esta estructura contendra los datos de la conexion del cliente. IP, puerto, etc.
		socklen_t datosConexionClienteSize = sizeof(datosConexionCliente);
		int socketMemoria = accept(listenningSocket, (struct sockaddr *) &datosConexionCliente, &datosConexionClienteSize);
		if (socketMemoria != -1) {

			pthread_t hiloRequest;
			pthread_create(&hiloRequest, NULL, (void*) procesarAccion, (void*) socketMemoria);
			pthread_detach(hiloRequest);

		} else {
			log_info(loggerInfo, "Fallo la conexion con Memoria");

		}

	}
}

void procesarAccion(int socketMemoria) {
	Paquete paquete;

	if (RecibirPaquete(socketMemoria, &paquete) > 0) {
		log_info(loggerInfo, "Request de memoria recibida: %s", paquete.mensaje);
		usleep(config->RETARDO * 1000);
		if (paquete.header.quienEnvia == MEMORIA) {
			usleep(config->RETARDO * 1000);
			switch ((int) paquete.header.tipoMensaje) {
			log_info(loggerInfo, "Request en fileSystem %s", (char*) paquete.mensaje);
		case (CONEXION_INICIAL_FILESYSTEM_MEMORIA):
			configuracionNuevaMemoria(socketMemoria, config->TAMANIO_VALUE);
			break;
		case (SELECT):
			procesarSELECT(paquete.mensaje, socketMemoria);
			break;
		case (INSERT):
			procesarINSERT(paquete.mensaje, socketMemoria);
			break;
		case (DROP):
			procesarDROP(paquete.mensaje, socketMemoria);
			break;

		case (CREATE):
			procesarCREATE(paquete.mensaje, socketMemoria);
			break;
		case (DESCRIBE):
			procesarDESCRIBE(paquete.mensaje, socketMemoria);
			break;
		case DESCRIBE_ALL:
			procesarDESCRIBE_ALL(socketMemoria);
			break;
			}

		} else {
			log_info(loggerInfo, "No es ningun proceso de Memoria");
		}
		if (paquete.mensaje != NULL) {
			free(paquete.mensaje);
		}
	}
	close(socketMemoria);

}

void configuracionNuevaMemoria(int socketMemoria, int valueMaximo) {
	log_info(loggerInfo, "Nueva memoria conectada. Socket N:%d", socketMemoria);
	char valueMaximoChar[10];
	sprintf(valueMaximoChar, "%d", valueMaximo);
	EnviarDatosTipo(socketMemoria, FILESYSTEM, valueMaximoChar, strlen(valueMaximoChar) + 1, CONEXION_INICIAL_FILESYSTEM_MEMORIA);

}

void procesarINSERT(char* request, int socketMemoria) {
	log_info(loggerInfo, "Procesando INSERT:  %s", request);
	char* requestAux = string_duplicate(request);
	char** valores = string_split(requestAux, "\""); //34 son las " en ASCII
	char** valoresAux = string_split(valores[0], " ");
	char* nombreTabla = valoresAux[0];
	char* key = valoresAux[1];
	char* value = valores[1];
	double timeStamp;
	if (valores[2] == NULL) {
		timeStamp = getCurrentTime();
	} else {
		timeStamp = atof(valores[2]);
	}

	int resultado = funcionINSERT(timeStamp, nombreTabla, key, value);

	enviarSuccess(resultado, INSERT, socketMemoria);
	freePunteroAPunteros(valoresAux);
	freePunteroAPunteros(valores);
	free(requestAux);

}

void procesarCREATE(char* request, int socketMemoria) {
	log_info(loggerInfo, "Procesando CREATE:  %s", request);
	char* requestAux = string_duplicate(request);
	char** valores = string_split(requestAux, " ");
	char* nombreTabla = string_duplicate(valores[0]);
	char* consistenciaChar = valores[1];
	char* cantParticiones = valores[2];
	char* tiempoCompactacion = valores[3];

	int resultado = funcionCREATE(nombreTabla, cantParticiones, consistenciaChar, tiempoCompactacion);

	enviarSuccess(resultado, CREATE, socketMemoria);
	freePunteroAPunteros(valores);
	free(requestAux);
	free(nombreTabla);
}

void procesarDROP(char* nombreTabla, int socketMemoria) {
	log_info(loggerInfo, "Procesando DROP:  %s", nombreTabla);
	int resultado = funcionDROP(nombreTabla);
	enviarSuccess(resultado, DROP, socketMemoria);
}

//esta funcion podria ser mucho mas linda, pero el obtenerNombreTablas no me sale
void procesarDESCRIBE_ALL(int socketMemoria) {
	pthread_mutex_lock(&mutexDrop);
	log_info(loggerInfo, "Procesando DESCRIBE ALL");
	t_list* listaDirectorios = list_create();
	buscarDirectorios(rutas.Tablas, listaDirectorios);

	char* tablasSerializadas = string_new();

	void serializarTablas(char* directorio) {
		char* nombreTabla = obtenerNombreTablaByRuta(directorio);
		t_metadata_tabla metadata = obtenerMetadata(nombreTabla);
		string_append_with_format(&tablasSerializadas, "%s %s %d %d /", nombreTabla, getConsistenciaCharByEnum(metadata.CONSISTENCIA),
				metadata.CANT_PARTICIONES, metadata.T_COMPACTACION);
		free(nombreTabla);
	}

	if (list_is_empty(listaDirectorios)) {
		enviarSuccess(1, DESCRIBE_ALL, socketMemoria);
	} else {
		list_iterate(listaDirectorios, (void*) serializarTablas);
		EnviarDatosTipo(socketMemoria, FILESYSTEM, tablasSerializadas, strlen(tablasSerializadas) + 1, DESCRIBE_ALL);
	}

	list_destroy_and_destroy_elements(listaDirectorios, free);
	free(tablasSerializadas);
	pthread_mutex_unlock(&mutexDrop);
}

void procesarDESCRIBE(char* nombreTabla, int socketMemoria) {
	log_info(loggerInfo, "Procesando DESCRIBE:  %s", nombreTabla);
	if (existeTabla(nombreTabla)) {
		t_metadata_tabla metadata = funcionDESCRIBE(nombreTabla);
		char* response = string_new();
		string_append_with_format(&response, "%s %s %d %d", nombreTabla, getConsistenciaCharByEnum(metadata.CONSISTENCIA),
				metadata.CANT_PARTICIONES, metadata.T_COMPACTACION);
		EnviarDatosTipo(socketMemoria, FILESYSTEM, response, strlen(response) + 1, DESCRIBE);
	} else {
		enviarSuccess(-1, DESCRIBE, socketMemoria);
	}

}

void procesarSELECT(char* request, int socketMemoria) {
	log_info(loggerInfo, "Procesando SELECT:  %s", request);
	char** valores = string_split(request, " ");
	char* nombreTabla = valores[0];
	int key = atoi(valores[1]);
	log_info(loggerInfo, "SELECT recibido Tabla: %s Key: %d", nombreTabla, key);
	t_registro* registro = funcionSELECT(nombreTabla, key);
	if (registro == NULL) {
		EnviarDatosTipo(socketMemoria, FILESYSTEM, NULL, 0, NOTFOUND);
	} else {
		char* response = string_new();
		string_append_with_format(&response, "%d;%s;%f", registro->key, registro->value, registro->timestamp);
		EnviarDatosTipo(socketMemoria, FILESYSTEM, response, strlen(response) + 1, SELECT);
		free(response);
		freeRegistro(registro);
	}

	freePunteroAPunteros(valores);
}

void enviarSuccess(int resultado, t_protocolo protocolo, int socketMemoria) {
	char success[2];
	sprintf(success, "%d", resultado);
	EnviarDatosTipo(socketMemoria, FILESYSTEM, success, 2, protocolo);
}

