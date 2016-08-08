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

/* font.h

   written by: Oliver Cordes 2012-10-26
   changed by: Oliver Cordes 2013-03-03

   $Id: font.h 687 2014-09-14 17:53:49Z ocordes $

*/

#ifndef __font_h

#define __font_h 1


#include "config.h"

#include "parsetree.h"

typedef struct{
  char *name;
  char *font_name;
  char *bold_name;
  char *italic_name;
  char *bolditalic_name;
  int   font_size;
  char *font_color;
  int   font_separator;
} font_descr;
  


void font_init( void );
void font_done( void );

void font_print_fonts( void );


font_descr *font_get_font( char *name );


void font_start( parsenode *name );
void font_end( void );
void font_set_font( parsenode *font );
void font_set_bold( parsenode *bold );
void font_set_italic( parsenode *italic );
void font_set_bolditalic( parsenode *bolditalic );
void font_set_size( parsenode *size );
void font_set_color( parsenode *color );
void font_set_separator( parsenode *sep );


#endif
