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

/* image.c

   written by: Oliver Cordes 2012-10-26
   changed by: Oliver Cordes 2016-08-30

   $Id: image.c 687 2014-09-14 17:53:49Z ocordes $

*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include <sys/stat.h>

#include "config.h"


#include "amath.h"
#include "output_ffmpeg.h"
#include "output_magick.h"
#include "image.h"
#include "magick.h"
#include "output.h"
#include "project.h"


typedef struct{
  char *name;
  int  (*start_func)( int );
  int  (*write_func)(void);
  int  (*block_start_func)( char *);
  int  (*block_end_func)(void);
  void (*end_func)(void);
} _output_func;


_output_func output_funcs[] = {
  { "jpeg", NULL, magick_image_out, NULL, NULL, NULL, },
  { "png", NULL, output_magick_png, NULL, NULL, NULL, },
  { "ffmpeg", ffmpeg_start, ffmpeg_out_func, NULL, NULL, ffmpeg_done },
  { NULL, NULL }
};


_output_func *output_func;

int (*image_out_func)( void ) = NULL;

/* module procedures */


int image_position_x( parsenode *nposx, int width, int offset )
{
  constant *con;
  int       posx;

  con = math_execute_node( nposx );
  switch( con->type )
    {
    case constant_int:
      posx = con->i;
      break;
    case constant_double:
      posx = floor( con->d );
      break;
    case constant_string:
      if ( strcmp( con->s, "center" ) == 0 )
	{
	  posx = ( width - offset ) / 2;
	  if ( posx < 0 )
	    posx = 0;
	}
      else
	{
	  if ( strcmp( con->s, "left" ) == 0 )
	    {
	      posx = 0;
	    }
	  else
	    {
	      if ( strcmp( con->s, "right" ) == 0 )
		{
		  posx = ( width - offset - 1);
		  if ( posx < 0 )
		    posx = 0;
		}
	      else
		{
		  output( 1, "Warning: positional information '%s' ignored!\n", con->s );
		  posx = 0;
		}
	    }
	}
      break;
    default:
      posx = 0;
    }

  free_constant( con );

  return posx;
}


int image_position_y( parsenode *nposy, int height, int offset )
{
  constant *con;
  int       posy;

  con = math_execute_node( nposy );
  switch( con->type )
    {
    case constant_int:
      posy = con->i;
      break;
    case constant_double:
      posy = floor( con->d );
      break;
    case constant_string:
      if ( strcmp( con->s, "center" ) == 0 )
	{
	  posy = ( height - offset ) / 2;
	  if ( posy < 0 )
	    posy = 0;
	}
      else
	{
	  if ( strcmp( con->s, "top" ) == 0 )
	    {
	      posy = 0;
	    }
	  else
	    {
	      if ( strcmp( con->s, "bottom" ) == 0 )
		{
		  posy = ( height - offset - 1);
		  if ( posy < 0 )
		    posy = 0;
		}
	      else
		{
		  output( 1, "Warning: positional information '%s' ignored!\n", con->s );
		  posy = 0;
		}
	    }
	}
      break;
    default:
      posy = 0;
    }

  free_constant( con );

  return posy;
}


char *image_out_raw_filename( char *ending )
{
  char dummy[2000];

  snprintf( dummy, 2000, "%s/rawfile%05i.%s",
	    main_project->outputdir,
	    main_project->framenr,
	    ending );

  return strdup( dummy );
}


int image_out_access( char *filename )
{
  int         erg;
  struct stat buf;

  /* if overwrite modus, always give 1 */

  if ( main_project->overwrite == 1 )
    return 1;

  erg = stat( filename, &buf );

  output( 10, "stat %s erg=%i\n", filename, erg );
  /* file exist == 0, everything else is an error! */
  return erg;
}



void image_init( char *path )
{
  magick_init();
}


void image_done( void )
{
  magick_done();
}



/* image out procedures */

void image_out_find( char *type )
{
  int i;

  for( i=0;;i++)
    {
      if ( output_funcs[i].name == NULL )
	{
	  output_func = NULL;
	  break;
	}
      if ( strcmp( output_funcs[i].name, type ) == 0 )
	{
	  output_func = &output_funcs[i];
	  break;
	}
    }
}

void image_out_init( char *type )
{
  image_out_find( type );
  if ( output_func != NULL )
    {
      if ( output_func->start_func != NULL )
	output_func->start_func( main_project->blockmovies );
    }
}


void image_out_write( void )
{
  if ( output_func != NULL )
    {
      if ( output_func->write_func != NULL )
	output_func->write_func();
    }
}


void image_out_done( void )
{
   if ( output_func != NULL )
    {
      if ( output_func->end_func != NULL )
	output_func->end_func();
    }
}


int image_out_block_start( char *blockname )
{
  int erg = 1;

  if ( output_func != NULL )
    {
      if ( output_func->block_start_func != NULL )
	erg = output_func->block_start_func( blockname );
    }

  return erg;
}


int image_out_block_end( void )
{
  int erg = 1;

  if ( output_func != NULL )
    {
      if ( output_func->block_end_func != NULL )
	erg = output_func->block_end_func( );
    }

  return erg;
}
