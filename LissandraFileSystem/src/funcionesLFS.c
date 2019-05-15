/*
 * funcionesLFS.c
 *
 *  Created on: 23 abr. 2019
 *      Author: utnso
 */

#include "funcionesLFS.h"

int cantTablas = 0;

char * obtenerRutaTablaSinArchivo(char * rutaTabla){
	char ** directorios;
	char * archivo = malloc(50);
	int tamanioNombreArchivo;
	int tamanioRuta;

	directorios = string_split(rutaTabla, "/");
	archivo = obtenerUltimoElementoDeUnSplit(directorios);
	tamanioNombreArchivo = strlen(archivo) +1;
	tamanioRuta = strlen(rutaTabla);

	liberarPunteroDePunterosAChar(directorios);
	free(directorios);
	free(archivo);
	return string_substring_until(rutaTabla,tamanioRuta-tamanioNombreArchivo);
}

int existeTabla(char* nombreTabla){
	char* rutaTabla = malloc(100);
	DIR* tablaActual;
	armarRutaTabla(rutaTabla, nombreTabla);
	tablaActual = opendir(rutaTabla);

			if(tablaActual !=NULL){
				closedir(rutaTabla);
				return 1;
			}



	free(rutaTabla);
	return 0;
}

void crearTablaYParticiones(char* nombreTabla, char* cantidadParticiones){
	t_tabla_memtable* tabla;


	char* rutaTabla= malloc(100);
	strcpy(rutaTabla, rutas.Tablas);
	int i=0, j=1;
	int cantPart = atoi(cantidadParticiones);

	string_append(&rutaTabla,"/");
	string_append(&rutaTabla, nombreTabla);
	mkdir(rutaTabla,0777);
	printf("%s creada en LFS\n", nombreTabla);
	cantTablas += 1;
	log_info(logger, "Se creo la  %s en LFS\n", nombreTabla);

	while (i < cantPart){
		string_append(&rutaTabla,"/");
		string_append_with_format(&rutaTabla, "%d.bin", i);
		FILE* archivo = fopen(rutaTabla, "w+");
		int *bloque = buscarBloquesLibres(1);

			if(bloque == NULL){
				logErrorAndExit( "No hay bloques libres para crear el archivo");
			}
		reservarBloque(bloque[0]);

		escribirBitmap();

		fprintf(archivo, "TAMANIO=0\n");

		fprintf(archivo, "BLOQUES=[%i]", bloque[0]);

		fclose(archivo);
		free(bloque);
		printf("Particion %d creada \n", j);
		rutaTabla = obtenerRutaTablaSinArchivo(rutaTabla);
		i++;
		j++;
	}

	tabla= malloc(sizeof(t_tabla_memtable));
	strcpy(tabla->tabla,nombreTabla);
	tabla->registros= list_create();
	list_add(memtable, tabla);
	printf("%s insertada en memtable \n", tabla->tabla);

	free(rutaTabla);

}

void crearMetadataTabla (char*nombreTabla, char* consistencia, char* cantidadParticiones, char* tiempoCompactacion){
	char*rutaTabla=malloc(100);
	armarRutaTabla(rutaTabla, nombreTabla);
	string_append(&rutaTabla, "Metadata");
	FILE*arch = fopen(rutaTabla, "w+");
	fprintf(arch, "CONSISTENCIA=%s\nPARTICIONES=%s\nTIEMPO_COMPACTACION=%s\n", consistencia, cantidadParticiones, tiempoCompactacion);

	fclose(arch);
	log_info(logger, "Metadata de %s creada\n", nombreTabla);


}

