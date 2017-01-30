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

/* imagedef.h

   written by: Oliver Cordes 2012-12-12
   changed by: Oliver Cordes 2017-01-30

   $Id: imagedef.h 687 2014-09-14 17:53:49Z ocordes $

*/

#ifndef __imagedef_h

#define __imagedef_h 1



#include "config.h"

#if MAGICK_VERSION >= 7
#include <MagickWand/MagickWand.h>
#else
#include <wand/magick_wand.h>
#endif

#include "parsetree.h"

#define resize_none   0
#define resize_value  1
#define resize_factor 2

typedef struct{
  char       *name;
  char       *file_name;
  int         need_resize;
  int         resize_x;
  int         resize_y;
  double      resize_fac;
  MagickWand *im;
  int         width;
  int         height;
  int         composite_operator;
} imagedef_descr;



void imagedef_init( void );
void imagedef_done( void );

void imagedef_print_imagedefs( void );


imagedef_descr *imagedef_get_imagedef( char *name );


void imagedef_start( parsenode *name );
void imagedef_end( void );
void imagedef_set_filename( parsenode *filename );

void imagedef_set_resize_value( parsenode *x, parsenode *y );
void imagedef_set_resize_factor( parsenode *fac );
void imagedef_set_mode( parsenode *mode );


#endif
