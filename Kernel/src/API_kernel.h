#ifndef API_KERNEL_H_
#define API_KERNEL_H_

#include "kernel.h"

// Es necesario un enum de consistencia, si ya tengo el diccionario??
typedef enum{
	SC,
	SHC,
	EV
}t_consistencia;

typedef struct{
	char* CONSISTENCIA;  // Cambiarlo por el enum, creo que lo puede hacer el Kernel
	int CANT_PARTICIONES;
	int T_COMPACTACION;
}t_metadata_tabla;

typedef t_list
		t_list_info_memoria;

// Esto si o si lo manda la memoria que lo recibe del LFS
typedef struct{
	char*nombre_tabla;
	t_metadata_tabla metadata_tabla;
}info_una_tabla;

// Se me puede enviar, info_memorias o t_list_info_memorias.
typedef struct{
	char* nombre_memoria; // Esto no importa
	int id_memoria; // Esto si es importante
	t_list_info_memoria* lista_info_memorias; // Esta cargada con info_una_tabla
}info_memorias; // Para diferenciarlos: info_una_tabla es lo que se manda del LFS e info_memorias es lo que se manda de las Memorias

void consolaKernel();
void procesarInput(char* linea);
void consolaInsert(char**palabras, int cantidad);
void consolaSelect(char**palabras, int cantidad);
void consolaAdd(char**palabras, int cantidad);

#endif /* API_KERNEL_H_ */
