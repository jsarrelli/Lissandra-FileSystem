#include "MemoriaPrincipal.h"

void inicializarEstadoMemoria() {
	estadoMemoria=list_create();
	for (int i = 0; i < cantPaginas - 1; i++) {
		list_add(estadoMemoria,LIBRE);
		//consultar
	}
}

void inicializarMemoria(int valueMaximoRecibido){
	cantPaginas=tamanioMemoria/tamanioPagina;
	memoria=malloc(tamanioMemoria);
	valueMaximo=valueMaximoRecibido;
	segmentos=list_create();
	inicializarEstadoMemoria();
}

void insertarSegmento(char* nombreSegmento){

	Segmento* segmento=malloc(sizeof(Segmento));
	strcpy(segmento->nombreTabla,nombreSegmento);
	segmento->nombreModificado=1;
	segmento->paginas=list_create();
	list_add(segmentos,segmento);
	free(segmento);


}

//inserta una pagina en la memoria y te devuelve la direccion de
//donde la puso
Pagina* insertarPagina(int key, char* value, char* nombreSegmento){
	Segmento* segmento=buscarSegmento(nombreSegmento);
	Pagina* paginaNueva=NULL;
		if(existeSegmento(nombreSegmento) && !existePagina(segmento,key) && !memoriaLlena()){
			void* marcoVacio = darMarcoVacio();

			t_registro registro={.key=key, .value=value};
			t_registro* registroEnMemoria=memcpy(marcoVacio,&registro,sizeof(t_registro));

			paginaNueva = malloc(sizeof(Pagina));
			paginaNueva->modificado=0;
			paginaNueva->registro=registroEnMemoria;

			list_add(segmento->paginas,paginaNueva);
		}
	return paginaNueva;
}

void* darMarcoVacio(){
	for(int i=0; i<list_size(estadoMemoria);i++){
		if((int)list_get(estadoMemoria,i) == LIBRE){
			return memoria+i*sizeof(t_registro);
		}
	}
	return NULL;
}


Pagina* buscarPagina(Segmento* segmento,int key){
	if(existeSegmento(segmento->nombreTabla)){
		for(int i=0;i<list_size(segmento->paginas)-1;i++){
			Pagina* paginaActual=(Pagina*) list_get(segmento->paginas,i);
			if(paginaActual->registro->key==key){
				return paginaActual;
			}
		}
	}
	return NULL;
}

Segmento* buscarSegmento(char* nombreSegmento){
	for(int i=0;i<list_size(segmentos);i++){
		Segmento* segmentoActual=(Segmento*)list_get(segmentos,i);
		if(strcmp(segmentoActual->nombreTabla,nombreSegmento)){
			return segmentoActual;
		}
	}
	return NULL;
}

bool existeSegmento(char* nombreSegmento){
	return buscarSegmento(nombreSegmento)!=NULL;
}

bool existePagina(Segmento* segmento,int key){
	return buscarPagina(segmento,key)!=NULL;
}

bool memoriaLlena(){
	t_list *espaciosDisponibles=list_filter(estadoMemoria,LIBRE);
	return list_size(espaciosDisponibles)==0;
}

bool estaLibre(int estado){
	return estado==LIBRE;
}

