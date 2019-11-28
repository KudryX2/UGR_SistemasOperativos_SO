#include<sys/types.h> 
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<string.h>
#include <ftw.h>

/*
	man 2 open | grep S_I
*/
#define CHECK_PERMISSIONS (atributos->st_mode & S_IRWXU) && (atributos->st_mode & S_IWOTH) && !(atributos->st_mode & S_IRWXG) ? 1 : 0


int visitar(const char* path, const struct stat *atributos, int flags, struct FTW* ftw) {

	if(CHECK_PERMISSIONS){
		char salida[1000];
		sprintf(salida, "%s | %o | %d \n", path , atributos->st_mode & ~S_IFMT , (int)atributos->st_size );

		printf(salida);
	}

	return 0;
}


int main(int argc , char** argv){
	// argv[1] 	- directorio donde funcionar
	// visitar 	- funcion que se llamada por cada archivo 
	// 10		- cantidad de archivos comprobados a la vez
	if (nftw(argv[1], visitar, 10, 0) != 0) {
		perror("nftw");
	}

	return 0;
}