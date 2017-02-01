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

/* imagedef.c

   written by: Oliver Cordes 2012-12-12
   changed by: Oliver Cordes 2017-01-31

   $Id$

*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <assert.h>

#include "config.h"

#if MAGICK_VERSION >= 7
#include <MagickWand/MagickWand.h>
#else
#include <wand/magick_wand.h>
#endif

#include "abort.h"
#include "helpers.h"
#include "imagedef.h"
#include "output.h"
#include "parsetree.h"
#include "project.h"


#define          new_increment_imagedefs 5
imagedef_descr **imagedefs;
int              nr_imagedefs;
int              max_imagedefs;

imagedef_descr  *current_imagedef = NULL;


typedef struct{
  char *name;
  int   operator;
} _overlay_mode_option;


static _overlay_mode_option overlay_mode_options[] = {
  { "AtopCompositeOp", AtopCompositeOp },
  { "XorCompositeOp",  XorCompositeOp },
  { "PlusCompositeOp", PlusCompositeOp },
//  { "MinusCompositeOp", MinusCompositeOp },
//  { "AddCompositeOp",   AddCompositeOp },
//  { "SubtractCompositeOp", SubtractCompositeOp },
  { "DifferenceCompositeOp", DifferenceCompositeOp },
  { "MultiplyCompositeOp", MultiplyCompositeOp },
  { "BumpmapCompositeOp", BumpmapCompositeOp },
  { "CopyCompositeOp", CopyCompositeOp },
  { "CopyRedCompositeOp", CopyRedCompositeOp },
  { "CopyGreenCompositeOp", CopyGreenCompositeOp },
  { "CopyBlueCompositeOp", CopyBlueCompositeOp },
  { "ClearCompositeOp", ClearCompositeOp },
  { "DissolveCompositeOp", DissolveCompositeOp },
  { "DarkenCompositeOp", DarkenCompositeOp },
  { "LightenCompositeOp", LightenCompositeOp },
  { "HueCompositeOp", HueCompositeOp },
  { "SaturateCompositeOp", SaturateCompositeOp },
  { "ColorizeCompositeOp", ColorizeCompositeOp },
  { "LuminizeCompositeOp", LuminizeCompositeOp },
  { "ScreenCompositeOp", ScreenCompositeOp },
  { "CopyCyanCompositeOp", CopyCyanCompositeOp },
  { "CopyMagentaCompositeOp", CopyMagentaCompositeOp },
  { "CopyYellowCompositeOp", CopyYellowCompositeOp },
  { "CopyBlackCompositeOp", CopyBlackCompositeOp },
  { "ColorDodgeCompositeOp", ColorDodgeCompositeOp },
  { "HardLightCompositeOp", HardLightCompositeOp },
  { "SoftLightCompositeOp", SoftLightCompositeOp },
  { NULL, 0 } };


/* module funtions */

int  imagedef_operator_from_name( char *smode )
{
  int i;

  for (i=0;;i++)
    {
      if ( overlay_mode_options[i].name == NULL )
	break;
      else
	if ( strcmp( overlay_mode_options[i].name, smode ) == 0 )
	  return i;
    }

  return -1;
}


void imagedef_print_imagedefs( void )
{
  int i;

  output( 1, "image descriptions:  nr. of imagedefs: %i\n", nr_imagedefs );
  for (i=0;i<nr_imagedefs;i++)
    {
      output( 1, "  NAME: %s\n", imagedefs[i]->name );
      if ( imagedefs[i]->file_name != NULL )
	output( 1, "   file        : %s\n", imagedefs[i]->file_name );
    }
}


imagedef_descr *imagedef_new_imagedef( void )
{
  int              i;
  imagedef_descr **new_imagedefs;
  imagedef_descr  *new_imagedef;

  if ( nr_imagedefs == max_imagedefs )
    {
      max_imagedefs += new_increment_imagedefs;
      new_imagedefs =  malloc( sizeof( imagedef_descr* ) * max_imagedefs );
      if ( new_imagedefs == NULL ) aabort( abort_msg_malloc, "imagedef_new_imagedef(new_imagedefs_array)" );

      for (i=0;i<nr_imagedefs;i++)
	new_imagedefs[i] = imagedefs[i];

      free( imagedefs );
      imagedefs = new_imagedefs;
    }
  new_imagedef = malloc( sizeof( imagedef_descr ) );
  if ( new_imagedef == NULL ) aabort( abort_msg_malloc, "new_imagedef" );

  new_imagedef->name = NULL;
  new_imagedef->file_name = NULL;
  new_imagedef->need_resize = resize_none;
  new_imagedef->im = NULL;
  new_imagedef->composite_operator = AtopCompositeOp;

  imagedefs[nr_imagedefs] = new_imagedef;
  nr_imagedefs++;

  return new_imagedef;
}


