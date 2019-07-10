#ifndef ApiLFS_H_
#define ApiLFS_H_

#include <Libraries.h>
#include <Sockets/Serializacion.h>
#include <Sockets/Conexiones.h>
#include "AdministradorConsultasLFS.h"
#include <readline/readline.h>
#include <readline/history.h>

void* consolaLFS();
void procesarInput(char* consulta);
void consolaCreate(char*argumentos);
void consolaDescribe(char* nombreTabla);
void consolaDrop(char* nombreTabla);
void consolaInsert(char* argumentos);
void consolaSelect(char*argumentos);

#endif /* ApiLFS_H_*/
