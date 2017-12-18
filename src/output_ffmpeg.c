/* (C) Copyright 2010 by Oliver Cordes
        - ocordes ( at ) astro ( dot ) uni-bonn ( dot ) de


    This file is part of animation.

    animation is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    animation is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with animation.  If not, see <http://www.gnu.org/licenses/>.

*/

/* output_ffmpeg.c

   written by: Oliver Cordes 2013-01-11
   changed by: Oliver Cordes 2017-02-23

   $Id$

*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>

#include "config.h"

#if MAGICK_VERSION >= 7
#include <MagickWand/MagickWand.h>
#else
#include <wand/magick_wand.h>
#endif

#include "image.h"
#include "magick.h"
#include "output.h"
#include "output_ffmpeg.h"


/* temporary files */

char  tempdir[17] =  "/tmp/anim.XXXXXX";
char *tempfile = NULL;


/* ffmpeg stream */
FILE *ffmpeg = NULL;

/* start a new ffmpeg stream */


int ffmpeg_start( int blockmovies )
{
  char *filename;
  char  cmdline[1000];

  int   erg;

  filename = malloc( strlen( main_project->outputdir ) + strlen( main_project->output_filename ) + 2 );
  sprintf( filename, "%s/%s", main_project->outputdir, main_project->output_filename );

  if ( image_out_access( filename ) == 0 )
    {
      output( 1, "%s exists! File will not be saved!\n", filename );
      return 1;
    }


  if ( mkdtemp( tempdir ) == NULL )
    {
      output( 1, "Error creating a temporary directory '%s'  (%i: %s)!\n",
	      tempdir,
	      errno,
	      strerror( errno ) );
      return 1;
    }


  /*
  erg = unlink( filename );
  if ( ( erg != 0 ) && ( errno != ENOENT ) )
    {
      output( 1, "Can't remove output file '%s' (%s)\n",
	      filename, strerror( errno ) );
      return 1;
    }
    */

  tempfile = malloc( strlen( tempdir ) + strlen( "pipe.ffmpeg" ) + 2 );
  sprintf( tempfile, "%s/%s", tempdir, "pipe.ffmpeg" );

  erg = mkfifo( tempfile, 0600 );
  if ( erg != 0 )
    {
      output( 1, "Can't create ffmpeg pipe '%s' (%s)\n",
	      tempfile, strerror( errno ) );
      return 1;
    }

  snprintf( cmdline,
            1000,
            //"ffmpeg -y -f rawvideo -s %ix%i -pix_fmt yuv420p -i %s -r %i -framerate %i %s 2> ffmpeg.log &",
            "ffmpeg -y -f rawvideo -s %ix%i -pix_fmt yuv420p -i %s -r %i -framerate %i  -f avi -vcodec mpeg4 -b:v 800k %s 2> ffmpeg.log &",
	    main_project->geometry[0],
	    main_project->geometry[1],
	    tempfile,
	    main_project->fps,
      main_project->fps,
	    filename
	    );

  free( filename );

  output( 1, "ffmpeg cmd: %s\n", cmdline );

  erg = system( cmdline );
  if ( erg != 0 )
    {
      output( 1, "Can't start ffmpeg sub process (err=%i)!\n",
	      erg );
      return 1;
    }

  ffmpeg = fopen( tempfile, "w" );

  return ffmpeg == NULL;
}


/* write out an image in yuv */

void ffmpeg_convert_rgb_yuv420p_orig( char *rgb, char *yuv420p, int width, int height )
{
  unsigned int i = 0;
  unsigned int numpixels;
  unsigned int ui;
  unsigned int vi;
  unsigned int s = 0;

  int j, k;

#define sR (char)(rgb[s+0])
#define sG (char)(rgb[s+1])
#define sB (char)(rgb[s+2])

  numpixels = width * height;
  ui = numpixels;
  vi = ui + numpixels / 4;


  for (j = 0; j < height; j++)
    for (k = 0; k < width; k++)
      {
	yuv420p[i] = (char)( (66*sR + 129*sG + 25*sB + 128) >> 8) + 16;

	if (0 == j%2 && 0 == k%2)
	  {
	    yuv420p[ui++] = (char)( (-38*sR - 74*sG + 112*sB + 128) >> 8) + 128;
	    yuv420p[vi++] = (char)( (112*sR - 94*sG - 18*sB + 128) >> 8) + 128;
	  }
	i++;
	s += 3;
      }
}

void compress_rgb( unsigned char *rgb, int width, int x, int y, int *r, int *g, int *b )
{
  int pos;

  x *= 2;
  y *= 2;;
  (*r) = 0;
  (*g) = 0;
  (*b) = 0;
  pos = (y*width+x)*3;
  (*r) += rgb[pos];
  (*g) += rgb[pos+1];
  (*b) += rgb[pos+2];
  pos = (y*width+x+1)*3;
  (*r) += rgb[pos];
  (*g) += rgb[pos+1];
  (*b) += rgb[pos+2];
  pos = ((y+1)*width+x)*3;
  (*r) += rgb[pos];
  (*g) += rgb[pos+1];
  (*b) += rgb[pos+2];
  pos = ((y+1)*width+x+1)*3;
  (*r) += rgb[pos];
  (*g) += rgb[pos+1];
  (*b) += rgb[pos+2];
  (*r) = (*r) >> 2;
  (*g) = (*g) >> 2;
  (*b) = (*b) >> 2;
}


