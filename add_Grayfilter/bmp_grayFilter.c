#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include "bitmap.h"

#define INT_SIZE sizeof(unsigned int)
#define SHORT_SIZE sizeof(unsigned short)
#define CHAR_SIZE sizeof(unsigned char)
#define MAX_HEIGHT 512
#define MAX_WIDTH 630


void showBmpHead(BMPFILEHEADER* entete_fichier) 
{
	//fprintf(stdout,"Taille du fichier bmp : %d kb\n", entete_fichier->file_size/1024);
	//fprintf(stdout,"Réservé mot 1 : %d\n",  entete_fichier->reserved1);
	//fprintf(stdout,"Réservé mot 2 : %d\n",  entete_fichier->reserved2);
	fprintf(stdout,"Octets de l'offset des données réelles de l'image : %d\n\n",  entete_fichier->bitmap_offset);
}
void showBmpInfoHead(BMPIMGHEADER* entete_bitmap) 
{
	fprintf(stdout,"Bitmap Information Header:\n" );
	//fprintf(stdout,"Taille de l'entête: %d\n" ,entete_bitmap->header_size);
	fprintf(stdout,"Largeur en px : %d\n" ,entete_bitmap->width);
	fprintf(stdout,"Hauteur en px : %d\n" ,entete_bitmap->height);
	//fprintf(stdout,"Nbre de plans utilisés de l'image : %d\n(nbre de plans correspond au nbre de palettes, par défaut 1)\n" ,entete_bitmap->planes);
	fprintf(stdout,"Nb de bits par px: %d\n" ,entete_bitmap->bits_per_pixel);
	fprintf(stdout,"Compression (0 pas de comp) : %d\n" ,entete_bitmap->compression);
	//fprintf(stdout,"Taille bmp : %d o\n" ,entete_bitmap->size_bitmap);
	//fprintf(stdout,"Résolution horizontale : %d px.m^(-1)\n" ,entete_bitmap->horiz_resolution);
	//fprintf(stdout,"Résolution verticale : %d px.m^(-1)\n" ,entete_bitmap->vert_resolution);
	//fprintf(stdout,"Nombre de couleurs utilisées : %d\n(0 -> palette entière)\n" ,entete_bitmap->colors_used);
	//fprintf(stdout,"Nb de couleurs importantes : %d\n" ,entete_bitmap->colors_important);
}

