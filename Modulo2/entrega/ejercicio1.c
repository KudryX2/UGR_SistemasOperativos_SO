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

void esperar(){
	pid_t wpid;
	int status;
	while ((wpid = wait(&status)) > 0);
}

int main(int argc, char** argv){

	int N = atoi(argv[1]);
	char* parametro = malloc(sizeof (char*));
	sprintf(parametro, "%s%d", "-", N);


	int cauce1[2]; 
	if(pipe(cauce1) == -1){
		perror(PIPE_ERROR);
	}  

	dup2(cauce1[0], STDIN_FILENO);		
	close(cauce1[1]);

	int pid = fork();
	if(pid < 0){
		perror(FORK_ERROR);
	}


	if(pid == 0){	// Primer hijo
		close(cauce1[0]);
		dup2(cauce1[1], STDOUT_FILENO);

		if((execlp("ls", "argv0", "-l", NULL)) < 0){
			perror(EXEC_ERROR);
		}
		
	}
	
	esperar();
	
	int pid2 = fork();
	if(pid2 < 0){
		printf(FORK_ERROR);
	}

	if(pid2 == 0){	// Segundo hijo 
		close(cauce1[1]);
		dup2(cauce1[0], STDIN_FILENO);
			
		if((execlp("tail", "argv0", parametro, NULL)) < 0){
			perror(EXEC_ERROR);
		}
	}


	esperar();



}
