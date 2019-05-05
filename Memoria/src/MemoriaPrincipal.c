#include "MemoriaPrincipal.h"

void inicializarEstadoMemoria() {
	estadoMemoria=list_create();
	for (int i = 0; i < cantPaginas - 1; i++) {
		list_add() = LIBRE;
	}
}

void inicializarMemoria(int valueMaximoRecibido){
	cantPaginas=tamanioMemoria/tamanioPagina;
	memoria=malloc(tamanioMemoria);
	valueMaximo=valueMaximoRecibido;
	segmentos=list_create();
	inicializarEstadoMemoria();
}

Segmento insertarSegmento(char* nombreSegmento){
	Segmento segmento={nombreSegmento, "", list_create(),0};
	list_add(segmentos,segmento);
	return segmento;

}

//Pagina insertarPagina(int key, char* value){
//	Pagina pagina=NULL;
//		if(!existePagina()&& memoriaLlena()){
//
//		}
//	return pagina;
//}


Pagina buscarPagina(Segmento segmento,int key){
	if(existeSegmento(segmento)){
		for(int i=0;i<list_size(segmento.paginas)-1;i++){
			Pagina paginaActual=(Pagina) list_get(segmento.paginas,i);
			if(paginaActual.registro->key==key){
				return paginaActual;
			}
		}
	}
	return NULL;
}

Segmento buscarSegmento(char* nombreSegmento){
	for(int i=0;i<list_size(segmentos);i++){
		Segmento segmentoActual=(Segmento)list_get(segmentos,i);
		if(strcmp(segmentoActual.nombreTabla,nombreSegmento)){
			return segmentoActual;
		}
	}
	return NULL;
}

bool existeSegmento(Segmento segmento){
	return buscarSegmento(segmento.nombreTabla)!=NULL;
}

bool existePagina(Segmento segmento,int key){
	return buscarPagina(segmento,key)!=NULL;
}

bool memoriaLlena(){
	t_list espaciosDisponibles=list_filter(estadoMemoria,estaLibre);
	return list_size(espaciosDisponibles)>0;
}

bool estaLibre(int estado){
	return estado==LIBRE;
}