void mostrarMetadataTabla(char* nombreTabla){

	char* rutaTabla=malloc(100);
	armarRutaTabla(rutaTabla, nombreTabla);
	string_append(&rutaTabla, "Metadata");
	t_config* configMetadata = config_create(rutaTabla);
	t_metadata_tabla* metadataTabla = malloc(sizeof (t_metadata_tabla));

	printf("\nMetadata de %s: \n", nombreTabla);

	metadataTabla->CONSISTENCIA = config_get_string_value(configMetadata, "CONSISTENCIA");
	metadataTabla->CANT_PARTICIONES = config_get_int_value(configMetadata, "PARTICIONES");
	metadataTabla->T_COMPACTACION = config_get_int_value(configMetadata, "TIEMPO_COMPACTACION");

	printf("CONSISTENCIA: %s\nPARTICIONES=%i\nTIEMPO_COMPACTACION=%i\n\n", metadataTabla->CONSISTENCIA, metadataTabla->CANT_PARTICIONES, metadataTabla->T_COMPACTACION);
	free(metadataTabla);
	free(rutaTabla);
	config_destroy(configMetadata);
}

void mostrarMetadataTabla2(char* nombreTabla){
	char*rutaTabla = malloc(100);
	char* nombTabla=string_new();
	strcpy(rutaTabla, nombreTabla);
	string_append(&nombreTabla, "/");
	string_append(&nombreTabla, "Metadata");
	t_config* configMetadata = config_create(nombreTabla);
	t_metadata_tabla* metadataTabla = malloc(sizeof (t_metadata_tabla));
	char** palabras = string_split(rutaTabla, "/");
	nombTabla = obtenerUltimoElementoDeUnSplit(palabras);
	printf("\nMetadata de %s: \n", nombTabla);

	metadataTabla->CONSISTENCIA = config_get_string_value(configMetadata, "CONSISTENCIA");
	metadataTabla->CANT_PARTICIONES = config_get_int_value(configMetadata, "PARTICIONES");
	metadataTabla->T_COMPACTACION = config_get_int_value(configMetadata, "TIEMPO_COMPACTACION");

	printf("CONSISTENCIA: %s\nPARTICIONES=%i\nTIEMPO_COMPACTACION=%i\n\n", metadataTabla->CONSISTENCIA, metadataTabla->CANT_PARTICIONES, metadataTabla->T_COMPACTACION);
	free(metadataTabla);
	free(nombTabla);
	liberarPunteroDePunterosAChar(palabras);
	free(palabras);
	free(rutaTabla);
	config_destroy(configMetadata);
}

char* armarRutaTabla(char* rutaTabla, char* nombreTabla){

		strcpy(rutaTabla, rutas.Tablas);
		string_append(&rutaTabla, nombreTabla);
		string_append(&rutaTabla, "/");

		return rutaTabla;
}

int existeArchivo(char*nombreTabla, char * rutaArchivo){
	int i =0;
	char ** archivos;
	char *archivo;
	char * ruta = malloc(100);
	char ** carpetas = string_split(rutaArchivo,"/");

	armarRutaTabla(ruta, nombreTabla);
	archivos = buscarArchivos(ruta);
	archivo = obtenerUltimoElementoDeUnSplit(carpetas);
	string_append(&ruta,archivo);

	if (archivos[i] != NULL) {
		while (archivos[i] != NULL) {
			if (string_equals_ignore_case(archivos[i], ruta)) {
				liberarPunteroDePunterosAChar(carpetas);
				free(carpetas);
				liberarPunteroDePunterosAChar(archivos);
				free(archivos);
				free(archivo);
				free(ruta);
				return 1;
			}
			i++;
		}
		liberarPunteroDePunterosAChar(archivos);
	}
	liberarPunteroDePunterosAChar(carpetas);
	free(carpetas);
	free(archivos);
	free(archivo);
	free(ruta);
	return 0;
}