int main ( int argc, char* argv[] )
{
	struct stat sts ;
	char* ref =(char*)malloc(4096*sizeof(char));
	
	if( argc == 2 )
		strcpy( ref, argv[1] );
	else if(argc == 1)
		strcpy( ref, "Classic.bmp" );
	else{
		fprintf(stderr, "Too much argument : Usage : ./bmp_gray (absolute_path/)<[name_file].bmp>") ;
		exit(7) ;		//argument list too long
	}
		
	if( stat(ref, &sts) == -1 && errno == ENOENT ){
		fprintf( stderr, "The file %s doesn't exist \n", argv[1] ) ;
		exit(2) ;		//no such file or directory
	}


	// ouverture pr lecture
	int desc = open( ref, O_RDONLY ) ;
	int desc2;
	if(desc == -1){
		perror("open has failed \n");
		exit(-1);		//operation not permitted
	}
	//ouverture pr ecriture
	char* token = strtok(ref, "/") ;
	while( token != NULL ){
		strcpy(ref, token) ;
		token = strtok(NULL, "/") ;
	}
	char* token2 = strtok(ref, ".") ;
	strcpy(ref, token2);
	
	char* res = strcat(ref, "_gray.bmp");
		
	if((desc2 = open( res, O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU )) == -1){
		perror("open write grey-scaled file has failed");
		exit(-1);
	}
	free(ref);
	
	BMPFILEHEADER fileHeader;
	BMPIMGHEADER infoHeader;
	

	read(desc, &fileHeader.file_type, SHORT_SIZE);
	write(desc2, &fileHeader.file_type, SHORT_SIZE) ;
	if (fileHeader.file_type == 0x4d42 && fileHeader.file_type == 19778)   	//"BM"
	{
		fprintf(stdout,"The file type identification is correct!" );
		fprintf(stdout,"\nFile identifier: %d\n", fileHeader.file_type);
		read(desc, &fileHeader.file_size, INT_SIZE);
		write(desc2, &fileHeader.file_size, INT_SIZE);
		lseek(desc, 0x0A, SEEK_SET) ;
		lseek(desc2, 0x0A, SEEK_SET) ;
		read(desc, &fileHeader.bitmap_offset, INT_SIZE) ;
		write(desc2, &fileHeader.bitmap_offset, INT_SIZE) ;
		read(desc, &infoHeader.header_size, INT_SIZE) ;
		write(desc2, &infoHeader.header_size, INT_SIZE) ;
		read(desc, &infoHeader.width, INT_SIZE);
		write(desc2, &infoHeader.width, INT_SIZE) ;
		read(desc, &infoHeader.height, INT_SIZE);
		write(desc2, &infoHeader.height, INT_SIZE) ;
		lseek(desc, 0x1C, SEEK_SET) ;
		lseek(desc2, 0x1C, SEEK_SET) ;
		read(desc, &infoHeader.bits_per_pixel, SHORT_SIZE);
		write(desc2, &infoHeader.bits_per_pixel, SHORT_SIZE) ;
		lseek(desc, 0x1E, SEEK_SET) ;
		lseek(desc2, 0x1E, SEEK_SET) ;
		read(desc, &infoHeader.compression, INT_SIZE);
		write(desc2, &infoHeader.compression, INT_SIZE) ;
		read(desc, &infoHeader.size_bitmap, INT_SIZE) ;
		write(desc2, &infoHeader.size_bitmap, INT_SIZE);
		
		fprintf(stdout,"Size: %d*%d\n", infoHeader.width, infoHeader.height) ;
		showBmpInfoHead(&infoHeader);
		
		if(infoHeader.bits_per_pixel != 0x18){
			fprintf(stderr, "The number of bits occupied by a pixel is incorrect (must be 24), not %d", infoHeader.bits_per_pixel);
			close(desc);
			close(desc2);
			exit(1) ;		//operation not permitted
		}
		
		int size = lseek(desc, 0, SEEK_END) ;
		size -= fileHeader.bitmap_offset ;
		fprintf(stdout, "size : %d\n", size) ;
		
		lseek(desc, fileHeader.bitmap_offset, SEEK_SET) ;
		lseek(desc2, fileHeader.bitmap_offset, SEEK_SET) ;
		if(size >= MAX_WIDTH * MAX_HEIGHT){
			fprintf(stderr, "Fichier trop volumineux, width > 630 et height > 512");
			close(desc);
			close(desc2);
			exit(-1) ;
		}
		PxInformation px[infoHeader.width][infoHeader.height] ;
		
		
		for(unsigned int k=0; k<infoHeader.width; k++){
			
			unsigned char gris, r, v, b;

			for(unsigned int p=0; p<infoHeader.height; p++){
				read(desc, &px[k][p], sizeof(PxInformation)) ;
				r = px[k][p].rgbRouge;
				v = px[k][p].rgbVert;
				b = px[k][p].rgbBleu;
				gris = (unsigned char) (r * .299 + v * .587 + b * .114 );
				px[k][p].rgbRouge = gris;
				px[k][p].rgbVert = gris;
				px[k][p].rgbBleu = gris;
				
				if(write(desc2, &px[k][p], sizeof(PxInformation)) <= 0){
					fprintf(stderr,"Erreur en ecrivant le pixel");
					return errno ;
				}
			}
			int nb_bytes_per_line = (infoHeader.bits_per_pixel/8)*infoHeader.width;
			if( nb_bytes_per_line % 4 != 0)
			{
				nb_bytes_per_line = 4-nb_bytes_per_line%4 ;
				read(desc, &px[k], nb_bytes_per_line);
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
		
		close(desc);
		close (desc2) ;
		
	}
	else
	{
		fprintf(stderr, "Error : is not a bmp file, %d\n", fileHeader.file_type) ;
		close(desc) ;
		close(desc2);
		exit(-1) ;
	}
	return 0 ;
}


/*
OUTPUT :
$ gcc -o bmp_gray bmp_grayFilter.c
$ ./bmp_gray /home/duc13/Downloads/Classic.bmp
The file type identification is correct!
File identifier: 19778
Size: 163*163
Bitmap Information Header:
Largeur en px : 163
Hauteur en px : 163
Nb de bits par px: 24
Compression (0 pas de comp) : 0
size : 80196
*/

