#ifndef CONEXIONES_H_
#define CONEXIONES_H_
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/sendfile.h>
#include <errno.h>
#include <netinet/in.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdlib.h>
#include <commons/string.h>
#include <commons/bitarray.h>
#include <commons/collections/list.h>
#include <readline/readline.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <stdint.h>

#define BACKLOG 5			// Define cuantas conexiones vamos a mantener pendientes al mismo tiempo
#define MAX_PACKAGE_SIZE 1024	//El servidor no admitira paquetes de mas de 1024 bytes
#define MAXCONSULTA 6
#define MAX_MENSAJE_SIZE 100

//////////////////////////////////////////
//       Funciones De Servidores        //
//////////////////////////////////////////

int ConectarAServidor(int puerto, char* ip);
int configurarSocketServidor(char* puertoEscucha);
int ConectarAServidorPlus(int puerto, char* ip);

#endif /* CONEXIONES_H_ */
