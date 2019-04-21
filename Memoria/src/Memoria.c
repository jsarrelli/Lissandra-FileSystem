/*
 ============================================================================
 Name        : Memoria.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include "API.h"
int main(int argc, char** argv) {
	puts("--Memoria inicializada--"); /* prints !!!Hello World!!! */
	if (argc > 1) {
			procesarConsulta(argc,argv);
		}
	return EXIT_SUCCESS;
}
