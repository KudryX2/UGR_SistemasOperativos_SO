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
#define ARG_ERROR "Falta el argumento\n"


int main(int argc, char** argv){

	if(argc != 2){
		printf(ARG_ERROR);
		exit(-1);
	}

	int N = atoi(argv[1]);
	char* parametro = malloc(sizeof (char*));
	sprintf(parametro, "%s%d", "-", N);

	int cauce1[2];	// Cauce encargado de manejar "ls -l" 
	int cauce2[2];	// Cauce encargado de manejar "tail -N"
	int cauce3[2];	// Cauce encargado de manejar "cut 0 10"


/*
	Primer hijo , hace "ls -l"  , escribe en cauce 1 
*/
	if(pipe(cauce1) == -1){				// Crea cauce1
		perror(PIPE_ERROR);
	}  

	int pid = fork();					// Crea hijo 1
	if(pid < 0){
		perror(FORK_ERROR);
	}

	if(pid == 0){	
		close(cauce1[0]);				// Cierra lectura cauce 1
		dup2(cauce1[1], STDOUT_FILENO); // Escribe cauce 1

		if((execlp("ls", "ls", "-l", NULL)) < 0){
			perror(EXEC_ERROR);
		}
		
	}


/*
	Segundo hijo , hace "tail -N" , lee del cauce 1 y escribe en cauce 2 
*/
	if(pipe(cauce2) == -1){				// Crea cauce 1
		perror(PIPE_ERROR);
	}   

	int pid2 = fork();					// Crea hijo 2
	if(pid2 < 0){
		printf(FORK_ERROR);
	}

	if(pid2 == 0){
		close(cauce1[1]);				// Cierra escritura cauce 1
		dup2(cauce1[0], STDIN_FILENO);	// Lee cauce 1
		
		close(cauce2[0]);				// Cierra lectura cauce 2
		dup2(cauce2[1], STDOUT_FILENO);	// Escribe cauce 2
			
		if((execlp("tail", "tail", parametro, NULL)) < 0){
			perror(EXEC_ERROR);
		}
	}


/*
	Tercer hijo , hace "cut -b 0-10" , lee del cauce 2 y escribe en cauce 3
*/

	if(pipe(cauce3) == -1){				// Crea cauce 3
		perror(PIPE_ERROR);
	}

	int pid3 = fork();					// Crea hijo 3
	if(pid3 < 0){
		printf(FORK_ERROR);
	}

	if(pid3 == 0){
		close(cauce1[0]);				// Cierra lectura cauce 1
		close(cauce1[1]);				// Cierra escritura cauce 1

		close(cauce2[1]);				// Cierra escritura cauce 2
		dup2(cauce2[0], STDIN_FILENO);	// Lee cauce 2

		close(cauce3[0]);				// Cierra escritura cauce 3
		dup2(cauce3[1], STDOUT_FILENO);	// Escribe cauce 3

		if((execlp("cut", "cut", "-b", "1-10", NULL)) < 0){
			perror(EXEC_ERROR);
		}
	}


/*
	Padre , lee del cauce 2
*/
	close(cauce1[0]);					// Cierra cauce 1	
	close(cauce1[1]);		
	
	close(cauce2[0]);					// Cierra cauce 2
	close(cauce2[1]);

	close(cauce3[1]);					// Cierra escritura cauce 3
	dup2(cauce3[0], STDIN_FILENO);		// Lee cauce 3


	char buffer[100];
	while ((read(STDIN_FILENO,buffer,sizeof(buffer))) > 0){
		printf("%s", buffer);
	}


	// Esperamos a los procesos hijos 
	int status;
	while ((pid = wait(&status)) > 0);

}
