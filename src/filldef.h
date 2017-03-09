/* (C) Copyright 2010-2017 by Oliver Cordes
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


/* filldef.h

  written by: Oliver Cordes 2017-03-04
  changed by: Oliver Cordes 2017-03-09

  $Id$

*/


#ifndef __filldef_h

#define __filldef_h  1

#include "config.h"

#if MAGICK_VERSION >= 7
#include <MagickWand/MagickWand.h>
#else
#include <wand/magick_wand.h>
#endif

#include "parsetree.h"



typedef struct {
  char *name;
  char *color;
  int   size;
} pendef_descr;


typedef struct{
  char *name;
} filldef_descr;

/* module init/done functions */
void filldef_init( void );
void filldef_done( void );
void filldef_print_defs( void );

void pendef_start( parsenode * );
void pendef_end( void );

void pendef_set_color( parsenode * );
void pendef_set_size( parsenode * );

pendef_descr *get_pendef_from_node( parsenode * );

void filldef_start( parsenode * );
void filldef_end( void );

filldef_descr *get_filldef_from_node( parsenode * );


#endif
