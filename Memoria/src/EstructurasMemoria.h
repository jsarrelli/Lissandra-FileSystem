#ifndef EstructurasMemoria_H_
#define EstructurasMemoria_H_

t_log* loggerInfo;
t_log* loggerError;
t_log* loggerTrace;

typedef struct Segmento {
	char* nombreTabla;
	t_list* paginas;
} Segmento;

typedef enum {
	NO_MODIFICADO = 0, MODIFICADO = 1
} t_modificado;

typedef struct t_registro_memoria {
	int key;
	double timestamp;
	char value[];
} t_registro_memoria;

typedef struct Pagina {
	t_modificado modificado;
	t_registro_memoria* registro;
} Pagina;

typedef enum {
	LIBRE = 0, OCUPADO
} t_estado;

typedef struct EstadoFrame {
	t_estado estado;
	double fechaObtencion;
} EstadoFrame;

typedef struct t_memoria {
	char* ip;
	char* puerto;
	int memoryNumber;
} t_memoria;

#endif /* EstructurasMmemoria_H_*/
