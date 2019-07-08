#include "SocketServidor.h"

#include <commons/log.h>
#include <commons/string.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdbool.h>

#include "AdministradorConsultasLFS.h"

void escuchar(int listenningSocket) {
	listen(listenningSocket, BACKLOG); // es una syscall bloqueante

	struct sockaddr_in datosConexionCliente; // Esta estructura contendra los datos de la conexion del cliente. IP, puerto, etc.
	socklen_t datosConexionClienteSize = sizeof(datosConexionCliente);
	while (true) {
		printf("Escuchando.. \n");
		int socketMemoria = accept(listenningSocket, (struct sockaddr *) &datosConexionCliente, &datosConexionClienteSize);
		if (socketMemoria != -1) {
			printf("Request de memoria recibida.. \n");
			pthread_t threadId;
			pthread_create(&threadId, NULL, (void*) procesarAccion, (void*) socketMemoria);
			pthread_detach(threadId);

		}

	}
}

void procesarAccion(int socketMemoria) {
	Paquete paquete;
	void* datos;
	if (RecibirPaqueteServidor(socketMemoria, FILESYSTEM, &paquete) > 0) {
		if (paquete.header.quienEnvia == MEMORIA) {

			datos = malloc(paquete.header.tamanioMensaje);
			datos = paquete.mensaje;
			int valueMaximo = 100;
			switch ((int) paquete.header.tipoMensaje) {
			case (CONEXION_INICIAL_FILESYSTEM_MEMORIA):
				configuracionNuevaMemoria(socketMemoria, valueMaximo);
				break;
			case (SELECT):
				//
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
			log_info(logger, "No es ningun proceso de Memoria");
		}

	}

	if (paquete.mensaje != NULL) {
		free(paquete.mensaje);
	}

}

void configuracionNuevaMemoria(int socketMemoria, int valueMaximo) {
	printf("Nueva memoria conectada. Socket N:%d", socketMemoria);
	char valueMaximoChar[10];
	sprintf(valueMaximoChar, "%d", valueMaximo);
	EnviarDatosTipo(socketMemoria, FILESYSTEM, valueMaximoChar, strlen(valueMaximoChar) + 1, CONEXION_INICIAL_FILESYSTEM_MEMORIA);

}

void procesarINSERT(char* request, int socketMemoria) {
	char** valores = string_split(request, "\""); //34 son las " en ASCII
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

}

void procesarCREATE(char* request, int socketMemoria) {
	char** valores = string_split(request, " ");
	char* nombreTabla = valores[0];
	char* consistenciaChar = valores[1];
	char* cantParticiones = valores[2];
	char* tiempoCompactacion = valores[3];

	int resultado = funcionCREATE(nombreTabla, cantParticiones, consistenciaChar, tiempoCompactacion);
	enviarSuccess(resultado, CREATE, socketMemoria);
}

void procesarDROP(char* nombreTabla, int socketMemoria) {
	int resultado = funcionDROP(nombreTabla);
	enviarSuccess(resultado, DROP, socketMemoria);
}

//esta funcion podria ser mucho mas linda, pero el obtenerNombreTablas no me sale
void procesarDESCRIBE_ALL(int socketMemoria) {

	t_list* listaDirectorios = list_create();
	buscarDirectorios(rutas.Tablas, listaDirectorios);

	char* obtenerNombreTablaByRuta(char* rutaTabla) {
		char** directorios = string_split(rutaTabla, "/");
		return (char*) obtenerUltimoElementoDeUnSplit(directorios);
	}

	void describeDirectorio(char* directorio) {
		char* nombreTabla = obtenerNombreTablaByRuta(directorio);
		procesarDESCRIBE(nombreTabla, socketMemoria);
	}

	list_iterate(listaDirectorios, (void*) describeDirectorio);
	EnviarDatosTipo(socketMemoria, FILESYSTEM, "fin", 4, DESCRIBE_ALL);
	list_destroy(listaDirectorios);

}

void procesarDESCRIBE(char* nombreTabla, int socketMemoria) {

	if (existeTabla(nombreTabla)) {
		t_metadata_tabla metadata = funcionDESCRIBE(nombreTabla);
		char respuesta[100];
		sprintf(respuesta, "%s %s %d %d", nombreTabla, getConsistenciaCharByEnum(metadata.CONSISTENCIA), metadata.CANT_PARTICIONES,
				metadata.T_COMPACTACION);
		EnviarDatosTipo(socketMemoria, FILESYSTEM, respuesta, strlen(respuesta) + 1, DESCRIBE);
		return;
	}
	enviarSuccess(1, DESCRIBE, socketMemoria);

}

void enviarSuccess(int resultado, t_protocolo protocolo, int socketMemoria) {
	char success[2];
	sprintf(success, "%d", resultado);
	EnviarDatosTipo(socketMemoria, FILESYSTEM, success, 2, protocolo);
}

