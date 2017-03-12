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

/* magick.c

   written by: Oliver Cordes 2012-10-26
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
#include "font.h"
#include "image.h"
#include "imagedef.h"
#include "magick.h"
#include "output.h"
#include "project.h"
#include "textfile.h"
#include "variable.h"


/* global variables */

magick_image *current_image = NULL;

/* internal variables */



/* module procedures */

void magick_init( void )
{
  MagickWandGenesis();
}


void magick_done( void )
{
  if ( current_image != NULL )
    {
      if ( current_image->im != NULL )
	      {
	         DestroyMagickWand( current_image->im );
	      }

      free( current_image );
    }

  MagickWandTerminus();
}


magick_image *magick_image_new( void )
{
  magick_image *im;

  im = (magick_image*) malloc( sizeof( magick_image ) );
  im->im = NULL;

  return im;
}



void magick_image_info( void )
{
  magick_check_current_image();

  set_int_variable( main_project->vars, "IMAGE.X", current_image->width );
  set_int_variable( main_project->vars, "IMAGE.Y", current_image->height );
}


/* routines for the image stack */


void magick_push_image(  MagickWand *new_image, int x, int y, int w, int h )
{
  if ( current_image->window_level < max_window_level )
    {
      current_image->windows[current_image->window_level].im = current_image->im;
      current_image->windows[current_image->window_level].x = x;
      current_image->windows[current_image->window_level].y = y;
      current_image->windows[current_image->window_level].w = w;
      current_image->windows[current_image->window_level].h = h;
      current_image->im = new_image;
      current_image->window_level++;
    }
  else
    current_image->window_level++;
}


MagickWand *magick_pop_image( int *x, int *y, int *width, int *height )
{
  MagickWand *w;

  current_image->window_level--;

  if ( current_image->window_level <= max_window_level )
    {
      w = current_image->im;
      current_image->im = current_image->windows[current_image->window_level].im;
      (*x) = current_image->windows[current_image->window_level].x;
      (*y) = current_image->windows[current_image->window_level].y;
      (*width) = current_image->windows[current_image->window_level].w;
      (*height) = current_image->windows[current_image->window_level].h;
      current_image->windows[current_image->window_level].im = NULL;
    }
  else
    w = NULL;

  return w;
}


/* text rendering routines */


void magick_fonts_verify( void )
{
  // {
  //   char **fl;
  //   unsigned long number_fonts, i;
  //
  //   fl = MagickQueryFonts( font->font_name, &number_fonts );
  //
  //   for (i=0;i<number_fonts;i++)
  //     output( 2, "font (#%i): %s\n", i, fl[i]);
  //
  // }
}


void magick_textline_metrics( textfilelinedef *textline, font_descr *font )
{
  DrawingWand   *dwand = NULL;

  double        *fm = NULL;

  int            width  = 0;
  int            height = 0;
  int            asc_height = 0;
  int            sep = 0;

  int            i;

  /* setup the font and image*/
  dwand = NewDrawingWand();

  DrawSetFontSize( dwand, font->font_size );

  if ( textline->nrfragments == 0 )
    {
      /* set standard font */
      DrawSetFont( dwand, font->font_name ) ;

      fm = MagickQueryFontMetrics( current_image->im, dwand, "Wym");
      height = fm[5];
    }
  else
    {
      for (i=0;i<textline->nrfragments;i++)
	      {
	        if ( textline->fragments[i]->size_override == -1 )
	          DrawSetFontSize( dwand, font->font_size );
	        else
	          DrawSetFontSize( dwand, textline->fragments[i]->size_override );
	        switch( textline->fragments[i]->fontnr )
	          {
	            case 0:   /* regular */
                output( 2, "set font: %s\n", font->font_name );
	              DrawSetFont( dwand, font->font_name ) ;
	              break;
	            case 1:   /* bold */
	              if ( font->bold_name != NULL )
		              DrawSetFont( dwand, font->bold_name ) ;
	              else
		              DrawSetFont( dwand, font->font_name ) ;
	              break;
	            case 2:   /* italics */
	              if ( font->italic_name != NULL )
		              DrawSetFont( dwand, font->italic_name ) ;
	              else
		              DrawSetFont( dwand, font->font_name ) ;
	              break;
	            case 3:   /* bolditalics */
	              if ( font->bolditalic_name != NULL )
		              DrawSetFont( dwand, font->bolditalic_name ) ;
	              else
		              DrawSetFont( dwand, font->font_name ) ;
	              break;
	          }
	        fm = MagickQueryFontMetrics( current_image->im,
				          dwand,
				          textline->fragments[i]->words );
          if ( fm == NULL )
          {
            output( 2, "MagickQueryFontMetrics failed!\n");
          }
          else
          {
	          textline->fragments[i]->fm_width = fm[4];
	          output( 2, ">%s< w=%f h=%f\n", textline->fragments[i]->words, fm[4], fm[5] );
  	        width += fm[4];
	          if ( fm[5] > height )
	            height = fm[5];
	          if ( fm[2] > asc_height )
	            asc_height = fm[2];
	          if ( fabs( fm[5] ) > sep )
	            sep = fabs( fm[5] );
          }
	      }
    }

  if ( fm != NULL )
    free( fm );

  /* release drawing wand */
  DestroyDrawingWand( dwand );

  /* adjust the font separator */
  if ( font->font_separator != -1 )
    sep = font->font_separator;

  output( 2, "line netrics: w=%i pix  h=%i pix\n", width, height );
  textline->fm_width = width;
  textline->fm_height = height + sep;
  textline->fm_asc_height = asc_height;
  textline->fm_separator = sep;
}


