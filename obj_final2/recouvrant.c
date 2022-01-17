#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include "bitmap.h"



int main ( int argc, char* argv[] )
{
	unsigned char color = argv[8][0];
	printf("Nb d'arguments : %d\n", argc);

	close(atoi(argv[2])) ;
	for (int i = 0; i < argc; i++)
	{
		fprintf(stdout,"(%d) %s \n",i, argv[i]);
	}
	int width = atoi(argv[3]);
	int height = atoi(argv[4]);
	int bitmap_offset = atoi(argv[5]) ;
	
	char* ref = (char*) malloc(512*sizeof(char)) ;
	
	int fd_read = atoi(argv[1])  ;
	strcpy(ref, argv[6] ) ;
	int desc2 ;
	
	char* token = strtok(ref, "/") ;
	while( token != NULL ){
		strcpy(ref, token) ;
		token = strtok(NULL, "/") ;
	}
	fprintf(stdout,"Filename original : %s\n",ref);
	
	char* token3 = strtok(argv[7], ".") ;

	char* res ;
	if(color == 'G')
		res = strcat(token3, "_gray.bmp");
	else if(color == 'V')
		res = strcat(token3, "_green.bmp");
	else if(color == 'B')
		res = strcat(token3, "_blue.bmp");
	else if(color == 'R')
		res = strcat(token3, "_red.bmp");
	else{
		fprintf(stderr, "Error while attributing a name..");
		exit(-1);
	}
	
	fprintf(stdout,"Copy_Filter name : %s\n",res) ;
	
	if((desc2 = open( res, O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU )) == -1){
		perror("open write grey-scaled file has failed");
		exit(-1);
	}
	
	char buff; int pos=0;
	while(read(fd_read, &buff, 1) > 0 && pos<54){
		write(desc2, &buff, 1);
		pos+=1;
	}
	
	lseek(desc2, bitmap_offset, SEEK_SET) ;
	
	PxInformation px[width][height] ;
		
		
	for(unsigned int k=0; k<width; k++){
		
		unsigned char r, v, b;

		for(unsigned int p=0; p<height; p++){
			read(fd_read, &px[k][p], sizeof(PxInformation)) ;
			if(color == 'G'){
				r = px[k][p].rgbRouge;
				v = px[k][p].rgbVert;
				b = px[k][p].rgbBleu;
				unsigned char gris = (unsigned char) (r * .299 + v * .587 + b * .114 );
				px[k][p].rgbRouge = gris;
				px[k][p].rgbVert = gris;
				px[k][p].rgbBleu = gris;
			}
			else if(color == 'V'){
				px[k][p].rgbRouge = 0;
				px[k][p].rgbBleu = 0;
			}
			else if(color == 'B'){
				px[k][p].rgbRouge = 0;
				px[k][p].rgbVert = 0;
			}
			else if(color == 'R'){
				px[k][p].rgbBleu = 0;
				px[k][p].rgbVert = 0;
			}
			else{
				fprintf(stderr, "error while applying filter...");
				exit(-1);
			}
			
			if(write(desc2, &px[k][p], sizeof(PxInformation)) <= 0){
				fprintf(stderr,"Erreur en ecrivant le pixel");
				return errno ;
			}
		}
		int nb_bytes_per_line = (24/8)*width;
		if( nb_bytes_per_line % 4 != 0)
		{
			nb_bytes_per_line = 4-nb_bytes_per_line%4 ;
			read(fd_read, &px[k], nb_bytes_per_line);
		}
		
		for(int l=0; l < nb_bytes_per_line; l++)
		{
			unsigned char gray = 0;
			if((write(desc2, &gray, 1)) <= 0){
				fprintf(stderr, "Erreur pendant l'écriture du padding");
				return errno ;
			}
		}
	}
	
	
	
	
	
	
	close(fd_read) ;
	close(desc2) ;
	free(ref) ;
	return 0;
}

