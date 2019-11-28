#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include <string.h>


#define PIPE_ERROR "Error al crear el cauce"
#define FORK_ERROR "Error al hacer fork"
#define EXEC_ERROR "Error al hacer execl"
#define READ_ERROR "Error al leer del cauce"

#define OPEN_ERROR "Error al hacer open, crear archivo "

int main(){

	int cauce[2];
	if((pipe(cauce)) == -1){
		perror(PIPE_ERROR);
	}
	
	int pid = fork();
	if(pid < 0){
		perror(FORK_ERROR);
	}

	
/*	
	Proceso hijo, ejecuta "leeRegulares777" , escribe en el cauce
*/
	if(pid == 0){
		close(cauce[0]);	 			// Cierra lectura
		dup2(cauce[1], STDOUT_FILENO);	// Escribe en el cauce		

		if((execl("leeRegulares777","arg0", NULL)) == -1){
			perror(EXEC_ERROR);
		}
	}

/*
	Proceso padre , lee del cauce , crea archivos 
*/
	close(cauce[1]);					// Cierra escritura
	dup2(cauce[0], STDIN_FILENO);		// Lee del cauce 

	setbuf(stdout, NULL); 				// Limpia buffer
	

	char* ruta = malloc(sizeof(char*));
	memset(ruta,0,sizeof(char*));

	char* nombre = malloc(sizeof(char*));
	memset(nombre,0,sizeof(char*));

	char buffer[1];	

	while ((read(STDIN_FILENO,buffer,1)) > 0){

		if(!strcmp(buffer, "\n")){	// En el salto de linea , creamos "ruta" y creamos fichero 

			sprintf(ruta, "ficheros/%s", nombre);
			printf("%s\n", ruta);

			int fd;
			if((fd = open(ruta, O_CREAT | O_TRUNC | O_WRONLY , S_IRWXU | S_IRWXG)) < 0){
				perror(OPEN_ERROR);
			}
			
			close(fd);

			memset(nombre,0,sizeof(char*));
		}else{
			sprintf(nombre, "%s%s", nombre, buffer);	// Concatenamos 
		}

	}


	int status;
	wait(&status);
}
