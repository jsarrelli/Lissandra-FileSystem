#ifndef API_KERNEL_H_
#define API_KERNEL_H_

#include "Kernel.h"
#include "ConfigKernel.h"
//#include "ConfigKernel.h"

// Por ahora estoy dudando de esto

//// Es necesario un enum de consistencia, si ya tengo el diccionario??
//typedef enum{
//	SC,
//	SHC,
//	EV
//}t_consistencia;
//
//typedef struct{
//	char* CONSISTENCIA;  // Cambiarlo por el enum, creo que lo puede hacer el Kernel
//	int CANT_PARTICIONES;
//	int T_COMPACTACION;
//}t_metadata_tabla_Kernel;
//
//typedef t_list
//		t_list_info_memoria; // Esto va a ser una lista de info_memorias
//
//// Esto si o si lo manda la memoria que lo recibe del LFS
//typedef struct{
//	char*nombre_tabla;
//	t_metadata_tabla metadata_tabla;
//}info_una_tabla;
//
//// Se me puede enviar, info_memorias o t_list_info_memorias.
//typedef struct{
//	char* nombre_memoria; // Esto no importa
//	int ip_memoria; // Esto si es importante, para crear la conexion con las sockets
//	t_list* lista_info_tablas; // Esta cargada con info_una_tabla 's
//}info_memorias; // Para diferenciarlos: info_una_tabla es lo que se manda del LFS e info_memorias es lo que se manda de las Memorias
int socketMemoria; //en este caso que tenemos una sola memoria

void consolaKernel(int socketMemoriaRecibido);
void procesarInput(char* linea);
void consolaInsert(char*consulta);
void consolaSelect(char* consulta);
void consolaAdd(char*consulta);
void consolaRun(char* consulta);

#endif /* API_KERNEL_H_ */
