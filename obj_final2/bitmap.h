// BMP-related data types based on Microsoft's own
#ifndef BITMAP_H
#define BITMAP_H

// File information header structure (14 octets)
typedef struct {
	unsigned short		file_type;		// must be "BM", or corresponding hexadecimal 0x4d42 or 19778 (decimal)
										//otherwise it's not a bitmap-file (signature not correct)
	unsigned int		file_size;		//file size in bytes (bytes 3-6)
	unsigned short		reserved1;		//reserved, must be set to 0 (bytes 7-8)
	unsigned short		reserved2;		//reserved, must be set to 0 (bytes 9-10)
	unsigned int		bitmap_offset;	//offset from file header to pixel data (bytes 11-14)
}BMPFILEHEADER ;

// Image information header structure	(40 octets)
typedef struct  {
	unsigned int		header_size;	//size of this structure (15-18)
	unsigned int		width;			//width of the image (19-22)
	unsigned int		height;			//height of the image (23-26)
	unsigned short		planes;			//plane attribute (27-28)
	unsigned short		bits_per_pixel;	//number of bits occupied by a px (29-30)
	unsigned int		compression;	//type of bmp data compression (31-34)
	unsigned int		size_bitmap;	//Size of pixel data, value equal to bitmap_offset minus header_size (35-38)
	unsigned int		horiz_resolution;	//horizontal resolution (39-42)
	unsigned int		vert_resolution;	//vertical resolution (43-46)
	unsigned int		colors_used;		//number of color indexes in the color table actually used by the bmp (47-50)
	unsigned int		colors_important;	//number of color indexes which have an important influence on the bmp display (51-54)

}BMPIMGHEADER ;

typedef struct{
	unsigned char	rgbBleu;
	unsigned char	rgbVert;
	unsigned char	rgbRouge;
	//unsigned char	rgbReserve;	//reserved, must be 0
} PxInformation ;

#endif
	
//BITMAP_H INCLUDED