char** buscarArchivos(char * rutaTabla){

	  DIR *directorioActual;
	  struct dirent *archivo;
	  char ** archivos = malloc(100);
	  char * rutaNueva;
	  int i = 0;

	  directorioActual = opendir(rutaTabla);

	  if (directorioActual == NULL){
	    puts("No se pudo abrir el directorio.");
	    log_error(loggerError,"No se pudo abrir el directorio.");

	  }else{
	  // Leo uno por uno los archivos que estan adentro del directorio actual
	  while ((archivo = readdir(directorioActual)) != NULL) {

		  //Con readdir aparece siempre . y .. como no me interesa no lo contemplo
		if ((strcmp(archivo->d_name, ".") != 0) && (strcmp(archivo->d_name, "..") != 0)) {

			rutaNueva = string_duplicate(rutaTabla);
			string_append(&rutaNueva,"/");
			string_append(&rutaNueva,archivo->d_name);
			archivos[i] = malloc(256);
				if(esArchivo(rutaNueva)){
					strcpy(archivos[i],rutaNueva);
					i++;
				}
			free(rutaNueva);

		}
	  }
	  closedir (directorioActual);
	  }

	  archivos[i] = NULL;

	  return archivos;
}

int esArchivo (char* ruta){
	struct stat estado;
	int i;

	stat(ruta,&estado);
	i= S_ISREG(estado.st_mode);

	return i;
}

int esDirectorio(char * ruta){
	struct stat estado;
	int i;

	stat(ruta,&estado);
	i = S_ISDIR(estado.st_mode);


	return i;
}

void removerArchivosDeTabla(char * rutaTabla){
	char ** archivos;
	int i = 0;
	archivos = buscarArchivos(rutaTabla);

	if(archivos[i] != NULL){
		while(archivos[i] != NULL){
			liberarBloquesDeArchivo(archivos[i]);
			remove(archivos[i]);
			i++;

		}
		liberarPunteroDePunterosAChar(archivos);
		free(archivos);
	}else{
		free(archivos);
	}
}

int liberarBloquesDeArchivo(char *rutaArchivo){

	t_archivo *archivo = malloc(sizeof(t_archivo));
	int result = leerArchivoDeTabla(rutaArchivo, archivo);
	if(result <0){
		return -1;
	}
	int i;
	for (i = 0; i < archivo->cantBloques ; ++i) {
		liberarBloque(strtol(archivo->BLOQUES[i],NULL,10));
		log_info(logger, "Bloque liberado: %s\n",archivo->BLOQUES[i]);
		printf( "Bloque liberado: %s\n",archivo->BLOQUES[i]);
		i++;
	}
	escribirBitmap();

	free(archivo);
	return 1;
}

int leerArchivoDeTabla(char *rutaArchivo, t_archivo *archivo){

	t_config* config = config_create(rutaArchivo);


	if(config==NULL){
		return -1;
		puts("El archivo no existe");
	}

	if (config_has_property(config, "TAMANIO")){
		archivo->TAMANIO = config_get_int_value(config,"TAMANIO");
	}else{
		return -2; //Archivo corrupto
	}
	if (config_has_property(config, "BLOQUES")){
		archivo->BLOQUES = config_get_array_value(config,"BLOQUES");
		int cant = 0;
		for (cant = 0 ; archivo->BLOQUES[cant] ; cant++);
		archivo->cantBloques = cant;
	}else{
		return -2;
	}


	return 1;
}

void removerTabla(char* nombreTabla){
	int tamanio = list_size(memtable);
	int i=0;


	while(tamanio != i){
		t_tabla_memtable * tabla = list_get(memtable, i);
		if(string_equals_ignore_case(tabla->tabla, nombreTabla)){
			list_remove(memtable, i);
			printf("%s eliminada de memtable\n", nombreTabla);
			break;

		}
			i++;
	}

	char* rutaTabla = malloc(200);
	armarRutaTabla(rutaTabla,nombreTabla);
	removerArchivosDeTabla(rutaTabla);
	rmdir(rutaTabla);
	free(rutaTabla);
}

t_tabla_memtable * obtenerTablaDeMemtable(char* nombreTabla){
	int i;
	for(i=0;i<list_size(memtable);i++){
		t_tabla_memtable *tabla = list_get(memtable,i);
		if(tabla->tabla==nombreTabla){
			return tabla;
		}
	}

	return NULL;
	printf("La tabla no se encuentra en la memtable");
}

