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
   changed by: Oliver Cordes 2017-03-12

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

DrawingWand * magick_draw_setup_Wand( parsenode *node, int flags )
{
  MagickBooleanType  erg;
  pendef_descr      *pendef;
  PixelWand         *pwand, *fwand;
  DrawingWand       *dwand;

  dwand = NewDrawingWand();

  pwand = NewPixelWand();
  fwand = NewPixelWand();


  if ( flags == 0 )
    /* no fill pattern */
    PixelSetColor( fwand, "none");

  pendef = get_pendef_from_node( node );
  if ( pendef != NULL )
  {
    if ( pendef->color != NULL )
    {
      output( 2, "No color set for a pen definition!\n" );
      erg = PixelSetColor( pwand, pendef->color);
    }

    DrawSetStrokeWidth( dwand, pendef->size );

    if ( flags == 1 )
    {
      if ( pendef->fillcolor != NULL )
        PixelSetColor( fwand, pendef->fillcolor );
      else
        PixelSetColor( fwand, "none");
    }
  }
  DrawSetStrokeColor( dwand, pwand );
  DrawSetFillColor( dwand, fwand );

  DestroyPixelWand( fwand );
  DestroyPixelWand( pwand );

  return dwand;
}


PointInfo *convertArray2PointInfo( constant *array )
{
  PointInfo *points = NULL;

  int        i, s;

  int        error = 0;

  s = array->a.nr;

  points = malloc( sizeof( PointInfo) * s );

  output( 2, "convertArray2PointInfo called!\n" );
  if ( points != NULL )
  {
    for (i=0;i<s;++i)
    {
      if ( array->a.cons[i]->type == constant_point )
      {
        points[i].x = array->a.cons[i]->p.x;
        points[i].y = array->a.cons[i]->p.y;
        output( 2, "i=%i x=%f y=%f\n", i, points[i].x, points[i].y );
      }
      else
      {
        error = 1;
      }
    }
    if ( error != 0 )
    {
      output( 1, "Some elements of array are not points! Return zero array!\n");
      free( points );
      points = NULL;
    }
  }

  return points;
}


/* module procedures */

void magick_draw_line( parsenode *nstart,
                       parsenode *ndir,
                       parsenode *pendef )
{
  DrawingWand  *dwand = NULL;

  constant     *xy;
  constant     *dir;

  dwand = magick_draw_setup_Wand( pendef, 0);

  xy  = math_execute_node_point( nstart );
  dir = math_execute_node_point( ndir );

  DrawLine( dwand,
            xy->p.x, xy->p.y,
            xy->p.x + dir->p.x, xy->p.y + dir->p.y );
  MagickDrawImage( current_image->im, dwand);

  free_constant( xy );
  free_constant( dir );

  DestroyDrawingWand( dwand );
}


void magick_draw_circle( parsenode *central, parsenode *radius, parsenode *pendef, int filldef )
{
  DrawingWand  *dwand = NULL;

  constant     *xy;
  double        r;
  int           ix, iy, ir;


  dwand = magick_draw_setup_Wand( pendef, filldef );

    /* DrawSetFillColor( dwand, fwand ); */

  /* extract the point from node */
  xy = math_execute_node_point( central );

  ix = round( xy->p.x );
  iy = round( xy->p.y );
  output( 2, "x=%i y=%i\n", ix, iy );

  r = math_execute_node_double( radius );
  ir = round( r );

  output( 2, "r=%i\n", ir );


  DrawCircle( dwand, xy->p.x, xy->p.y, xy->p.x+r, xy->p.y );
  MagickDrawImage( current_image->im, dwand);

  /* free all variables */
  free_constant( xy );

  DestroyDrawingWand( dwand );
}


void magick_draw_rectangle( parsenode *nstart, parsenode *nsize, parsenode *pendef, int filldef )
{
  DrawingWand  *dwand = NULL;

  constant     *xy;
  constant     *size;


  dwand = magick_draw_setup_Wand( pendef, filldef );

  /* extract the point from node */
  xy   = math_execute_node_point( nstart );
  size = math_execute_node_point( nsize );

  DrawRectangle( dwand, xy->p.x, xy->p.y, xy->p.x+size->p.x, xy->p.y+size->p.y );
  MagickDrawImage( current_image->im, dwand);

  free_constant( xy );
  free_constant( size );

  DestroyDrawingWand( dwand );
}


void magick_draw_roundrectangle( parsenode *nstart,
                                 parsenode *nsize,
                                 parsenode *nradius,
                                 parsenode *pendef, int filldef )
{
  DrawingWand  *dwand = NULL;

  constant     *xy;
  constant     *size;
  constant     *radius;


  dwand = magick_draw_setup_Wand( pendef, filldef );

  /* extract the point from node */
  xy     = math_execute_node_point( nstart );
  size   = math_execute_node_point( nsize );
  radius = math_execute_node_point( nradius );

  DrawRoundRectangle( dwand,
                      xy->p.x, xy->p.y,
                      xy->p.x+size->p.x, xy->p.y+size->p.y,
                      radius->p.x, radius->p.y );
  MagickDrawImage( current_image->im, dwand);

  free_constant( xy );
  free_constant( size );
  free_constant( radius );

  DestroyDrawingWand( dwand );
}


void magick_draw_polygon( parsenode *narray,
                          parsenode *pendef, int filldef,
                          int polyline )
{
  DrawingWand  *dwand = NULL;
  PointInfo    *points;
  int           nr_points;

  constant     *array;

  dwand = magick_draw_setup_Wand( pendef, filldef );

  array = math_execute_node_array( narray );
  nr_points = array->a.nr;
  points = convertArray2PointInfo( array );
  free_constant( array );

  if ( points != NULL )
  {
    output( 2, "Drawing polygon with %i points ...\n", nr_points );
    if ( polyline == 0 )
      DrawPolygon( dwand, nr_points, points );
    else
      DrawPolyline( dwand, nr_points, points );
    MagickDrawImage( current_image->im, dwand);
    free( points );

  }

  DestroyDrawingWand( dwand );
}