void magick_textfile_metrics( textfiledef *textfile, font_descr *font )
{
  int i;

  int width = 0;
  int height = 0;

  if ( ( textfile->fm_width == -1 ) && ( textfile->fm_height == -1 ) )
    {
      output( 1, "Calculating font metrics for textfile ..." );
      for (i=0;i<textfile->nrlines;i++)
	{
	  magick_textline_metrics( textfile->lines[i], font );

	  height += textfile->lines[i]->fm_height;
	  if ( textfile->lines[i]->fm_width > width )
	    width = textfile->lines[i]->fm_width;
	}
      output( 1, "Done.\n" );
      output( 1, "textfile metrics: w=%i h=%i\n", width, height );
      textfile->fm_width = width;
      textfile->fm_height = height;
    }
}




void magic_textfile_drawline( int posx,
                              int posy,
                              double alpha,
                              font_descr *font,
                              textfilelinedef *line )
{
  DrawingWand   *dwand = NULL;
  PixelWand     *pwand = NULL;

  int i;

  int x;

  if ( line->nrfragments == 0 )
    return;


  /* setup the font and image*/
  dwand = NewDrawingWand();
  pwand = NewPixelWand();


  DrawSetFontSize( dwand, font->font_size );
  PixelSetColor( pwand, font->font_color );

  // Turn antialias on - not sure this makes a difference
  DrawSetTextAntialias( dwand, MagickTrue );

  /*posy += line->fm_height;  */
  posy += line->fm_asc_height;
  x = posx;
  for (i=0;i<line->nrfragments;i++)
  {
    if ( line->fragments[i]->size_override== -1 )
	     DrawSetFontSize( dwand, font->font_size );
    else
	     DrawSetFontSize( dwand, line->fragments[i]->size_override );

    switch( line->fragments[i]->fontnr )
	  {
	    case 0:   /* regular */
	      DrawSetFont( dwand, font->font_name ) ;
	      break;
	    case 1:   /* bold */
	      if ( font->bold_name != NULL )
	        DrawSetFont( dwand, font->bold_name ) ;
	      else
	        DrawSetFont( dwand, font->font_name ) ;
	      break;
	    case 2:   /* italics */
	      if ( font->italic_name != NULL )
	        DrawSetFont( dwand, font->italic_name ) ;
	      else
	        DrawSetFont( dwand, font->font_name ) ;
	      break;
	     case 3:   /* bolditalics */
	       if ( font->bolditalic_name != NULL )
	         DrawSetFont( dwand, font->bolditalic_name ) ;
	       else
	         DrawSetFont( dwand, font->font_name ) ;
	       break;
	  }


    /* Pixel setalpha is not workgin on graphicsmagick */
    PixelSetAlpha( pwand, alpha );
    // DrawSetStrokeColor( dwand, pwand );
    DrawSetFillColor( dwand, pwand );

    DrawAnnotation(dwand, x, posy, (unsigned char*) line->fragments[i]->words );
    x += line->fragments[i]->fm_width;
  }

  // Draw the image on to the magick_wand
  MagickDrawImage( current_image->im, dwand);

  DestroyPixelWand( pwand );
  DestroyDrawingWand( dwand );
}



