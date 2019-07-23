#ifndef AdministradorDeConsultasLFS_H_
#define AdministradorDeConsultasLFS_H_

#include <Libraries.h>
#include <commons/string.h>
#include "funcionesLFS.h"
#include "FileSystem.h"

int funcionCREATE(char* nombreTabla, char* cantParticiones, char* consistenciaChar, char* tiempoCompactacion);
int funcionDROP(char* nombreTabla);
t_metadata_tabla funcionDESCRIBE(char* nombreTabla);
void funcionDESCRIBE_ALL();
int funcionINSERT(double timeStamp, char* nombreTabla, char* key, char* value);
void iniciarProcesoDump();
t_registro* funcionSELECT(char*nombreTabla, int keyActual);

#endif /* AdministradorDeConsultasLFS_H_ */
