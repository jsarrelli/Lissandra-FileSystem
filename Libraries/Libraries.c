#include "Libraries.h"
int contadorConexiones = 0;

int get_campo_config_int(t_config* archivo_configuracion, char* nombre_campo) {
	int valor;
	if (config_has_property(archivo_configuracion, nombre_campo)) {
		valor = config_get_int_value(archivo_configuracion, nombre_campo);
		printf("El %s es: %i\n", nombre_campo, valor);
		return valor;
	}
	return NULL;
}

char* get_campo_config_string(t_config* archivo_configuracion, char* nombre_campo) {
	char* valor;
	if (config_has_property(archivo_configuracion, nombre_campo)) {
		valor = config_get_string_value(archivo_configuracion, nombre_campo);
		printf("El %s es: %s\n", nombre_campo, valor);
		return valor;
	}
	return NULL;
}

char** get_campo_config_array(t_config* archivo_configuracion, char* nombre_campo) {
	char** valor;
	if (config_has_property(archivo_configuracion, nombre_campo)) {
		valor = config_get_array_value(archivo_configuracion, nombre_campo);
		printf("El %s es: %s\n", nombre_campo, valor);
		return valor;
	}
	return NULL;
}

void logErrorAndExit(char * mensaje) {
	log_error(loggerError, mensaje);
	exit(-1);
}

void inicializarArchivoDeLogs(char * ruta) {
	FILE * archivo = fopen(ruta, "w");
	fclose(archivo);
}

int contarPunteroDePunteros(char ** puntero) {
	char ** aux = puntero;
	int contador = 0;
	while (*aux != NULL) {
		contador++;
		aux++;
	}
	return contador;
}

void liberarPunteroDePunterosAChar(char** palabras) {
	int i = 0;
	while (palabras[i] != NULL) {
		free(palabras[i]);
		i++;
	}

}

void freePunteroAPunteros(char** palabras) {
	liberarPunteroDePunterosAChar(palabras);
	free(palabras);
}

void crearHilo(pthread_t * nombreHilo, void * nombreFuncion, void * parametros) {
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	if (pthread_create(nombreHilo, &attr, nombreFuncion, parametros) < 0) {
		logErrorAndExit("No se pudo crear el hilo.");
	}
}

int cantidadParametros(char ** palabras) {
	int i = 1;
	while (palabras[i] != NULL) {
		i++;
	}
	return i - 1;
}

char * obtenerUltimoElementoDeUnSplit(char ** palabras) {
	char * ultimaPalabra;
	int i = 0;
	while (palabras[i] != NULL) {
		ultimaPalabra = palabras[i];
		i++;
	}
	return strdup(ultimaPalabra);
}

int recibirConexion(int socket_servidor);
//--------LOG----------------//

int recibirConexion(int socket_servidor) {
	struct sockaddr_storage their_addr;
	socklen_t addr_size;

	int estado = listen(socket_servidor, 5);

	if (estado == -1) {
		log_info(loggerError, "\nError al poner el servidor en listen\n");
		close(socket_servidor);
		return 1;
	}

	if (estado == 0) {
		//	log_info(logConsolaPantalla,"\nSe puso el socket en listen\n");
		//	printf("---------------------------------------------------\n");
	}

	addr_size = sizeof(their_addr);

	int socket_aceptado;
	socket_aceptado = accept(socket_servidor, (struct sockaddr *) &their_addr, &addr_size);

	contadorConexiones++;
	printf("\n----------Nueva Conexion aceptada numero: %d ---------\n", contadorConexiones);
	//printf("----------Handler asignado a (%d) ---------\n",contadorConexiones);

	if (socket_aceptado == -1) {
		close(socket_servidor);
		log_error(loggerError, "\nError al aceptar conexion\n");
		return 1;
	}
	return socket_aceptado;
}

int crear_socket_cliente(char * ip, char * puerto) {

	int descriptorArchivo, estado;
	struct addrinfo hints, *infoServer, *n;

	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if ((estado = getaddrinfo(ip, puerto, &hints, &infoServer)) != 0) {
		fprintf(stderr, "Error en getaddrinfo: %s", gai_strerror(estado));
		return -1;
	}

	for (n = infoServer; n != NULL; n = n->ai_next) {
		descriptorArchivo = socket(n->ai_family, n->ai_socktype, n->ai_protocol);
		if (descriptorArchivo != -1)
			break;
	}

	if (descriptorArchivo == -1) {
		perror("Error al crear el socket");
		freeaddrinfo(infoServer);
		return -1;
	}
	estado = connect(descriptorArchivo, n->ai_addr, n->ai_addrlen);

	if (estado == -1) {
		perror("Error conectando el socket");
		freeaddrinfo(infoServer);
		return -1;
	}

	freeaddrinfo(infoServer);

	return descriptorArchivo;
}

