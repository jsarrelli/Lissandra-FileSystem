/*
 * funcionesLFS.h
 *
 *  Created on: 23 abr. 2019
 *      Author: utnso
 */

#ifndef FUNCIONESLFS_H_
#define FUNCIONESLFS_H_

#include <Libraries.h>
#include "LissandraFileSystem.h"
#include "EstructurasFileSystem.h"
//t_list* listaNombresTablas;

//tablas

int existeTabla(char* nombreTabla);
char * obtenerRutaTablaSinArchivo(char * rutaTabla);
void crearTablaYParticiones(char* nombreTabla, char* cantidadParticiones);
void crearMetadataTabla(char*nombreTabla, char* consistencia, char* cantidadParticiones, char* tiempoCompactacion);
char* armarRutaTabla(char* nombreTabla);
void mostrarMetadataTabla(t_metadata_tabla metadataTabla, char* nombreTabla);
void mostrarMetadataTabla2(char* nombreTabla);
void removerTabla(char* nombreTabla);
void mostrarMetadataTodasTablas(char *ruta);

void insertarKey(char* nombreTabla, char* key, char* value, double timestamp);
//char** buscarRegistrosDeTabla(char*nombreTabla);
void buscarDirectorios(char * ruta, t_list* listaDirectorios);
void limpiarRegistrosDeTabla(char*nombreTabla);
int obtenerTamanioListaRegistros(t_list* registros);

t_metadata_tabla obtenerMetadata(char* nombreTabla);
void mostrarMetadataTodasTablas(char *ruta);

//ARCHIVOS DE TABLA

t_list* buscarArchivos(char* nombreTabla);
char** buscarArchivosTemporales(char * rutaTabla);
int esArchivo(char* ruta);
void removerArchivosDeTabla(char * nombreTabla);
int leerArchivoDeTabla(char *rutaArchivo, t_archivo *archivo);
int liberarBloquesDeArchivo(char *rutaArchivo);
char * obtenerNombreDeArchivoDeUnaRuta(char * ruta);
char * obtenerExtensionDeArchivoDeUnaRuta(char * rutaLocal);
void crearYEscribirArchivosTemporales(char*ruta);
void crearYEscribirTemporal(char*rutaTabla);
void crearArchReservarBloqueYEscribirBitmap(char* rutaArch);
void escribirArchivo(char*rutaArchivo, t_archivo *archivo);
int contarArchivosTemporales(t_list* archivos);
int escribirEnTmp(char*nombreTabla, char*rutaTmp);

char* obtenerNombreTablaByRuta(char* rutaTabla);
t_tabla_memtable* getTablaFromMemtable(char* nombreTabla);
void eliminarArchivo(char* rutaArchivo);

#endif /* FUNCIONESLFS_H_ */