imagedef_descr *imagedef_find_imagedef( char *name )
{
  int i;

  for (i=0;i<nr_imagedefs;i++)
    if ( strcmp( imagedefs[i]->name, name ) == 0 )
      return imagedefs[i];

  return NULL;
}


/* exported funtions */

imagedef_descr *imagedef_get_imagedef( char *name )
{
  return imagedef_find_imagedef( name );
}


void imagedef_start( parsenode *name )
{
  imagedef_descr *new_imagedef;
  char           *sname = NULL;

  sname =  get_string_from_node( name );

  new_imagedef = imagedef_find_imagedef( sname );
  if ( new_imagedef != NULL )
    {
      output( 1, "Warning: image '%s' is already defined! Skip definition!\n", sname );
      current_imagedef = NULL;

      return;
    }

  new_imagedef = imagedef_new_imagedef();
  new_imagedef->name = strdup( sname );

  current_imagedef = new_imagedef;

  printf( "starting a new imagedef '%s' \n", sname );

  nfree( sname );
  free_node( name );
}


void imagedef_end( void )
{
  if ( current_imagedef != NULL )
    {
      printf( "finishing imagedef\n" );
      current_imagedef = NULL;
    }
}


void imagedef_set_filename( parsenode *filename )
{
  char *sfilename = NULL;

  assert( current_imagedef != NULL );

  sfilename = get_string_from_node( filename );
  if ( current_imagedef->file_name != NULL )
    free( current_imagedef->file_name );
  current_imagedef->file_name = strdup( sfilename );

  nfree( sfilename );
  free_node( filename );
}


void imagedef_set_resize_value( parsenode *x, parsenode *y )
{
  int ix, iy;

  assert( current_imagedef != NULL );
  ix = get_int_from_node( x );
  iy = get_int_from_node( y );

  current_imagedef->need_resize = resize_value;
  current_imagedef->resize_x = ix;
  current_imagedef->resize_y = iy;

  free_node( x );
  free_node( y );
}


void imagedef_set_resize_factor( parsenode *fac )
{
  char *sfac = NULL;
  char *brk;

  char *p;
  double d;

  assert( current_imagedef != NULL );

  sfac = get_string_from_node( fac );


  p = strtok_r( sfac, "%\0", &brk );
  if ( p != NULL )
    {
      d = atof( p );
      current_imagedef->need_resize = resize_factor;
      current_imagedef->resize_fac = d / 100.0;
    }
  else
    output( 1, "Warning: Resize factor without '%'! Factor ignored!\n" );

  nfree( sfac );
  free_node( fac );
}


void imagedef_set_mode( parsenode *mode )
{
  char *smode = NULL;

  int   nr;

  assert( current_imagedef != NULL );

  smode = get_string_from_node( mode );

  nr = imagedef_operator_from_name( smode );

  if ( nr != -1 )
    {
      current_imagedef->composite_operator = overlay_mode_options[nr].operator;
      output( 1, "Set new composite mode '%s'!\n", smode );
    }
  else
    output( 1, "Warning: Opertator '%s' not found! Using defaults!\n", smode );

  nfree( smode );
  free_node( mode );
}


/* init/done funtions */

/* module funtions */

void imagedef_init( void )
{
  max_imagedefs = new_increment_imagedefs;

  imagedefs = malloc( sizeof( imagedef_descr* ) * max_imagedefs );

  if ( imagedefs == NULL ) aabort( abort_msg_malloc, "imagedef_init(new_imagedefs_array)" );
  nr_imagedefs = 0;
}


void imagedef_done( void )
{
  int i;

  for (i=0;i<nr_imagedefs;i++)
    {
      if ( imagedefs[i]->name != NULL ) free( imagedefs[i]->name );
      if ( imagedefs[i]->file_name != NULL ) free( imagedefs[i]->file_name );
      if ( imagedefs[i]->im != NULL )
	DestroyMagickWand( imagedefs[i]->im );
      free( imagedefs[i] );
    }
  free( imagedefs );
  imagedefs = NULL;
}
