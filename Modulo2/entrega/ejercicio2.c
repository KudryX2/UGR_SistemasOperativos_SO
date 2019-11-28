#include<sys/types.h> 
#include<sys/stat.h>
#include<sys/wait.h>
#include<fcntl.h>
#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<string.h>

#define FORK_ERROR "Error al hacer fork"
#define EXEC_ERROR "Error al hacer execl"
#define PIPE_ERROR "Error al crear cauce"
#define READ_ERROR "Error al leer del cauce"


int main(){

	int cauce1[2];
	int cauce2[2];

/*
	Primer hijo , hace "ls -l", escribe en el primer cauce
*/
	if(pipe(cauce1) == -1){
		perror(PIPE_ERROR);
	}

	int pid1 = fork();	
	if(pid1 < 0){
		perror(FORK_ERROR);
	}

	if(pid1 == 0){

		close(cauce1[0]);									// Cierra lectura cauce 1
		dup2(cauce1[1], STDOUT_FILENO);						// Escribe cauce 1

		if((execlp("ls", "arg0", "-l", NULL)) < 0){
			perror(EXEC_ERROR);
		}
	}


/*
	Segundo hijo , hace "wc -l", lee primer cauce , escribe segundo cauce
*/
	if(pipe(cauce2) == -1){
		perror(PIPE_ERROR);
	}

	int pid2 = fork();
	if(pid2 < 0){
		perror(FORK_ERROR);
	}

	if(pid2 == 0){

		close(cauce1[1]);									// Cierra escritura cauce 1
		dup2(cauce1[0], STDIN_FILENO);						// Lee cauce 2

		close(cauce2[0]);									// Cierra lectura cauce 2
		dup2(cauce2[1], STDOUT_FILENO);						// Escribe cauce 2 	

		if((execlp("wc","arg0", "-l", NULL)) < 0){
			perror(EXEC_ERROR);
		}
	}


/*
	Padre , lee del cauce 2 
*/
	close(cauce1[0]);										// Cierra cauce 1
	close(cauce1[1]);

	close(cauce2[1]);										// Cierra escritura cauce 2
	dup2(cauce2[0], STDIN_FILENO);							// Lee cauce 2



	char buffer[200];
	if((read(STDIN_FILENO,buffer,sizeof(buffer))) < 0){
		perror(READ_ERROR);
	}

	int numero = atoi(buffer);
	printf("Numero de lineas: %d , es %s  \n", numero, numero%2 ? "impar" : "par");

	// Esperamos a los procesos hijos
	pid_t pid;
	int status;
	while((pid = wait(&status)) > 0);

}