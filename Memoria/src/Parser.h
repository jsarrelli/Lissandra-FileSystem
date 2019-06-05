
#include <stdio.h>
#include <stdlib.h>
#include "AdministradorDeConsultas.h"
#include "Libraries.h"

void* procesarConsulta(char* consulta);
void* procesarINSERT(char* consulta);
void* procesarSELECT(char* consulta);
void* procesarCREATE(char* consulta);
void* procesarDROP(char* consulta);
void* procesarDESCRIBE(char* consulta);
