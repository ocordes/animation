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

/* magick.h

   written by: Oliver Cordes 2012-10-26
   changed by: Oliver Cordes 2017-01-30

   $Id: magick.h 687 2014-09-14 17:53:49Z ocordes $

*/


#ifndef __magick_h

#define __magick_h 1



#include "config.h"
#include "project.h"


#if MAGICK_VERSION >= 7
#include <MagickWand/MagickWand.h>
#else
#include <wand/magick_wand.h>
#endif


#define max_window_level 10

typedef struct{
  MagickWand *im;
  int         x;
  int         y;
  int         w;
  int         h;
} magick_stack_window;

typedef struct{
  MagickWand          *im;
  int                  width;
  int                  height;

  magick_stack_window  windows[max_window_level];
  int                  window_level;
} magick_image;


extern magick_image *current_image;

#define magick_check_current_image( x ) { if ( ( current_image == NULL ) || ( current_image->im == NULL ) ) return x; }


void magick_init( void);
void magick_done( void );


void magick_image_window( parsenode*, parsenode*, parsenode *, parsenode* );
void magick_image_endwindow( void );

void magick_image_load( filedef *file );

void magick_text( parsenode*, parsenode*, parsenode*, parsenode*, parsenode* );
void magick_textfile( parsenode*, parsenode*, parsenode*, parsenode*, parsenode* );

void magick_resize( int );

void magick_drawimage( parsenode*, parsenode*, parsenode* );

#endif
