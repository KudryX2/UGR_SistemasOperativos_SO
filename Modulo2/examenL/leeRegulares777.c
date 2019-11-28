#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define STAT_ERROR "Error al hacer stat"

#define PERMISOS 0777
#define COMPROBAR(mode, tam) S_ISREG(mode) && tam < 7 && (mode & PERMISOS) == PERMISOS 

int main(){

	DIR* dir;
	dir = opendir(".");		// Abrimos directorio actual

	struct dirent *dirent;	// DIRENT
	struct stat atributos;	// STAT
		

	while(dirent = readdir(dir)){

		if((stat(dirent->d_name , &atributos)) == -1){	// Obtenemos atributos de STAT
			perror(STAT_ERROR);
		}

		if(COMPROBAR(atributos.st_mode, strlen(dirent->d_name))){	// Filtro con macro
			printf("%s", dirent->d_name);
		}


	}
		

}