void magick_image_rectangle( int x1, int y1, int x2, int y2, int width, int filled,  char *color )
{
  DrawingWand   *dwand = NULL;
  PixelWand     *pwand = NULL;


  /* setup the font and image*/
  dwand = NewDrawingWand();
  pwand = NewPixelWand();


  PixelSetColor( pwand, color );

  if ( filled == 1 )
    {
      DrawSetFillColor( dwand, pwand );
    }
  else
    {
      DrawSetStrokeColor( dwand, pwand );
      DrawSetStrokeWidth( dwand, width );
    }

  DrawRectangle( dwand, x1, y1, x2, y2 );

  // Draw the image on to the magick_wand
  MagickDrawImage( current_image->im, dwand);

  DestroyPixelWand( pwand );
  DestroyDrawingWand( dwand );
}



void magick_load_imagedef( imagedef_descr *imagedef )
{
  unsigned int result;

  if ( imagedef->file_name == NULL )
    {
      output( 1, "Error: No filename given for imagedefinition!\n" );
      return;
    }

  output( 1, "Loading %s ...\n" , imagedef->file_name );
  imagedef->im = NewMagickWand();

  /* Read the input image */
  result = MagickReadImage( imagedef->im, imagedef->file_name );

  if ( result != 0 )
    {
      output( 1, "Warning: Can't read image '%s'! Skip Image!\n", imagedef->file_name );
      DestroyMagickWand( imagedef->im );
      imagedef->im = NULL;
      return;
    }

  /* get some infos */
  imagedef->width  = MagickGetImageWidth( imagedef->im );
  imagedef->height = MagickGetImageHeight( imagedef->im );

  /* rescale image */
  switch ( imagedef->need_resize )
    {
    case resize_value:
      imagedef->width  = imagedef->resize_x;
      imagedef->height = imagedef->resize_y;
      break;
    case resize_factor:
      imagedef->width  *= imagedef->resize_fac;
      imagedef->height *= imagedef->resize_fac;
      break;
    }

  // Resize the image using the Lanczos filter
  // The blur factor is a "double", where > 1 is blurry, < 1 is sharp
  // I haven't figured out how you would change the blur parameter of MagickResizeImage
  // on the command line so I have set it to its default of one.
  if ( imagedef->need_resize > resize_none )
  #if MAGICK_VERSION >= 7
    MagickResizeImage( imagedef->im,
		       imagedef->width,
		       imagedef->height,
		       LanczosFilter );
  #else
    MagickResizeImage( imagedef->im,
                       imagedef->width,
                       imagedef->height,
                       LanczosFilter,
                       1 );
  #endif

  output( 1, "Done.\n" );
}



/* image routines */

void magick_image_window( parsenode *varx1, parsenode *vary1, parsenode *varx2, parsenode *vary2 )
{
  int x1;
  int y1;
  int x2;
  int y2;

  int width, height;

  MagickWand *clone;

  x1 = math_execute_node_int( varx1 );
  y1 = math_execute_node_int( vary1 );
  x2 = math_execute_node_int( varx2 );
  y2 = math_execute_node_int( vary2 );

  width = (x2-x1);
  if ( width < 1 )
    width = 1;

  height = ( y2-y1 );
  if ( height < 1 )
    height = 1;

  clone = CloneMagickWand( current_image->im );

  MagickCropImage( clone, width, height, x1, y1 );

  /* delete old offsets ... */
  /*MagickResetImagePage( clone, "" ); */

  magick_push_image( clone, x1, y1, width, height );


  current_image->width  = MagickGetImageWidth( current_image->im);
  current_image->height = MagickGetImageHeight( current_image->im );

  /* update variables */
  magick_image_info();
}


void magick_image_endwindow( void )
{
  MagickWand *w;

  int x = 0;
  int y = 0;
  int width, height;

  w = magick_pop_image( &x, &y, &width, &height );

  // MagickCompositeImage( current_image->im, w, BlurCompositeOp, 0, 0 );
  #if MAGICK_VERSION >= 7
  MagickCompositeImage( current_image->im, w,
                        ReplaceCompositeOp,
                        MagickTrue,
                        x,
                        y );
  #else
  MagickCompositeImage( current_image->im, w,
                        ReplaceCompositeOp,
                        x,
                        y );
  #endif

  DestroyMagickWand( w );

  current_image->width  = MagickGetImageWidth( current_image->im);
  current_image->height = MagickGetImageHeight( current_image->im );

  /* update variables */
  magick_image_info();
}


