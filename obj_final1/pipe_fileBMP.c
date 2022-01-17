#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include "bitmap.h"
#include <time.h>

#define INT_SIZE sizeof(unsigned int)
#define SHORT_SIZE sizeof(unsigned short)

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
	int tube[2] ;
	char ref[4096] ;
	char res[1024]  = "copie";
	char ch[5], ch2[5], ch3[5], ch4[5], ch6[5];
	int desc=0, desc2=0;
	char buff;
	//int etat;
	

	if(argc == 3)
	{
		strcpy(ref, argv[1]) ;
	}else{
		fprintf(stderr, "Too much arguments or not enough : \nUsage : ./prog  (absolute_path/)<[name_file].bmp>  ../<[copy_nameFile].bmp>\n") ;
		exit(1) ;		//argument list too long or too short
	}
	
	if( stat(ref, &sts) == -1 && errno == ENOENT )
	{
		fprintf( stderr, "The file %s doesn't exist \n", argv[1] ) ;
		exit(1) ;		//no such file or directory
	}
	else if(S_ISDIR(sts.st_mode))
	{
		strcpy(ref, argv[1]) ;
		fprintf(stderr, "This file is a directory : %s !\n", ref) ;
	}
	
	strcat( res, argv[2] ) ;
	
	
	
	BMPFILEHEADER fileHeader;
	BMPIMGHEADER infoHeader;
	if(pipe(tube) == -1){
		perror("pipe failed");
		exit(1) ;
	}
	if(S_ISREG(sts.st_mode))
	{
		fprintf(stdout, "The size of the reg file is :  %ld o\n", sts.st_size) ;
		desc = open(ref, O_RDONLY) ;
		if((desc2 = open( res, O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU )) == -1){
			perror("open write file has failed");
			exit(-1);
		}
		//on verifie si c'est un bien un fichier bmp
		read(desc, &fileHeader.file_type, SHORT_SIZE);
		write(desc2, &fileHeader.file_type, SHORT_SIZE);
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
			write(desc2, &infoHeader.width, INT_SIZE);
			read(desc, &infoHeader.height, INT_SIZE);
			write(desc2, &infoHeader.height, INT_SIZE);
			read(desc, &infoHeader.planes, SHORT_SIZE) ;
			write(desc2, &infoHeader.planes, SHORT_SIZE) ;
			read(desc, &infoHeader.bits_per_pixel, SHORT_SIZE);
			write(desc2, &infoHeader.bits_per_pixel, SHORT_SIZE);
			lseek(desc, 0x1E, SEEK_SET) ;
			lseek(desc2, 0x1E, SEEK_SET) ;
			read(desc, &infoHeader.compression, INT_SIZE);
			write(desc2, &infoHeader.compression, INT_SIZE);
			read(desc, &infoHeader.size_bitmap, INT_SIZE) ;
			write(desc2, &infoHeader.size_bitmap, INT_SIZE);
			
			showBmpHead(&fileHeader);
			showBmpInfoHead(&infoHeader);
			
			if(infoHeader.bits_per_pixel != 0x18 || fileHeader.bitmap_offset != 54 || infoHeader.header_size != 40 || infoHeader.compression != 0 ){
				fprintf(stderr, "Unsupported file format %d\n", infoHeader.bits_per_pixel);
				exit(1) ;		//operation not permitted
			}
			lseek(desc,fileHeader.bitmap_offset, SEEK_SET) ;
			lseek(desc2, fileHeader.bitmap_offset, SEEK_SET) ;

			while(read(desc, &buff, sizeof(char)) > 0 ){
				write(desc2, &buff, sizeof(char)) ;
			}
			
			sprintf( ch6, "%d", fileHeader.bitmap_offset ) ;
			sprintf( ch3, "%d", infoHeader.width ) ;
			sprintf( ch4, "%d", infoHeader.height ) ;
			close(desc);
			close(desc2);
		}
		else
		{
			fprintf(stderr, "Error : is not a bmp file, %d\n", fileHeader.file_type) ;
			close(desc) ;
			close(desc2);
			exit(-1) ;
		}

	}
	

	pid_t pid = fork() ;
		
	if(pid == -1)
	{
		perror("erreur fork");
		exit(1) ;
	}
	else if(!pid)
	{		//fils lecteur?

		sprintf( ch, "%d", tube[0] ) ;
		sprintf(ch2, "%d", tube[1]) ;
		fprintf(stdout, "je suis le fils, mon pid est %d\n",getpid()) ;
		fprintf(stdout, "%s \t %s\n", ref,res) ;
		sleep(2);
		if( execl ( "./fils","fils",ch, ch2, ch3, ch4 , ch6, ref,res, NULL ) < 0)
			fprintf(stderr, "Execl error\n") ;
		exit(-1);
		
	}
	else{
		close(tube[0]) ;		//shut the read part
		if(infoHeader.bits_per_pixel == 24)
		{
			int desc3 ;
			if((desc3 = open(ref, O_RDONLY))==-1)
			{
				perror( "Error open read file has failed" ) ;
				return EXIT_FAILURE;
			}
			char buffer_tp;
			while(read(desc3, &buffer_tp, sizeof(char)) > 0){
				if( write(tube[1], &buffer_tp, sizeof(char)) == -1 ){
					perror("Error writing in pipes or in copy");
					return EXIT_FAILURE;
				}
			}
			close(tube[1]);
			close(desc3);
			
			
		}
		else{
			fprintf(stderr, "Unsupported file format : not 24 bytes/pixel");
			return EXIT_FAILURE;
		}
	}
	
	return 0;
}

