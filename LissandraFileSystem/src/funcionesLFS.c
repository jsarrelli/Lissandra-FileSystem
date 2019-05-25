/*
 * funcionesLFS.c
 *
 *  Created on: 23 abr. 2019
 *      Author: utnso
 */

#include "funcionesLFS.h"



double getCurrentTime(){
	struct timeval tv;
	gettimeofday(&tv, NULL);
	unsigned long long result = (((unsigned long long)tv.tv_sec)*1000 + ((unsigned long long )tv.tv_usec)/1000);
	double res = result;
	return res;
}

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
				closedir(tablaActual);
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
	log_info(logger, "Se creo la  %s en LFS\n", nombreTabla);

	while (i < cantPart){
		string_append(&rutaTabla,"/");
		string_append_with_format(&rutaTabla, "%d.bin", i);
		crearArchReservarBloqueYEscribirBitmap(rutaTabla);

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
	//FILE* archivo = fopen ("/home/utnso/tp-2019-1c-Los-Sisoperadores/LissandraFileSystem/FS_LISSANDRA/archivoRegistros.txt", "w+");
	//fclose(archivo);
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



void insertarKey(char* nombreTabla, char* key, char* value, double timestamp){
	int i;
	int clave  = atoi(key);

	for(i=0;i<list_size(memtable);i++){
		t_tabla_memtable *tabla = list_get(memtable,i);
		if(strcmp(tabla->tabla,nombreTabla) ==0){

				t_registro* registro = malloc(sizeof(t_registro));
				registro->timestamp = timestamp;
				registro->key = clave;
				strcpy(registro->value,value);

				list_add(tabla->registros, registro);
				printf("Registro %f;%d;%s insertado correctamente en memtable, %s\n\n",timestamp, registro->key, registro->value, nombreTabla);

				break;
		}
	}
}


void crearArchivosTemporales(char*ruta){
	char ** directorios;
		int i = 0;

		directorios = buscarDirectorios(ruta);

		while (directorios[i] != NULL) {

			crearTemporal(directorios[i]);

			i++;

		}
		liberarPunteroDePunterosAChar(directorios);
		free(directorios);
}

void crearTemporal(char*nombreTabla){

		char* nombTabla=malloc(50);
		//char** registros;
		//char*archivoTxt= malloc(100);
		char**palabras = string_split(nombreTabla, "/");
		nombTabla = obtenerUltimoElementoDeUnSplit(palabras);


	/*	char* rutaBloque = malloc(150);
		strcpy(rutaBloque, rutas.Bloques);
		int i=0, j=0;
		char*registro=malloc(100);
		string_append(&nombreTabla, "/");
		string_append_with_format(&nombreTabla, "%sRegistros.txt", nombTabla);
		strcpy(archivoTxt,nombreTabla);
		FILE* archivo = fopen(archivoTxt, "w+");
		escribirEnTxt(nombTabla, archivo);
		int tamanioTxt = tamanioArchivo(archivo);
		nombreTabla = obtenerRutaTablaSinArchivo(nombreTabla);*/
		string_append(&nombreTabla, "/");
		string_append_with_format(&nombreTabla, "%stmp0.tmp", nombTabla);
		crearArchReservarBloqueYEscribirBitmap(nombreTabla);
		printf("Archivo temporal creado en %s\n\n", nombTabla);
/*		t_archivo* arch = malloc(sizeof(t_archivo));
		int res = leerArchivoDeTabla(nombreTabla, arch);
		string_append(&rutaBloque, arch->BLOQUES[i]);
		string_append(&rutaBloque, ".bin");
		FILE*archBloque = fopen(rutaBloque, "w+");

		registros = buscarRegistrosDeTabla(nombTabla);
		int bytesAEscribir = obtenerTamanioArrayRegistros(registros);
		int tamanioArchBloque =0;
		while(bytesAEscribir>=0){
			while(registros[j] != NULL){
				strcpy(registro, registros[j]);
				char** reg = string_split(registro, ";");
				fprintf(archBloque, "%s;%s;%s\n", reg[0], reg[1], reg[2]);
				tamanioArchBloque += (strlen(registro)+1);
				bytesAEscribir-= (strlen(registro) +1);
				//fwrite(registros[j], strlen(registros[j]), 1, archBloque);
				liberarPunteroDePunterosAChar(reg);
				free(reg);
				j++;
			}
		}
		fclose(archBloque);*/
		printf("Vamos a escribir en el temporal");
		escribirEnTmp(nombTabla,nombreTabla);


		//fclose(archivo);

		//free(archivoTxt);
		liberarPunteroDePunterosAChar(palabras);
		free(palabras);
		//liberarPunteroDePunterosAChar(registros);
		//free(registros);
		free(nombTabla);

}



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

void crearArchReservarBloqueYEscribirBitmap(char* rutaArch){
	FILE*archivo = fopen(rutaArch, "w");
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

}
char** buscarRegistrosDeTabla(char*nombreTabla){
	char** registros = malloc(500);
	t_registro* reg;
	t_tabla_memtable* tabla;
	int i,j;
	char* registro=malloc(100);


	for (i=0;i<list_size(memtable); i++){
			tabla= (t_tabla_memtable*)  list_get (memtable,i);
			if(!strcmp(tabla->tabla, nombreTabla)){
			for (j=0;j<list_size(tabla->registros); j++){
				reg = (t_registro*) list_get(tabla->registros, j);

				sprintf(registro, "%f;%d;%s", reg->timestamp, reg->key, reg->value);

				registros[j] = string_duplicate(registro);

			}
			}
	}
	registros[j] = NULL;
	free(registro);
	return registros;


}
int obtenerTamanioArrayRegistros(char** registros){
	int tamanio=0,i=0,j=0;

	while(registros[i] != NULL){
		tamanio += strlen(registros[i]);
		i++;
		j++;
	}
	return tamanio +j;
}


int escribirEnTmp (char*nombreTabla,char*rutaTmp){
	puts("entre a escribir temporal");

	int*bloques;
	char *rutaBloque = malloc(150);
	char **arrBloques = malloc(40);
	t_archivo *archivo = malloc(sizeof(t_archivo));
	strcpy(rutaBloque,rutas.Bloques);
	char**registros = buscarRegistrosDeTabla(nombreTabla);
	int res = leerArchivoDeTabla(rutaTmp, archivo);
	if(res<0){ return -1;}
	int bytesAEscribir = obtenerTamanioArrayRegistros(registros);
	int bloquesNecesarios = (bytesAEscribir + metadata.BLOCK_SIZE - 1) / metadata.BLOCK_SIZE; // redondeo para arriba
	int bloquesReservados = archivo->cantBloques;
	int i,j;

	if(bloquesNecesarios>bloquesReservados){
			int cantBloques = bloquesNecesarios-bloquesReservados;
			bloques = buscarBloquesLibres(cantBloques);
			if(bloques==NULL){
				puts("Error al guardar datos en el archivo. No hay suficientes bloques libres");
				return -1;
			}


			for (i = 0; i < archivo->cantBloques ; ++i) {
				arrBloques[i] = archivo->BLOQUES[i];
			}

			for (j = 0; j < cantBloques; ++j) {
				reservarBloque(bloques[j]);
				arrBloques[i] = string_itoa(bloques[j]);
				i++;
			}

			archivo->BLOQUES=arrBloques;
			archivo->cantBloques = bloquesNecesarios;
			free(bloques);
			free(arrBloques);
	}

	FILE*archivoBloque;
	i=0, j=0;

		while(archivo->BLOQUES[i] != NULL){
			int tamanioArchBloque =0;

			string_append(&rutaBloque,archivo->BLOQUES[i]);
			string_append(&rutaBloque,".bin");
			archivoBloque = fopen(rutaBloque,"wb");
			if(archivoBloque == NULL){
			puts("Error al guardar datos en bloques");
			return -1;
			}
			while(tamanioArchBloque + sizeof(registros[j]) < metadata.BLOCK_SIZE){
			char registro[strlen(registros[j]) +1];
			strcpy(registro, registros[j]);
			fwrite(registro,1,sizeof(registro),archivoBloque);
			tamanioArchBloque += (sizeof(registro));
			bytesAEscribir-= (sizeof(registro));
			if(bytesAEscribir==0){
				fclose(archivoBloque);
				archivo->TAMANIO+= tamanioArchBloque;
				escribirArchivo(rutaTmp, archivo);
				escribirBitmap();
				free(archivo->BLOQUES);
				free(archivo);
				free(rutaBloque);
				return 1;
			}

			j++;
			}
			fclose(archivoBloque);
			archivo->TAMANIO += tamanioArchBloque;
			rutaBloque = obtenerRutaTablaSinArchivo(rutaBloque);
			i++;
		}

	return 1;
}

void escribirArchivo(char*rutaArchivo, t_archivo *archivo){

	FILE* arch = fopen(rutaArchivo, "w+");
	fprintf(arch, "TAMANIO=%i\n", archivo->TAMANIO);
	fprintf(arch, "BLOQUES=[");
	int i;
	for (i = 0; i < archivo->cantBloques ; ++i) {
		if(i!=0){
			fprintf(arch, ",");
		}
		fprintf(arch, "%s", archivo->BLOQUES[i]);
	}
	fprintf(arch, "]");
	fclose(arch);
}




/*
int guardarDatosEnArchivo(char *path, int offset, int size, void* buffer)
{
	char** directorios = string_split("/home/utnso/tp-2019-1c-Los-Sisoperadores/LissandraFileSystem/FS_LISSANDRA/archivoRegistros.txt", "\n");
	t_archivo *archivo = newArchivo();
	int res = leerArchivo(path, archivo);
	if(res<0){
		return -1;
	}
	//Calculo bloques necesarios y los reservo
	int tamanio = tamanioArchivo("/home/utnso/tp-2019-1c-Los-Sisoperadores/LissandraFileSystem/FS_LISSANDRA/archivoRegistros.txt");
	int bloquesNecesarios = (tamanio + metadata.BLOCK_SIZE - 1) / metadata.BLOCK_SIZE; // redondeo para arriba
	int bloquesReservados = archivo->cantBloques;
	if(bloquesNecesarios>bloquesReservados){
		int cantBloques = bloquesNecesarios-bloquesReservados;
		int *bloques = buscarBloquesLibres(cantBloques);
		if(bloques==NULL){
			puts("Error al guardar datos en el archivo. No hay suficientes bloques libres");
			return -1;
		}
		int i;
		char **arrBloques = (char**) malloc(sizeof(char*)*bloquesNecesarios);
		for (i = 0; i < archivo->cantBloques ; ++i) {
			arrBloques[i] = archivo->BLOQUES[i];
		}
		int j;
		for (j = 0; j < cantBloques; ++j) {
			reservarBloque(bloques[j]);
			arrBloques[i] = string_itoa(bloques[j]);
			i++;
		}
		free(archivo->BLOQUES);
		free(bloques);
		archivo->BLOQUES=arrBloques;
		archivo->cantBloques = bloquesNecesarios;
	}
	//Escribir datos en bloques
	int bloqueActual = archivo->BLOQUES[0]; //Bloque inicial
	int offsetBloque;
	int bytesAEscribir = tamanio;
	while(bytesAEscribir){
		offsetBloque = offset - bloqueActual * metadata.BLOCK_SIZE;
		char *pat = string_new();
		string_append(&pat,rutas.Bloques);
		string_append(&pat,archivo->BLOQUES[bloqueActual]);
		string_append(&pat,".bin");
		FILE* archBloque = fopen(pat,"rb+");
		if(archBloque == NULL){
			puts("Error al guardar datos en bloques");
			return -1;
		}
		fseek(archBloque,offsetBloque,SEEK_SET);
		int bytesLibres = metadata.BLOCK_SIZE - offsetBloque;
		int cant = (bytesAEscribir<=bytesLibres)? bytesAEscribir : bytesLibres;
		fwrite(buffer+size-bytesAEscribir,1,cant,archBloque);
		fclose(archBloque);
		free(pat);
		bytesAEscribir-=cant;
		offset+=cant;
		bloqueActual++;
	}
	//Escribo nuevo tamaño
	archivo->TAMANIO = (tamanio>archivo->TAMANIO)? tamanio : archivo->TAMANIO;
	//Escribo archivos de metadata
	escribirArchivo(path, archivo);
	escribirBitmap();
	free(archivo->BLOQUES);
	free(archivo);
	return 1;
}
*/

/*int escribirEnTmp(char* rutaTmp, char* rutaTxt, int tamanioTxt){
	FILE*arch = fopen(rutaTmp,"w+" );
	int i,j,k=0;
	char** directorios = string_split(rutaTxt, "\n");
	//int cantReg = contarPunteroDePunteros(directorios);
	int bytesAEscribir = tamanioTxt;
	int tamanioMaxBloque = metadata.BLOCK_SIZE;
	t_archivo *archivo = malloc(sizeof(t_archivo));
	int res = leerArchivoDeTabla(rutaTmp, archivo);
	if(res<0){ return -1;}
	int bloquesNecesarios = (tamanioTxt + metadata.BLOCK_SIZE - 1) / metadata.BLOCK_SIZE; // redondeo para arriba
	int bloquesReservados = archivo->cantBloques;
	if(bloquesNecesarios>bloquesReservados){
			int cantBloques = bloquesNecesarios-bloquesReservados;
			int *bloques = buscarBloquesLibres(cantBloques);
			if(bloques==NULL){
				puts("Error al guardar datos en el archivo. No hay suficientes bloques libres");
				return -1;
			}

			char **arrBloques = (char**) malloc(sizeof(char*)*bloquesNecesarios);
			for (i = 0; i < archivo->cantBloques ; ++i) {
				arrBloques[i] = archivo->BLOQUES[i];
			}

			for (j = 0; j < cantBloques; ++j) {
				reservarBloque(bloques[j]);
				arrBloques[i] = string_itoa(bloques[j]);
				i++;
			}
			free(archivo->BLOQUES);
			free(bloques);
			archivo->BLOQUES=arrBloques;
			archivo->cantBloques = bloquesNecesarios;
	}
	i=0;

	while(archivo->BLOQUES[i] != NULL){
		int tamanioArchBloque =0;
		char *rutaBloque = string_new();
		string_append(&rutaBloque,rutas.Bloques);
		string_append(&rutaBloque,archivo->BLOQUES[i]);
		string_append(&rutaBloque,".bin");
		FILE* archBloque = fopen(rutaBloque,"w+");
		if(archBloque == NULL){
		puts("Error al guardar datos en bloques");
		return -1;
		}
		while(bytesAEscribir){
			if(tamanioArchBloque < tamanioMaxBloque){
					t_registro * reg = malloc(sizeof(t_registro));
					char ** directorio = string_split(directorios[k],";");
					reg->timestamp = atof(directorio[0]);
					reg->key = directorio[1];
					strcpy(reg->value, directorio[2]);
					fprintf(archBloque, "%f;%d;%s\n", reg->timestamp, reg->key, reg->value);
					tamanioArchBloque += tamanioArchivo(archBloque);
					bytesAEscribir-= tamanioArchivo(archBloque);

					free(reg);
					liberarPunteroDePunterosAChar(directorio);
					free(directorio);
					k++;
			} else{
				archivo->TAMANIO += tamanioArchivo(archBloque);
				fclose(archBloque);
				break;
			}
		}

	i++;
	}
	escribirArchivo(arch, archivo);
	liberarPunteroDePunterosAChar(directorios);
	free(directorios);
	free(archivo);


	for(k=0; k < cantReg; k++){
			t_registro * reg = malloc(sizeof(t_registro));
			char ** directorio = string_split(directorios[k],";");
			reg->timestamp = directorio[0];
			strcpy(reg->value, directorio[2]);
			list_add(listaReg, reg);
	}
	return 1;
}

void escribirArchivo(FILE* arch, t_archivo *archivo){


	fprintf(arch, "TAMANIO=%i\n", archivo->TAMANIO);
	fprintf(arch, "BLOQUES=[");
	int i;
	for (i = 0; i < archivo->cantBloques ; ++i) {
		if(i!=0){
			fprintf(arch, ",");
		}
		fprintf(arch, "%s", archivo->BLOQUES[i]);
	}
	fprintf(arch, "]");
	fclose(arch);
}*/