void magick_image_load( filedef *file )
{
  int size;
  unsigned char *pixels;

  MagickBooleanType result;

  if ( current_image == NULL )
    {
      current_image = magick_image_new();
    }

  if ( current_image->im != NULL )
    {
      DestroyMagickWand( current_image->im );
      current_image->im = NULL;
    }

  /* check for empty images */
  switch( file->type )
    {
    case file_type_empty:
      output( 10, "Creating blank image ...\n" );
      size = file->dimx * file->dimy * 3;  /* RGB simple 8bit */
      if ( ( pixels = (unsigned char*) calloc( size, 1 ) ) != NULL )
	      {
	        current_image->im = NewMagickWand();
          result = MagickConstituteImage( current_image->im,
              file->dimx,
					    file->dimy,
					    "RGB",
					    CharPixel,
					    pixels );
	        free( pixels );
	        current_image->width  = file->dimx;
	        current_image->height = file->dimy;

	        magick_image_rectangle( 0, 0, current_image->width-1, current_image->height-1,
				      1,    /* rectangle width */
				      1,    /* filled */
				      main_project->background  );
          output( 10, "Done.\n" );
	      }
      else
      	{
	        output( 1, "Can't allocate memory for empty image! Skip Image!\n" );
	      }
      break;
    case file_type_file:
      output( 10, "Loading %s ...\n" , file->name );
      current_image->im = NewMagickWand();

      /* Read the input image */
      result = MagickReadImage( current_image->im, file->name );

      if ( result   == MagickFalse )
	      {
	        output( 1, "Warning: Can't read image '%s'! Skip Image!\n", file->name );
	        DestroyMagickWand( current_image->im );
	        current_image->im = NULL;
	      }
      else
	      {
	        current_image->width  = MagickGetImageWidth( current_image->im);
	        current_image->height = MagickGetImageHeight( current_image->im );
	      }
      output( 10, "Done.\n" );
      break;
    }

  /* update variables */
  magick_image_info();
}



void magick_text( parsenode *nposx,
		  parsenode *nposy,
		  parsenode *ntext,
		  parsenode *nfont,
		  parsenode *nalpha )
{
  font_descr      *font;

  char            *stext;
  textfilelinedef *textline;
  char            *sfont;
  double           alpha;

  int              posx, posy;


  magick_check_current_image();

  sfont = get_string_from_node( nfont );
  font = font_get_font( sfont );

  if ( font == NULL )
    {
      output( 1, "Warning: Couldn't find font '%s' in font list!Skip text command!\n",
	      sfont );
      return;
    }

  if ( nalpha != NULL )
    alpha = math_execute_node_double( nalpha );
  else
    alpha = 1.0;

  /* extract text */
  stext = math_execute_node_string( ntext );

  textline = textfile_get_textline( stext );
  magick_textline_metrics( textline, font );


  /* calculate the positions */
  posx = image_position_x( nposx, current_image->width, textline->fm_width );
  posy = image_position_y( nposy, current_image->height, textline->fm_height );


  /* draw the line */
  output( 100, "x=%i y=%i text='%s' fontname='%s'\n", posx, posy,stext, sfont );

  magic_textfile_drawline( posx, posy, alpha, font, textline );


  textfile_free_textline( textline );

  /* free variables */
  free( stext );
  free( sfont );
}


void magick_textfile( parsenode *nposx,
                      parsenode *nposy,
                      parsenode *nfont,
                      parsenode *nfilename,
                      parsenode *nalpha )
{
  font_descr    *font;

  char          *sfont;
  double         alpha;

  textfiledef   *textfile;

  int            i;

  int            startx, starty;

  magick_check_current_image();

  sfont = get_string_from_node( nfont );
  font = font_get_font( sfont );

  if ( font == NULL )
    {
      output( 1, "Warning: Couldn't find font '%s' in font list!Skip textfile command!\n",
	      sfont );
      return;
    }

  if ( nalpha != NULL )
    alpha = math_execute_node_double( nalpha );
  else
    alpha = 1.0;


  textfile = textfile_get_textfile( nfilename );
  if ( textfile != NULL )
    {
      /* calculate text metrics */
      magick_textfile_metrics( textfile, font );

      /* y coordinates has to be calculated first */
      starty = image_position_y( nposy, current_image->height, textfile->fm_height );

      for (i=0;i<textfile->nrlines;i++)
	{
	  startx = image_position_x( nposx, current_image->width, textfile->lines[i]->fm_width );
	  magic_textfile_drawline( startx, starty, alpha, font, textfile->lines[i] );
	  starty += textfile->lines[i]->fm_height;
	}
    }
  else
    {
      output( 1, "Warning: textfile block invalid!Skip textfile command!\n" );
    }
}


