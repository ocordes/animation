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
#include "type_array.h"
#include "type_point.h"
#include "variable.h"


/* global variables */

/* internal variables */


/* internal routines */

void magick_draw_setup_Wand( parsenode *node, int flags,
                            DrawingWand *dwand,
                            PixelWand **pwand, PixelWand **fwand )
{
  MagickBooleanType  erg;
  pendef_descr      *pendef;

  (*pwand) = NewPixelWand();
  (*fwand) = NewPixelWand();


  if ( flags == 0 )
    /* no fill pattern */
    PixelSetColor( (*fwand), "none");

  pendef = get_pendef_from_node( node );
  if ( pendef != NULL )
  {
    if ( pendef->color != NULL )
    {
      output( 2, "No color set for a pen definition!\n" );
      erg = PixelSetColor( (*pwand), pendef->color);
    }

    DrawSetStrokeWidth( dwand, pendef->size );

    if ( flags == 1 )
    {
      if ( pendef->fillcolor != NULL )
        PixelSetColor( (*fwand), pendef->fillcolor );
      else
        PixelSetColor( (*fwand), "none");
    }
  }
  DrawSetStrokeColor( dwand, pwand );
  DrawSetFillColor( dwand, fwand );
}


/* module procedures */

void magick_draw_circle( parsenode *central, parsenode *radius, parsenode *pendef, int filldef )
{
  PixelWand    *pwand = NULL;
  PixelWand    *fwand = NULL;

  DrawingWand  *dwand = NULL;

  constant     *xy;
  double        r;
  int           ix, iy, ir;

  dwand = NewDrawingWand();

  magick_draw_setup_Wand( pendef, filldef, dwand, &pwand, &fwand );

    /* DrawSetFillColor( dwand, fwand ); */

  /* extract the point from node */
  xy = math_execute_node_point( central );

  ix = round( xy->p.x );
  iy = round( xy->p.y );
  output( 2, "x=%i y=%i\n", ix, iy );

  r = math_execute_node_double( radius );
  ir = round( r );

  output( 2, "r=%i\n", ir );

  
  DestroyPixelWand( fwand );
  DestroyPixelWand( pwand );
  fwand = NULL; pwand = NULL;

  DrawCircle( dwand, xy->p.x, xy->p.y, xy->p.x+r, xy->p.y );
  MagickDrawImage( current_image->im, dwand);

  /* free all variables */
  free_constant( xy );

  DestroyDrawingWand( dwand );
  /*DestroyPixelWand( fwand );
  DestroyPixelWand( pwand ); */

}
