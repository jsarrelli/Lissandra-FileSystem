/*
 ============================================================================
 Name        : LissandraFileSystem.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */


#include <LissandraFileSystem.h>

int main(void) {
	puts("!!!Hello World!!!"); /* prints !!!Hello World!!! */
	return EXIT_SUCCESS;

}

int cargarConfig (){
		t_configuracion *config = malloc(sizeof(t_configuracion));
		t_config *fsConfig = config_create("./fsConfig.cfg");

		config->PUERTO_ESCUCHA = config_get_int_value(fsConfig, "PUERTO_ESCUCHA");
		config->PUNTO_MONTAJE = config_get_string_value(fsConfig, "PUNTO_MONTAJE");
		config->RETARDO = config_get_int_value(fsConfig, "RETARDO");
		config->TAMANIO_VALUE = config_get_int_value(fsConfig, "TAMANIO_VALUE");
		config->TIEMPO_DUMP = config_get_int_value(fsConfig, "TIEMPO_DUMP");

		config_destroy(fsConfig);

		return 1;
	}


