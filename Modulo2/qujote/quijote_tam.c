#include<sys/types.h> 
#include<sys/stat.h>
#include<sys/wait.h>
#include<fcntl.h>
#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<string.h>


#define STAT_ERROR "Error al obtener la estructura stat"

int main(int argc, char** argv){

	

	struct stat atributos; 
	if(stat("quijote.txt", &atributos) == -1){		// Obtenemos los atributos (stat) del archivo 
		perror(STAT_ERROR);
	}

	printf("Tamaño quijote.txt : %d \n", (int)atributos.st_size);	// Escribimos en el cauce el tamaño del archivo
}