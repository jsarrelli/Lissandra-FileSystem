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

//t_list* listaNombresTablas;
double getCurrentTime();

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
void mostrarMetadataTabla2(char* nombreTabla);
void removerTabla(char* nombreTabla);
void mostrarMetadataTodasTablas(char *ruta);
t_tabla_memtable * obtenerTablaDeMemtable(char* nombreTabla);
void insertarKey(char* nombreTabla, char* key, char* value, double timestamp);
void obtenerRegistrosDeTabla(char* nombreTabla);

int obtenerTamanioArrayRegistros(char** registros);


//ARCHIVOS DE TABLA

char** buscarArchivos(char * rutaTabla);
int esArchivo (char* ruta);
void removerArchivosDeTabla(char * rutaTabla);
int leerArchivoDeTabla(char *rutaArchivo, t_archivo *archivo);
int liberarBloquesDeArchivo(char *rutaArchivo);
char * obtenerNombreDeArchivoDeUnaRuta(char * ruta);
void crearArchivosTemporales(char*ruta);
void crearTemporal(char*nombreTabla);
void crearArchReservarBloqueYEscribirBitmap(char* rutaArch);
void escribirArchivo(FILE* arch, t_archivo *archivo);

int escribirEnTmp (char* nombreTabla,char*rutaTmp);

#endif /* FUNCIONESLFS_H_ */
