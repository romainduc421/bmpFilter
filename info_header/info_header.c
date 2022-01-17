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


void showBmpHead(BMPFILEHEADER* entete_fichier) 
{
	fprintf(stdout,"Taille du fichier bmp : %d Kio soit %d o\n", entete_fichier->file_size/1024, entete_fichier->file_size);
	fprintf(stdout,"Réservé mot 1 : %d\n",  entete_fichier->reserved1);
	fprintf(stdout,"Réservé mot 2 : %d\n",  entete_fichier->reserved2);
	fprintf(stdout,"Octets de l'offset des données réelles de l'image : %d\n\n",  entete_fichier->bitmap_offset);
}
void showBmpInfoHead(BMPIMGHEADER* entete_bitmap) 
{
	fprintf(stdout,"Bitmap Information Header:\n" );
	fprintf(stdout,"Taille de l'entête: %d\n" ,entete_bitmap->header_size);
	fprintf(stdout,"Largeur en px : %d\n" ,entete_bitmap->width);
	fprintf(stdout,"Hauteur en px : %d\n" ,entete_bitmap->height);
	fprintf(stdout,"Nbre de plans utilisés de l'image : %d\n(nbre de plans correspond au nbre de palettes, par défaut 1)\n" ,entete_bitmap->planes);
	fprintf(stdout,"Nb de bits par px: %d\n" ,entete_bitmap->bits_per_pixel);
	fprintf(stdout,"Compression (0 pas de comp) : %d\n" ,entete_bitmap->compression);
	fprintf(stdout,"Taille bmp : %d o\n" ,entete_bitmap->size_bitmap);
	fprintf(stdout,"Résolution horizontale : %d px.m^(-1)\n" ,entete_bitmap->horiz_resolution);
	fprintf(stdout,"Résolution verticale : %d px.m^(-1)\n" ,entete_bitmap->vert_resolution);
	fprintf(stdout,"Nombre de couleurs utilisées : %d\n(0 -> palette entière)\n" ,entete_bitmap->colors_used);
	fprintf(stdout,"Nb de couleurs importantes : %d\n" ,entete_bitmap->colors_important);
}

int main ( int argc, char* argv[] )
{
	struct stat sts ;
	char ref [4096];
	
	if( argc == 2 )
		strcpy( ref, argv[1] );
	else if(argc)
		strcpy( ref, "Classic.bmp" );
	else{
		fprintf(stderr, "Too much argument : Usage : ./info (absolute_path/)<Classic.bmp>") ;
		exit(7) ;		//argument list too long
	}
		
	if( stat(ref, &sts) == -1 && errno == ENOENT ){
		fprintf( stderr, "The file %s doesn't exist \n", argv[1] ) ;
		exit(2) ;		//no such file or directory
	}


	//lecture
	int desc = open( ref, O_RDONLY ) ;
	if(desc == -1){
		perror("open has failed : Classic.bmp");
		exit(1);		//operation not permitted
	}
	
	BMPFILEHEADER fileHeader;
	BMPIMGHEADER infoHeader;
	

	read(desc, &fileHeader.file_type, SHORT_SIZE);  
	if (fileHeader.file_type == 0x4d42 && fileHeader.file_type == 19778)   
	{
		fprintf(stdout,"The file type identification is correct!" );
		fprintf(stdout,"\nFile identifier: %d\n", fileHeader.file_type);
		read(desc, &fileHeader.file_size, INT_SIZE);
		read(desc, &fileHeader.reserved1, SHORT_SIZE);
		read(desc, &fileHeader.reserved2, SHORT_SIZE);
		read(desc, &fileHeader.bitmap_offset, INT_SIZE);
		showBmpHead(&fileHeader);
		read(desc, &infoHeader.header_size, INT_SIZE);
		read(desc, &infoHeader.width, INT_SIZE);
		read(desc, &infoHeader.height, INT_SIZE);
		read(desc, &infoHeader.planes, SHORT_SIZE);
		read(desc, &infoHeader.bits_per_pixel, SHORT_SIZE);
		read(desc, &infoHeader.compression, INT_SIZE);
		read(desc, &infoHeader.size_bitmap, INT_SIZE);
		read(desc, &infoHeader.horiz_resolution, INT_SIZE);
		read(desc, &infoHeader.vert_resolution, INT_SIZE);
		read(desc, &infoHeader.colors_used, INT_SIZE);
		read(desc, &infoHeader.colors_important, INT_SIZE);
		showBmpInfoHead(&infoHeader);
		close(desc);
	}
	else{
		fprintf(stderr, "Error : is not a bmp file, %d\n", fileHeader.file_type) ;
		exit(1) ;		//operation not permitted
	}
	return 0 ;
}



/*
OUTPUT
$ gcc -o info info_header.c
$ ./info Classic.bmp
The file type identification is correct!
File identifier: 19778
Taille du fichier bmp : 78 Kio soit 80250 o
Réservé mot 1 : 0
Réservé mot 2 : 0
Octets de l'offset des données réelles de l'image : 54

Bitmap Information Header:
Taille de l'entête: 40
Largeur en px : 163
Hauteur en px : 163
Nbre de plans utilisés de l'image : 1
(nbre de plans correspond au nbre de palettes, par défaut 1)
Nb de bits par px: 24
Compression (0 pas de comp) : 0
Taille bmp : 80196 o
Résolution horizontale : 0 px.m^(-1)
Résolution verticale : 0 px.m^(-1)
Nombre de couleurs utilisées : 0
(0 -> palette entière)
Nb de couleurs importantes : 0


*/

