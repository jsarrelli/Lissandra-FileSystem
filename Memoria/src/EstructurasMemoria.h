#ifndef EstructurasMemoria_H_
#define EstructurasMemoria_H_

typedef struct Segmento {
	char* nombreTabla;
	t_list* paginas;
	t_metadata_tabla* metaData;
} Segmento;

typedef enum{
	NO_MODIFICADO = 0,
	MODIFICADO = 1
}t_modificado;

typedef struct Pagina {
	t_modificado modificado;
	t_registro* registro;
} Pagina;

typedef enum {
	LIBRE = 0,
	OCUPADO
} t_estado;

typedef struct EstadoFrame {
	t_estado estado;
	double fechaObtencion;
} EstadoFrame;


#endif /* EstructurasMmemoria_H_*/