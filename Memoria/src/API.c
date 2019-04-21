#include <stdio.h>
#include <stdlib.h>


void procesarConsulta(int cantAtributos, char** consulta) {
	puts("API-Memoria inicializada");
	char* comando=consulta[1];
	if(comando=="SELECT"&&cantAtributos==4){
		//select
	}else if(comando=="INSERT"&&cantAtributos==5){
		//insert
	}else if(comando=="CREATE"&&cantAtributos==5){
		//create
	}else if(comando=="DESCRIBE"&&cantAtributos==2){
		//describe
	}else if(comando=="DROP"&&cantAtributos==2){
		//drop
	}else{
		puts("Comando no encontrado");
	}
}

