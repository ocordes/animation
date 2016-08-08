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

/* geometry.c

   written by: Oliver Cordes 2010-07-02
   changed by: Oliver Cordes 2010-07-02


   $Id: geometry.c 117 2010-07-02 04:19:29Z ocordes $

*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "geometry.h"


static _geometry  default_geometry[] = {
  { "VGA",  640, 480 },
  { "SVGA", 800, 600 },
  { "1080I", 1920, 1080 },
  { "1080P", 1920, 1080 },
  { "FULLHD", 1920, 1080 },
  { NULL, 0, 0 } };


int lookup_geometry( char *s, int *width, int *height )
{
  int i = 0;

  while ( default_geometry[i].name != NULL )
    {
      if ( strcmp( s, default_geometry[i].name ) == 0 )
	{
	  (*width)  = default_geometry[i].width;
	  (*height) = default_geometry[i].height;
	  return 0;
	}
      i++;
    }
  return -1;
}



int translate_geometry( char *s, int *width, int *height )
{
  char *p, *pp;


  p = strtok_r( s, "xX\0", &pp );

  if ( p == NULL ) return -1;

  (*width) = atoi( p );

  p = strtok_r( NULL, "\0", &pp );

  if ( p == NULL ) return -1;
  
  (*height) = atoi( p );


  return 0;
}

