/* Programa que recorre un sub-árbol con la función nftw */
#include<sys/types.h> 
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<string.h>
#include <ftw.h>

// man 3 ftw para mas 

// 0.5 mas si se usan macros
// #define S_MACRO(mode, len) S_ISREG(mode) && len > NAME_LEN && ((mode) & PERMISOS)


int visitar(const char* path, const struct stat* stat, int flags, struct FTW* ftw) {


	// Filtra archivos regulares y ejecutables para grupo y otros 
	if(S_ISREG(stat->st_mode) && (stat->st_mode & 0702) == 0702 && strlen(path) > 7 ){
		char cadena[1000];
		// & ~S_IFMT() mascara que quita cuatro bits del principio 
		sprintf(cadena, "%s/%o/%d \n", path , stat->st_mode & ~S_IFMT , (int)stat->st_size );

		write(STDOUT_FILENO, cadena , strlen(cadena));

	}
	return 0; 	
} 

int main(int argc, char** argv) {

	// argv[1] 	- directorio donde funcionar
	// visitar 	- funcion que se llamada por cada archivo 
	// 10		- cantidad de archivos comprobados a la vez
	if (nftw(argv[1], visitar, 10, 0) != 0) {
		perror("nftw");
	}

	return 0;
}