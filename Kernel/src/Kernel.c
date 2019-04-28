/*
 ============================================================================
 Name        : Kernel.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include "SocketCliente.h"

int main(void) {
	puts("!!!Hello World!!!"); /* prints !!!Hello World!!! */
	if(configurarSocketCliente()){
		enviarConsulta();
	}
	close(serverSocket);
	return EXIT_SUCCESS;
}
