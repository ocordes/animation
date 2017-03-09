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

/* magick_pixel.c

   written by: Oliver Cordes 2017-03-07
   changed by: Oliver Cordes 2017-03-09

   $Id$

*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "config.h"

#if MAGICK_VERSION >= 7
#include <MagickWand/MagickWand.h>
#else
#include <wand/magick_wand.h>
#endif

#include "amath.h"
#include "filldef.h"
#include "font.h"
#include "image.h"
#include "imagedef.h"
#include "magick.h"
#include "output.h"
#include "project.h"
#include "textfile.h"
#include "variable.h"


/* global variables */

/* internal variables */


/* internal routines */

PixelWand *magick_draw_setup_PixelWand( parsenode *node )
{
  PixelWand         *pwand = NULL;
  MagickBooleanType  erg;
  pendef_descr      *pendef;

  pendef = get_pendef_from_node( node );
  pwand = NewPixelWand();

  if ( pendef != NULL )
  {
    erg = PixelSetColor( pwand, pendef->color);
  }

  return pwand;
}

PixelWand *magick_draw_setup_FillWand( PixelWand *pen_wand, parsenode *filldef )
{
  PixelWand *pwand = NULL;

  pwand = NewPixelWand();

  if ( filldef == NULL )
    PixelSetColorFromWand( pwand, pen_wand );

  return pwand;
}


/* module procedures */

void magick_draw_circle( parsenode *central, parsenode *radius, parsenode *pendef, parsenode *filldef )
{
  PixelWand    *pwand = NULL;
  PixelWand    *fwand = NULL;

  DrawingWand  *dwand = NULL;

  pwand = magick_draw_setup_PixelWand( pendef );
  fwand = magick_draw_setup_FillWand( pwand, NULL );

  dwand = NewDrawingWand();

  DrawSetFillColor( dwand, fwand );

  DestroyDrawingWand( dwand );
  DestroyPixelWand( fwand );
  DestroyPixelWand( pwand );
}
