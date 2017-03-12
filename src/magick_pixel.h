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

/* magick_pixel.h

   written by: Oliver Cordes 2017-03-07
   changed by: Oliver Cordes 2017-03-12

   $Id$

*/


#ifndef __magick_pixel_h

#define __magick_pixel_h 1



#include "config.h"
#include "project.h"


#if MAGICK_VERSION >= 7
#include <MagickWand/MagickWand.h>
#else
#include <wand/magick_wand.h>
#endif

void magick_draw_line( parsenode *, parsenode *, parsenode * );
void magick_draw_circle( parsenode *, parsenode *, parsenode *, int );
void magick_draw_rectangle( parsenode *, parsenode *, parsenode *, int );
void magick_draw_roundrectangle( parsenode *, parsenode *, parsenode *, parsenode *, int );
void magick_draw_polygon( parsenode *, parsenode *, int, int  );

#endif
