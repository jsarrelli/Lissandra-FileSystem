#include "SocketClienteMemoria.h"

Segmento* buscarSegmentoEnFileSystem(char* nombreSegmento) {
	EnviarDatosTipo(socketFileSystem, MEMORIA, (void*) nombreSegmento, strlen(nombreSegmento), DESCRIBE);
	Paquete paquete;
	RecibirPaqueteCliente(socketFileSystem, FILESYSTEM, &paquete);

	if (paquete.header.tipoMensaje == NOTFOUND) {
		return NULL;
	}

	void*cadenaRecibida = malloc(paquete.header.tamanioMensaje);
	char** datos = string_split(cadenaRecibida, " ");
	char* nombreSegmentoRecibido = datos[0];
	t_consistencia consistencia = getConsistenciaByChar(datos[1]);
	int cantParticiones = atoi(datos[2]);
	int tiempoCompactacion = atoi(datos[3]);

	t_metadata_tabla* metadata = malloc(sizeof(t_metadata_tabla));
	metadata->CONSISTENCIA = consistencia;
	metadata->CANT_PARTICIONES = cantParticiones;
	metadata->T_COMPACTACION = tiempoCompactacion;

	Segmento* segmentoRecibido=malloc(sizeof(Segmento));
	segmentoRecibido->nombreTabla=malloc(strlen(nombreSegmentoRecibido));
	strcpy(segmentoRecibido->nombreTabla,nombreSegmentoRecibido);
	strcpy(segmentoRecibido->nombreTabla, nombreSegmentoRecibido);
	segmentoRecibido->metaData = metadata;

	return segmentoRecibido;
}
