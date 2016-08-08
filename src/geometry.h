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

/* geometry.h

   written by: Oliver Cordes 2010-07-02
   changed by: Oliver Cordes 2010-07-02

   $Id: geometry.h 117 2010-07-02 04:19:29Z ocordes $
*/

#ifndef __geometry_h 
#define __geometry_h 1

typedef struct{
  char *name;
  int  width;
  int  height;
} _geometry;


int lookup_geometry( char *s, int *width, int *height );
int translate_geometry( char *s, int *width, int *height );


#endif