char** buscarDirectorios(char * ruta){

	  DIR *directorioActual;
	  struct dirent *directorio;
	  char ** directorios = malloc(100);
	  char * rutaNueva;
	  int i = 0;

	  directorioActual = opendir(ruta);

	  if (directorioActual == NULL){
		  puts("No pudo abrir el directorio");
		  log_error(loggerError,"No se pudo abrir el directorio.");
	  }
	  else{
		  // Leo uno por uno los directorios que estan adentro del directorio actual
		  while ((directorio = readdir(directorioActual)) != NULL) {

			  //Con readdir aparece siempre . y .. como no me interesa no lo contemplo
			if ((strcmp(directorio->d_name, ".") != 0) && (strcmp(directorio->d_name, "..") != 0)) {


				rutaNueva = string_duplicate(ruta);
				string_append(&rutaNueva,directorio->d_name);

				directorios[i] = malloc(256);
				if(esDirectorio(rutaNueva)){
					strcpy(directorios[i],rutaNueva);
					i++;
				}
				free(rutaNueva);
			}

		  }


		  closedir (directorioActual);
	  }
	  directorios[i] = NULL;
	  return directorios;
}

void mostrarMetadataTodasTablas(char *ruta){
	char ** directorios;
	int i = 0;

	directorios = buscarDirectorios(ruta);

	while (directorios[i] != NULL) {

		mostrarMetadataTabla2(directorios[i]);

		i++;

	}
	liberarPunteroDePunterosAChar(directorios);
	free(directorios);
}

void obtenerRegistrosDeTabla(char* nombreTabla){
	int i,j=0;
	for(i=0;i<list_size(memtable);i++){
		t_tabla_memtable *tabla = list_get(memtable,i);
		if(strcmp(tabla->tabla,nombreTabla) ==0){
			int tamanio = list_size(tabla->registros);
			printf("Registros de %s\n\n", tabla->tabla);
			while(tamanio != j){

				t_registro* reg = list_get(tabla->registros,j);

				printf("%f;%d;%s", reg->timestamp, reg->key, reg->value);
				j++;
			}
		}
	}
}

void insertarKey(char* nombreTabla, char* key, char* value, double timestamp){
	int i;
	int clave  = atoi(key);

	for(i=0;i<list_size(memtable);i++){
		t_tabla_memtable *tabla = list_get(memtable,i);
		if(strcmp(tabla->tabla,nombreTabla) ==0){

				t_registro* registro = malloc(sizeof(t_registro));
				registro->timestamp = timestamp;
				registro->key = clave;
				registro->value = value;

				list_add(tabla->registros, registro);
				printf("Registro %f;%d;%s insertado correctamente en memtable, %s\n\n",timestamp, registro->key, registro->value, nombreTabla);

				break;
		}
	}
}

double getCurrentTime(){
	struct timeval tv;
	gettimeofday(&tv, NULL);
	unsigned long long result = (((unsigned long long)tv.tv_sec)*1000 + ((unsigned long long )tv.tv_usec)/1000);
	double res = result;
	return res;
}


/*void crearArchivosTemporales(){
	char**directorios;
	int i =0, j=0;
	directorios = buscarDirectorios(rutas.Tablas);
	while (directorios [i] != NULL){
		char*rutaArch = malloc(150);
		char** palabras = string_split(directorios[i], "/");
		char* nombTabla = obtenerUltimoElementoDeUnSplit(palabras);

	}*/

char * obtenerNombreDeArchivoDeUnaRuta(char * ruta){
	char * archivoConExtension;
	char ** split = string_split(ruta, "/");
	archivoConExtension = obtenerUltimoElementoDeUnSplit(split);
	liberarPunteroDePunterosAChar(split);
	free(split);
	if(strstr(archivoConExtension, ".") != NULL){
		return archivoConExtension;
	}
	return NULL;

}



