/*
	Filename  :  gthd2.c (the decoder to gthc2.c)
	Written by:  Gerald R. Tamayo, 2004
*/
#include <stdio.h>
#include <stdlib.h>
#include "utypes.h"
#include "gtbitio.h"
#include "huf.h"

typedef struct {
	char algorithm[4];
	ulong file_size;
} file_stamp;

void copyright( void );

int main( int argc, char *argv[] )
{
	file_stamp fstamp;
	hfreq_type freq;
	int c = 0;
	ulong i_file_size = 0;

	if ( argc != 3 ) {
		fprintf(stderr, "\n Usage: gthd2 infile outfile");
		copyright();
		return 0;
	}

	if ( (gIN = fopen( argv[1], "rb" )) == NULL ) {
		fprintf(stderr, "\nError opening input file.");
		return 0;
	}
	fread( &fstamp, sizeof(file_stamp), 1, gIN );

	if ( (pOUT = fopen( argv[2], "wb" )) == NULL ) {
		fprintf(stderr, "\nError opening output file.");
		goto halt_prog;
	}
	init_put_buffer();
	
	/* ---- start the Decompression process. ---- */
	
	/* initialize hfreq. */
	init_hfreq();

	/* read the number of symbols from the file. */
	fread( &hcount, sizeof(int), 1, gIN );

	/* now read the symbols from the file. */
	for ( c = 0; c < hcount; c++ ){
		fread( &freq, sizeof(hfreq_type), 1, gIN );
		hfreq[ (unsigned char) freq.b ] = freq;
	}

	/* then create the corresponding huffman tree. */
	create_hufftree();

	/* called after fopening gIN, and after reading the file stamp. */
	init_get_buffer();

	fprintf(stderr, "\n Name of input file  : %s", argv[1] );
	fprintf(stderr, "\n\n  Decompressing...");

	/* finally, decompress. */
	for ( i_file_size = fstamp.file_size; i_file_size; i_file_size-- ){
		c = hdecompress( top );
		pfputc( (uchar) c );
	}
	flush_put_buffer();
	fprintf(stderr, "complete.\n");
	
	fprintf(stderr, "\n Name of output file : %s\n", argv[2] );
	
	halt_prog:
	
	free_get_buffer();
	free_put_buffer();
	if ( gIN ) fclose( gIN );
	if ( pOUT ) fclose( pOUT );
	return 0;
}

void copyright( void )
{
	fprintf(stderr, "\n\n Written by: Gerald R. Tamayo, 2004\n");
}
