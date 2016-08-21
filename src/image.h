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

/* image.h

   written by: Oliver Cordes 2012-10-26
   changed by: Oliver Cordes 2016-08-21

   $Id: image.h 687 2014-09-14 17:53:49Z ocordes $

*/


#ifndef __image_h

#define __image_h 1


#include "config.h"
#include "parsetree.h"
#include "project.h"


void image_init( char *path );
void image_done( void );

void image_out_init( char * );
void image_out_write( void );
void image_out_done( void );

void image_list_output_formats( void );


char *image_out_raw_filename( char *ending );
int image_out_access( char *filename );


int image_position_x( parsenode *nposx, int width, int offset );
int image_position_y( parsenode *nposx, int width, int offset );


#endif
