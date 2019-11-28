#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>

#include <sys/types.h>
#include <sys/wait.h>


#define ARGS_ERROR "Error en los argumentos"
#define PIPE_ERROR "Error al crear el cauce"
#define FORK_ERROR "Error al hacer el fork"


int main (int argc , char** argv){

	if(argc < 2){					// Comprobamos que se hayan pasado argumentos
		perror(ARGS_ERROR);
	}

	int cauce[2];			
	if((pipe(cauce)) == -1){		// Creamos el cauce
		perror(PIPE_ERROR);
	}
	

	for(int i = 1 ; i < argc ; ++i){		
	//	printf("%s\n", *(argv+i));
	
		int pid = fork();			
		if(pid < 0){
			perror(FORK_ERROR);
		}

/*
	Proceso hijo , ejecuta "du -k argv[i]" , escribe en el cauce 1
*/
		if(pid == 0){
			close(cauce[0]);				// Cierra lectura
			dup2(cauce[1], STDOUT_FILENO);	// Escribe cauce 1
		
			execlp("du", "arg0", "-k", *(argv+i), NULL);		
		}
	
	}	// End for

/*
	Proceso padre , lee del cauce 1
*/
	close(cauce[1]);				// Cierra escritura
	dup2(cauce[0], STDIN_FILENO);	// Lee del cauce 1
	
	setbuf(stdout, NULL);			// Limpiamos el buffer
		
	char buffer[100];
	while((read(STDIN_FILENO, buffer, sizeof(buffer))) > 0){	
		printf("%s", buffer);		// Imprimimos lo que han escrito los hijo
	}

	int status;
	while((wait(&status)) > 0 );	// Evitamos procesos zombie

}
