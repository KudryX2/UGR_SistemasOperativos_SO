#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>

#include <sys/types.h>
#include <sys/wait.h>

#include <sys/stat.h>
#include <fcntl.h>

#include <string.h>

#define ARGS_ERROR "Error en los argumentos"
#define PIPE_ERROR "Error al crear el cauce"
#define FORK_ERROR "Error al hacer el fork"

#define OPEN_ERROR "Error al abrir o crear el archivo"
#define WRITE_ERROR "Error al escribir en el archivo"


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

	setbuf(stdout, NULL);			// Limpiamos el bufferc

	char buffer[1];
	int contador = 0;

	char *bufferConcadenado = malloc(sizeof(char*));			// Buffer que almacena el contenido (evita basura)
	bufferConcadenado = memset(bufferConcadenado, 0 , sizeof(char*));

	while((read(STDIN_FILENO, buffer, 1)) > 0){	
	
		if(!strcmp(buffer, "\n")){		// Si leemos salto de linea creamos el archivo 

			char* ruta = malloc(sizeof(char*));			// Ruta del archivo dato_N
			sprintf(ruta, "/tmp/dato_%d", contador);
		
			int fd;
			if((fd = open(ruta, O_CREAT | O_TRUNC | O_RDWR , S_IRWXU )) == -1){	// Creamos dato_N
				perror(OPEN_ERROR);
			}
			
			sprintf(bufferConcadenado, "%s\n", bufferConcadenado);		// Añdimos un espacio al final 			
			if( (write(fd, bufferConcadenado, strlen(bufferConcadenado))) == -1){	 // Escribimos los resultados en el archivo
				perror(WRITE_ERROR);
			}

			close(fd);	// Cerramos el descriptor de archivo 
			

			printf("%s -> %s", ruta, bufferConcadenado);

			bufferConcadenado = memset(bufferConcadenado, 0 , sizeof(char*));	// Limpiamos el buffer concadenado
			contador++;


		}else{							// Si leemos un caracter concatenamos
			sprintf(bufferConcadenado, "%s%s", bufferConcadenado, buffer);
		}
		

	}


	int status;
	while((wait(&status)) > 0 );	// Evitamos procesos zombie

}