void ffmpeg_convert_rgb_yuv420p( unsigned char *rgb, unsigned char *yuv420p, int width, int height )
{
 int i;
 int x,y;

 unsigned  char *Y, *Cb, *Cr;
 int  r, g, b;

 int  w;

 i = width*height;
 Y = yuv420p;
 Cb = Y + i;
 Cr = Cb + i / 4;

 i = width * height;
 i = i + i / 2;

 for (x=0;x<i;x++)
   yuv420p[x] = 128;

 for (y=0;y<height;y++)
   for (x=0;x<width;x++)
     {
       i = (y*width+x)*3;

       r = rgb[i];
       g = rgb[i+1];
       b = rgb[i+2];



       w = 128;
       w += 66 * r;
       w += 129 * g;
       w += 25 * b;
       w = w >> 8;
       /*Y[y*width+x] =  (char)( (66*(int)rgb[i] + 129*(int)rgb[i+1] + 25*(int)rgb[i+2] + 128) >> 8) + 16; */
       Y[y*width+x] = (char)(w + 16);

       /*if ( ( y == 300 ) )
	 printf( "%i %i %i %i\n", rgb[i], rgb[i+1], rgb[i+2], w ); */

     }

 for(y=0;y<height/2;y++)
   for(x=0;x<width/2;x++)
     {
       /*i = ( y*2*width+(x*2))*3;

       r = rgb[i];
       g = rgb[i+1];
       b = rgb[i+2]; */

       /* using the mean of all 4 values */
       compress_rgb( rgb, width, x, y, &r, &g, &b );
       w = 128;
       w -= 38*r;
       w -= 74*g;
       w += 112*b;
       w = w >> 8;
       Cb[y*width/2+x] = (unsigned char) (w + 128 );

       w = 128;
       w += 112*r;
       w -= 94*g;
       w -= 18*b;
       w = w >> 8;
       Cr[y*width/2+x] = (unsigned char) (w + 128 );
       /*Cb[y*width+x] =  (unsigned char)( (-38*rgb[i] - 74*rgb[i+1] + 112*rgb[i+2] + 128) >> 8) + 128;
	 Cr[y*width+x] = (unsigned char)( (112*rgb[i] - 94*rgb[i+1] - 18*rgb[i+2] + 128) >> 8) + 128; */
       /*Cb[y*width+x] = 128;
	 Cr[y*width/2+x] = 128; */
     }
}


void ffmpeg_create_yuv( unsigned char **yuv_pixels, int *yuv_size)
{
  unsigned char     *rgb_pixels;
  int                rgb_size;

  int                i;

  MagickBooleanType  result;


  magick_check_current_image();

  rgb_size = 3 * current_image->width * current_image->height;
  rgb_pixels = malloc( rgb_size );

  i = current_image->width * current_image->height;
  (*yuv_size) = i + i / 2;
  (*yuv_pixels) = malloc( (*yuv_size) );

  result = MagickExportImagePixels( current_image->im,
				    0, 0,
				    current_image->width, current_image->height,
				    "RGB",
				    CharPixel,
				    rgb_pixels );

  if ( result == MagickFalse )
  {
    output( 1, "Pixel extraction failed!\n" );
    free( (*yuv_pixels) );
    (*yuv_pixels) = NULL;
  }
  else
  {
    ffmpeg_convert_rgb_yuv420p( rgb_pixels,
				(*yuv_pixels),
				current_image->width,
				current_image->height );

  }

  free( rgb_pixels );
}


void ffmpeg_save_yuv( int stream )
{
  char          *filename;
  FILE          *file;

  unsigned char *yuv_pixels;
  int            yuv_size;
  int w;

  ffmpeg_create_yuv( &yuv_pixels, &yuv_size );

  if ( stream == 0 )
    {
      filename = image_out_raw_filename( "yuv" );

      output( 10, "Saving %s ...\n", filename );

      file = fopen( filename, "w" );
      fwrite( yuv_pixels, yuv_size, 1, file );
      fclose( file );

      output( 10, "Done.\n" );
      free( filename );
    }
  else
    {
      if ( ffmpeg != NULL )
	{
	  output( 1, "Saving data to ffmpeg!\n" );
	  w = fwrite( yuv_pixels, yuv_size, 1, ffmpeg );
	  if ( w != 1 )
	    {
	      output( 1, "Error in writing data to ffmpeg stream!\n" );
	    }
	  fflush( ffmpeg );
	}
    }

  free( yuv_pixels );
}


/* write out an image */

int ffmpeg_out_func( void )
{
  ffmpeg_save_yuv( 1 );
  return 0;
 }


/* close the ffmpeg stream */

void ffmpeg_done( void )
{
  int erg;

  if ( ffmpeg != NULL )
    {
      /*pclose( ffmpeg ); */
      fclose( ffmpeg );
    }

  /* remove ffmpeg pipe */
  if ( tempfile != NULL )
    {
      erg = unlink( tempfile );
      if ( erg != 0 )
	output( 1, "Can't remove ffmpeg pipe '%s' (%s)!\n",
		tempfile, strerror( errno ) );
    }

  /* removing temp directory */
  erg = rmdir( tempdir );
  /*if ( erg != 0 )
    output( 1, "Can't delete tempory directory '%s' (%s)!\n",
    tempdir, strerror( errno ) ); */

}
