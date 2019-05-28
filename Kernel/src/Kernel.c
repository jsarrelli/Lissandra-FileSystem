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

#include "SocketClienteMemoria.h"

int main(void) {
	if(configurarSocketCliente()){

		INSERT(32, "juli", "tabla1");
	}
	close(serverSocket);
	return EXIT_SUCCESS;
}
