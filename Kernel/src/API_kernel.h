#ifndef API_KERNEL_H_
#define API_KERNEL_H_

#include "kernel.h"

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


/*
 * O sea, basicamente va a haber una lista info memorias que guarde la info de las memorias
 *
 *
 */

void consolaKernel();
void procesarInput(char* linea);
void consolaInsert(char**palabras, int cantidad);
void consolaSelect(char**palabras, int cantidad);
void consolaAdd(char**palabras, int cantidad);

#endif /* API_KERNEL_H_ */
