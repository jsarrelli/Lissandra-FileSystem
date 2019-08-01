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
#include "Compactador.h"
#include <stdlib.h>
#include <math.h>
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

void limpiarRegistrosDeTabla(char*nombreTabla);
int obtenerTamanioListaRegistros(t_list* registros);

//Funciones del select
void getRegistrosFromBinByNombreTabla(char*nombreTabla, int keyActual, t_list* listaRegistros);
void getRegistrosFromMemtableByNombreTabla(char* nombreTabla, t_list* listaRegistros);
void getRegistrosFromTmpByNombreTabla(char* nombreTabla, t_list* listaRegistros);
void getRegistrosFromTempcByNombreTabla(char* nombreTabla, t_list* listaRegistros);

t_registro* getRegistroByKeyAndNombreTabla(char*nombreTabla, int keyActual);
t_registro* getRegistroFromMemtableByKey(char* nombreTabla, int key);
t_registro* getRegistroFromTmpByKey(char* nombreTabla, int key);
t_registro* getRegistroFromTmpcByKey(char* nombreTabla, int key);
t_registro* getRegistroFromBinByKey(char* nombreTabla, int key);
t_registro* buscarRegistroByKeyFromListaRegistros(t_list* listaRegistros, int key);

t_metadata_tabla obtenerMetadata(char* nombreTabla);
void mostrarMetadataTodasTablas(char *ruta);

//ARCHIVOS DE TABLA
void buscarDirectorios(char * ruta, t_list* listaDirectorios);
t_list* buscarArchivos(char* nombreTabla);
char** buscarArchivosTemporales(char * rutaTabla);

int esArchivo(char* ruta);
void removerArchivosDeTabla(char * nombreTabla);
int leerArchivoDeTabla(char *rutaArchivo, t_archivo *archivo);
int liberarBloquesDeArchivo(char *rutaArchivo);
char * obtenerNombreDeArchivoDeUnaRuta(char * ruta);
char * obtenerExtensionDeArchivoDeUnaRuta(char * rutaLocal);
void procesoDump();
void dumpearTabla(char*rutaTabla);
void crearArchivo(char* rutaArch);
void escribirArchivo(char*rutaArchivo, t_archivo *archivo);
int contarArchivosTemporales(t_list* archivos);
int escribirRegistrosEnBloquesByPath(t_list* registrosAEscribir, char*pathArchivoAEscribir);
t_list* buscarRegistrosDeTabla(char*nombreTabla);

char* obtenerNombreTablaByRuta(char* rutaTabla);
t_tabla_memtable* getTablaFromMemtable(char* nombreTabla);
void eliminarArchivo(char* rutaArchivo);

void insertarTablaEnMemtable(char* nombreTabla);
t_tabla_memtable* newTablaMemtable(char* nombreTabla);
FILE* obtenerArchivoBloque(int numeroBloque, bool sobreEscribirBloques);
void borrarContenidoArchivoBloque(int bloque);
void freeArchivo(t_archivo *archivo);


void vaciarMemtable();
void freeTabla(t_tabla_memtable* tabla);

int tamanioTotalBloquesPorArchivo(t_list* bloques);
char* armarRutaBloque(int bloque);
void asignarBloquesArchivo(t_archivo* archivo, int cantidadBloqueNecesarios);

t_list* getRegistrosFromMemtable(char* nombreTabla);

#endif /* FUNCIONESLFS_H_ */
