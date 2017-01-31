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

/* output_magick.c

   writtem by: Oliver Cordes 2016-08-14
   changed by: Oliver Cordes 2017-01-31

*/

#include "image.h"
#include "magick.h"
#include "output.h"

#include "config.h"

#if MAGICK_VERSION >= 7
#include <MagickWand/MagickWand.h>
#else
#include <wand/magick_wand.h>
#endif


int output_magick( char *filename )
{
  MagickBooleanType result;

  magick_check_current_image( 1 );

  if ( image_out_access( filename ) == 0 )
    {
      output( 1, "%s exists! File will not be saved!\n", filename );
      return 0;
    }

  output( 10, "Saving %s ...\n", filename );

  result = MagickWriteImage( current_image->im, filename );

  if ( result == MagickFalse )
    {
      output( 1, "Warning: Image '%s' can't be written!\n", filename );
    }

  output( 10, "Done.\n" );

  if ( result == MagickTrue )
    return 0;
  else
    return 1;
}


int output_magick_jpeg( void )
{
  char *filename;
  int   result;

  filename = image_out_raw_filename( "jpg" );

  result = output_magick( filename );

  free( filename );

  return result;
}


int output_magick_png( void )
{
  char *filename;
  int   result;

  filename = image_out_raw_filename( "png" );

  result = output_magick( filename );

  free( filename );

  return result;
}
