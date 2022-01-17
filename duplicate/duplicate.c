#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>

int main ( int argc, char* argv[] )
{
	struct stat sts ;
	char* ref = (char*) malloc(4096*sizeof(char));
	
	if( argc == 2 )
		strcpy( ref, argv[1] );
	else if(argc == 1)
		strcpy( ref, "Classic.bmp" );
	else{
		fprintf(stderr, "Too much argument : Usage : ./dup (absolute_path/)<[name_file].bmp>") ;
		exit(7) ;		//argument list too long
	}
		
	if( stat(argv[1], &sts) == -1 && errno == ENOENT )
		fprintf( stderr, "The file %s doesn't exist \n", argv[1] ) ;


	//lecture
	int desc = open( ref, O_RDONLY ) ;
	if(desc == -1){
		perror("open has failed");
		exit(EXIT_FAILURE) ;
	}
		
	char buffer; //short buffer1, int buffer2 ;
	int desc2 , cpt=0 , bmp_ext =0;
	char* token = strtok(ref, "/") ;
	while( token != NULL ){
		strcpy(ref, token) ;
		token = strtok(NULL, "/") ;
	}
	char* token2 = strtok(ref, ".") ;
	strcpy(ref, token2);
	
	char* res = strcat(ref, "_copy.bmp");
	if((desc2 = open( res, O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU )) == -1){
		perror("open write file has failed");
		exit(EXIT_FAILURE);
	}
	
	while( read( desc, &buffer, 1  ) != 0 ){
		write(desc2, &buffer, 1);
		if(cpt == 0 || cpt == 1){
			if(cpt == 0 && buffer=='B')
				bmp_ext = 1;
			else if(cpt == 1 && buffer=='M' && bmp_ext)
				fprintf(stdout, "Le fichier a bien une extension en .bmp  : %s\n", ref) ;
			else if(cpt == 1 && buffer != 'M')
				fprintf( stdout, "Le fichier a une autre extension : %s\n", ref ) ;
				
		}
		cpt++;
	
	}
	
	close(desc) ;
	close (desc2);
	free(ref) ;
	return 0 ;
}

/*
OUTPUT : 
$ ./dup /home/duc13/Downloads/Classic.bmp
Le fichier a bien une extension en .bmp  : /home/duc13/Downloads/Classic.bmp

$ ./dup /home/duc13/Downloads/twisk.jar
Le fichier a une autre extension : /home/duc13/Downloads/twisk.jar
*/
