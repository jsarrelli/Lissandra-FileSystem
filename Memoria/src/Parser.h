
#include <stdio.h>
#include <stdlib.h>
#include "AdministradorDeConsultas.h"
#include <Libraries.h>

void procesarConsulta(char* consulta);
void* procesarINSERT(char* consulta);
void* procesarSELECT(char* consulta);
int procesarCREATE(char* consulta);
int procesarDROP(char* consulta);
void procesarDESCRIBE(char* consulta);
