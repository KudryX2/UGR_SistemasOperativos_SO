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


#define OPEN_ERROR "Error al abrir o crear archivo"
#define WRITE_ERROR "Error al escribir en un archivo"

int main(int argc, char** argv){

	int cauce1[2];
	int cauce2[2];

	if(pipe(cauce1)){					// Creamos cauce
		perror(PIPE_ERROR);
	}

	int pid = fork();					// Creamos proceso hijo
	if(pid < 0){
		perror(FORK_ERROR);
	}


/*
	Proceso hijo , ejecuta el programa "quijote_tam" , escribe cauce 1
*/
	if(pid == 0){

		close(cauce1[0]); 				// Cierra lectura
		dup2(cauce1[1], STDOUT_FILENO);	// Escribe cauce 1

		if((execl("quijote_tam", "arg0", NULL)) < 0 ){
			perror(EXEC_ERROR);
		}
	}


/*
	Proceso padre, lee cauce 1
*/
	close(cauce1[1]); 					// Cierra escritura
	dup2(cauce1[0], STDIN_FILENO);		// Lee cauce 1

/*
	char buffer[100];
	while ((read(STDIN_FILENO,buffer,sizeof(buffer))) > 0){	// Lee tamaño del quijote
		printf("%s", buffer);
	}
*/
	char buffer[100];
	read(STDIN_FILENO,buffer,sizeof(buffer));

	int status;
	wait(&status);						// Espera la finalizacion del hijo


	int fd;
	if((fd = open("resultado.txt", O_WRONLY | O_CREAT | O_TRUNC , S_IRWXU)) < 0){	// Creamos/sobreescribimos fichero resultados
		perror(OPEN_ERROR);
	}
	write(fd, buffer, strlen(buffer));	// Escribimos el tamaño en "resultados.txt"


/*
	Hijo 2 , ejecuta "wc -w quijote.txt", escribe cauce 2
*/
	if(pipe(cauce2) == -1){
		perror(PIPE_ERROR);
	}

	int pid2 = fork();
	if(pid2 < 0){
		perror(FORK_ERROR);
	}


	if(pid2 == 0){

		close(cauce1[0]);					// Cierra cauce 1
		close(cauce1[1]);

		close(cauce2[0]);					// Cierra lectura
		dup2(cauce2[1], STDOUT_FILENO); 	// Escribe cauce 2

		if((execlp("wc", "arg0","-w", "quijote.txt", NULL)) == -1){
			perror(EXEC_ERROR);
		}
	}

	close(cauce1[0]);						// Cierra cauce 1
	close(cauce1[1]);

	close(cauce2[1]);						// Cierra escritura
	dup2(cauce2[0], STDIN_FILENO);			// Lee cauce 2


	char buffer2[100];
	read(STDIN_FILENO,buffer2,6);			// Lee del cauce 2 6 bytes (resultado numerico )


	char *resultado = malloc(sizeof(char*));
	sprintf(resultado,"\n\nPalabras: %s\n", buffer2);	// Añade dos espacios delante y modifica el resultado 


	int fd2;
	if((fd2 = open("resultado.txt", O_WRONLY | O_APPEND, S_IRWXU)) < 0){	// Creamos/sobreescribimos fichero resultados
		perror(OPEN_ERROR);
	}
	write(fd2, resultado, strlen(resultado));	// Escribimos el tamaño en "resultados.txt"


	int status2;
	while((pid = wait(&status2)) > 0);		// Espera hijo

}