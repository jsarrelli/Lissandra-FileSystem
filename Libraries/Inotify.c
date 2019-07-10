#include "Inotify.h"
void esperarModificacionArchivo() {

	pthread_detach(pthread_self());
	int EVENT_SIZE = sizeof(struct inotify_event) + strlen(rutaDirectorio);
	int BUF_LEN = (1024 * EVENT_SIZE);
	char buffer[BUF_LEN];

	// Al inicializar inotify este nos devuelve un descriptor de archivo
	int file_descriptor = inotify_init();
	if (file_descriptor < 0) {
		perror("inotify_init");
	}

	// Creamos un monitor sobre un path indicando que eventos queremos escuchar
	int watch_descriptor = inotify_add_watch(file_descriptor, rutaDirectorio, IN_MODIFY);

	// El file descriptor creado por inotify, es el que recibe la información sobre los eventos ocurridos
	// para leer esta información el descriptor se lee como si fuera un archivo comun y corriente pero
	// la diferencia esta en que lo que leemos no es el contenido de un archivo sino la información
	// referente a los eventos ocurridos

	int length = read(file_descriptor, buffer, BUF_LEN);
	if (length < 0) {
		perror("read");
	}

	int offset = 0;

	// Luego del read buffer es un array de n posiciones donde cada posición contiene
	// un eventos ( inotify_event ) junto con el nombre de este.
	while (offset < length) {

		// El buffer es de tipo array de char, o array de bytes. Esto es porque como los
		// nombres pueden tener nombres mas cortos que 24 caracteres el tamaño va a ser menor
		// a sizeof( struct inotify_event ) + 24.
		struct inotify_event *event = (struct inotify_event *) &buffer[offset];

		// El campo "len" nos indica la longitud del tamaño del nombre
		if (event->len) {
			// Dentro de "mask" tenemos el evento que ocurrio y sobre donde ocurrio
			// sea un archivo o un directorio
			if (event->mask & IN_CREATE) {
				if (event->mask & IN_ISDIR) {
					printf("The directory %s was created.\n", event->name);
				} else {
					printf("The file %s was created.\n", event->name);
				}
			} else if (event->mask & IN_DELETE) {
				if (event->mask & IN_ISDIR) {
					printf("The directory %s was deleted.\n", event->name);
				} else {
					printf("The file %s was deleted.\n", event->name);
				}
			} else if (event->mask & IN_MODIFY) {
				if (event->mask & IN_ISDIR) {
					printf("The directory %s was modified.\n", event->name);
				} else {
					printf("The file %s was modified.\n", event->name);
					inotify_rm_watch(file_descriptor, watch_descriptor);
					close(file_descriptor);
					funcion();
				}
			}
		}
		offset += sizeof(struct inotify_event) + event->len;
	}

	inotify_rm_watch(file_descriptor, watch_descriptor);
	close(file_descriptor);

}

void listenArchivo(char* directorio, void (*cargarConfiguracion)(void)) {
	rutaDirectorio = directorio;
	funcion = cargarConfiguracion;
	pthread_t thread;
	pthread_create(&thread, NULL, (void*) esperarModificacionArchivo, NULL);

}

