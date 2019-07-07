#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/inotify.h>
#include <commons/string.h>
#include "Libraries.h"

char* rutaDirectorio;
void (*funcion)(void);
void esperarModificacionArchivo();
void listenArchivo(char* directorio, void (*cargarConfiguracion)(void) );
