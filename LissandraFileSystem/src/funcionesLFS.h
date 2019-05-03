/*
 * funcionesLFS.h
 *
 *  Created on: 23 abr. 2019
 *      Author: utnso
 */

#ifndef FUNCIONESLFS_H_
#define FUNCIONESLFS_H_

#include "Libraries.h"
#include "FileSystem.h"

//consola

void procesarInput(char * linea);
void consolaLFS(void);
void consolaDrop(char** palabras, int cantidadParametros);
void consolaInsert(char**palabras, int cantidadParametros);
void consolaSelect(char**palabras, int cantidadParametros);
void consolaCreate(char**palabras, int cantidadParametros);
void consolaDescribe(char**palabras, int cantidadParametros);

//tablas

int existeTabla(char* nombreTabla);
char * obtenerRutaTablaSinArchivo(char * rutaTabla);
void crearTablaYParticiones(char* nombreTabla, char* cantidadParticiones);
void crearMetadataTabla (char*nombreTabla, char* consistencia, char* cantidadParticiones, char* tiempoCompactacion);
char* armarRutaTabla(char* rutaTabla, char* nombreTabla);
void mostrarMetadataTabla(char* nombreTabla);
void removerTabla(char* nombreTabla);
//void mostrarMetadataTodasTablas();

//ARCHIVOS DE TABLA

char** buscarArchivos(char * rutaTabla);
int esArchivo (char* ruta);
void removerArchivosDeTabla(char * rutaTabla);
int leerArchivoDeTabla(char *rutaArchivo, t_archivo *archivo);
int liberarBloquesDeArchivo(char *rutaArchivo);

#endif /* FUNCIONESLFS_H_ */