void magick_basic_resize( int keep_aspect )
{
  int width,height;
  double ratio;

  magick_check_current_image();

  if ( keep_aspect == 1 )
    {
      /* Get the image's width and height */
      width = current_image->width;
      height = current_image->height;

      ratio = (double) width / (double) height;

      if ( height > main_project->geometry[1] )
	{
	  height = main_project->geometry[1];
	  width = floor( height * ratio );
	}

      if ( width > main_project->geometry[0] )
	{
	  width = main_project->geometry[0];
	  height = floor( width / ratio );
	}
    }
  else
    {
      width  = main_project->geometry[0];
      height = main_project->geometry[1];
    }

  // Resize the image using the Lanczos filter
  #if MAGICK_VERSION >= 7
  MagickResizeImage( current_image->im, width, height, LanczosFilter );
  #else
  MagickResizeImage( current_image->im, width, height, LanczosFilter, 1 );
  #endif

  current_image->width = width;
  current_image->height = height;

  /* update variables */
  magick_image_info();
}


void magick_resize_extent( void )
{
  PixelWand *p_wand;
  int        width, height;
  int        sx, sy;

  magick_check_current_image();

  sx = main_project->geometry[0];
  sy = main_project->geometry[1];

  width = MagickGetImageWidth( current_image->im );
  height = MagickGetImageHeight( current_image->im );

  if ( ( sx == width ) && ( sy == height ) )
    return;

  /* Create a wand */
  p_wand = NewPixelWand();

  // Change this to whatever colour you like - e.g. "none"
  PixelSetColor(p_wand, main_project->background );

  MagickSetImageBackgroundColor( current_image->im, p_wand );

  /* This centres the original image on the new canvas.
     Note that the extent's offset is relative to the
     top left corner of the *original* image, so adding an extent
     around it means that the offset will be positive (in
     older versions of ImageMagick it was negative!)
     since 6.7.9.10-4 it is negative again!
  */
  MagickExtentImage( current_image->im, sx, sy,
  		     (width-sx)/2, (height-sy)/2 );

  /* cleanup the wands */
  DestroyPixelWand( p_wand );


  current_image->width = sx;
  current_image->height = sy;

  /* update variables */
  magick_image_info();
}


void magick_resize( int keep_aspect )
{
  magick_basic_resize( keep_aspect );
  magick_resize_extent();
}


void magick_drawimage( parsenode *nposx,
		       parsenode *nposy,
		       parsenode *nimagename )
{
  char           *imagename;

  imagedef_descr *imagedef;

  int             posx, posy;

  magick_check_current_image();

  imagename = get_string_from_node( nimagename );
  imagedef = imagedef_get_imagedef( imagename );

  if ( imagedef == NULL )
    {
      output( 1, "Error: Can't find imagedef! Drawimage aborted!\n" );
      return;
    }

  if ( imagedef->im == NULL )
    {
      magick_load_imagedef( imagedef );
    }
  else
    {
      output( 1, "Reusing image '%s'!\n", imagedef->file_name );
    }

  /* calculate the positions */
  posx = image_position_x( nposx, current_image->width, imagedef->width );
  posy = image_position_y( nposy, current_image->height, imagedef->height );

  #if MAGICK_VERSION >= 7
  MagickCompositeImage( current_image->im,
			imagedef->im,
			imagedef->composite_operator,
                        MagickTrue,
			posx, posy );
  #else
  MagickCompositeImage( current_image->im,
                        imagedef->im,
                        imagedef->composite_operator,
                        posx, posy );
  #endif
}



void magick_fade( void )
{
  PixelWand *pw = NULL;
  PixelWand *pwo = NULL;

  magick_check_current_image();

  pw = NewPixelWand();
  PixelSetColor(pw,"grey70");
  // It seems that this must be a separate pixelwand for Colorize to work!
  pwo = NewPixelWand();
  // AHA .. this is how to do a 60% colorize
  PixelSetColor(pwo,"rgb(60%,60%,60%)");
  MagickColorizeImage( current_image->im ,pw,pwo);
}