int crear_socket_servidor(char *puerto) {
	int descriptorArchivo, estado;
	struct addrinfo hints, *infoServer, *n;

	memset(&hints, 0, sizeof(struct addrinfo));

	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if ((estado = getaddrinfo(NULL, puerto, &hints, &infoServer)) != 0) {
		fprintf(stderr, "Error en getaddrinfo: %s", gai_strerror(estado));

		return -1;
	}

	for (n = infoServer; n != NULL; n = n->ai_next) {
		descriptorArchivo = socket(n->ai_family, n->ai_socktype, n->ai_protocol);
		if (descriptorArchivo != -1)
			break;
	}

	if (descriptorArchivo == -1) {
		perror("Error al crear el socket");
		freeaddrinfo(infoServer);

		return -1;
	}

	int si = 1;

	if (setsockopt(descriptorArchivo, SOL_SOCKET, SO_REUSEADDR, &si, sizeof(int)) == -1) {
		perror("Error en setsockopt");
		//   close(descriptorArchivo);
		freeaddrinfo(infoServer);

		return -1;
	}

	if (bind(descriptorArchivo, n->ai_addr, n->ai_addrlen) == -1) {
		perror("Error bindeando el socket");
		//   close(descriptorArchivo);
		freeaddrinfo(infoServer);

		return -1;
	}

	freeaddrinfo(infoServer);

	return descriptorArchivo;
}

char* recibir_string(int socket_aceptado) {
	return (char*) recibir(socket_aceptado);
}

void enviar_string(int socket, char* mensaje) {
	int tamanio = string_length(mensaje) + 1;

	enviar(socket, (void*) mensaje, tamanio);
}

void enviar(int socket, void* cosaAEnviar, int tamanio) {
	void* mensaje = malloc(sizeof(int) + tamanio);
	void* aux = mensaje;
	*((int*) aux) = tamanio;
	aux += sizeof(int);
	memcpy(aux, cosaAEnviar, tamanio);

	send(socket, mensaje, sizeof(int) + tamanio, 0);
	free(mensaje);
}

void* recibir(int socket) {
	int checkSocket = -1;

	void* recibido = malloc(sizeof(int));

	checkSocket = read(socket, recibido, sizeof(int));

	int tamanioDelMensaje = *((int*) recibido);

	free(recibido);

	if (!checkSocket)
		return NULL;

	recibido = malloc(tamanioDelMensaje);

	int bytesRecibidos = 0;

	while (bytesRecibidos < tamanioDelMensaje && checkSocket) {
		checkSocket = read(socket, (recibido + bytesRecibidos), (tamanioDelMensaje - bytesRecibidos));
		bytesRecibidos += checkSocket;
	}

	return !checkSocket ? NULL : recibido;
}

void *get_in_addr(struct sockaddr *sa) {
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*) sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*) sa)->sin6_addr);
}

int tamanioArchivo(FILE*archivo) {
	int tamanio;
	fseek(archivo, 0L, SEEK_END);
	tamanio = ftell(archivo);
	fseek(archivo, 0L, SEEK_SET);

	return tamanio;
}

double getCurrentTime() {
	struct timeval tv;
	gettimeofday(&tv, NULL);
	unsigned long long result = (((unsigned long long) tv.tv_sec) * 1000 + ((unsigned long long) tv.tv_usec) / 1000);
	double res = result;
	return res;
}

t_consistencia getConsistenciaByChar(char* consistenciaChar) {
	t_consistencia consistencia;
	if (strcmp(consistenciaChar, "SC") == 0) {
		consistencia = STRONG;
	} else if (strcmp(consistenciaChar, "SHC") == 0) {
		consistencia = STRONG_HASH;
	} else {
		consistencia = EVENTUAL;
	}

	return consistencia;
}

char* getConsistenciaCharByEnum(t_consistencia consistencia) {
	switch (consistencia) {
	case STRONG:
		return "SC";
	case STRONG_HASH:
		return "SHC";
	case EVENTUAL:
		return "EC";
	}
	return NULL;

}

void list_iterate2(t_list* self, void (*closure)(void*, void*), void* segundoParametro) {
	t_link_element *element = self->head;
	t_link_element *aux = NULL;
	while (element != NULL) {
		aux = element->next;
		closure(element->data, segundoParametro);
		element = aux;
	}
}

t_registro* registro_new(char** valores) {
	t_registro* registro = malloc(sizeof(t_registro));
	registro->key = atoi(valores[1]);
	registro->value = string_duplicate(valores[2]);
	registro->timestamp = atof(valores[0]);
	freePunteroAPunteros(valores);
	return registro;
}

t_registro* registro_duplicate(t_registro* registro) {
	t_registro* registroDuplicado = malloc(sizeof(t_registro));
	registroDuplicado->key = registro->key;
	registroDuplicado->value = string_duplicate(registro->value);
	registroDuplicado->timestamp = registro->timestamp;
	return registroDuplicado;
}

void freeRegistro(t_registro* registro) {
	if (registro != NULL) {
		if (registro->value != NULL) {
			free(registro->value);
		}
		free(registro);
	}
}
