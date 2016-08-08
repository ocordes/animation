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

/* font.c

   written by: Oliver Cordes 2012-10-29
   changed by: Oliver Cordes 2013-03-03

   $Id: font.c 687 2014-09-14 17:53:49Z ocordes $

*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <assert.h>

#include "config.h"

#include "abort.h"
#include "font.h"
#include "helpers.h"
#include "output.h"
#include "parsetree.h"
#include "project.h"


#define       new_increment_fonts 5
font_descr **fonts;
int          nr_fonts;
int          max_fonts;

font_descr  *current_font = NULL;


/* module funtions */

void font_print_fonts( void )
{
  int i;

  output( 1, "font descriptions:  nr. of fonts: %i\n", nr_fonts );
  for (i=0;i<nr_fonts;i++)
    {
      output( 1, "  NAME: %s\n", fonts[i]->name );
      if ( fonts[i]->font_name != NULL )
	output( 1, "   font        : %s\n", fonts[i]->font_name );
      if ( fonts[i]->bold_name != NULL )
	output( 1, "   bold        : %s\n", fonts[i]->bold_name );
      if ( fonts[i]->italic_name != NULL )
	output( 1, "   italics     : %s\n", fonts[i]->italic_name );
      if ( fonts[i]->bolditalic_name != NULL )
	output( 1, "   bolditalics : %s\n", fonts[i]->bolditalic_name ); 
      output( 1, "   size        : %i pixels\n", fonts[i]->font_size );
      if ( fonts[i]->font_color != NULL )
	output( 1, "   color       : %s\n", fonts[i]->font_color );
    }
}


font_descr *font_new_font( char *fontname )
{
  int          i;
  font_descr **new_fonts;
  font_descr  *new_font;

  if ( nr_fonts == max_fonts )
    {
      max_fonts += new_increment_fonts;
      new_fonts =  malloc( sizeof( font_descr* ) * max_fonts );
      if ( new_fonts == NULL ) aabort( abort_msg_malloc, "font_new_font(new_fonts_array)" );
      
      for (i=0;i<nr_fonts;i++)
	new_fonts[i] = fonts[i];

      free( fonts );
      fonts = new_fonts;
    }
  new_font = malloc( sizeof( font_descr ) );
  if ( new_font == NULL ) aabort( abort_msg_malloc, "new_font" );

  new_font->name            = strdup( fontname );
  new_font->font_name       = NULL;
  new_font->bold_name       = NULL;
  new_font->italic_name     = NULL;
  new_font->bolditalic_name = NULL;
  new_font->font_size       = 0;
  new_font->font_color      = NULL;
  new_font->font_separator  = -1;

  fonts[nr_fonts] = new_font;
  nr_fonts++;

  return new_font;
}


font_descr *font_find_font( char *name )
{
  int i;

  for (i=0;i<nr_fonts;i++)
    if ( strcmp( fonts[i]->name, name ) == 0 )
      return fonts[i];

  return NULL;
}

/* exported funtions */


font_descr *font_get_font( char *name )
{
  return font_find_font( name );
}


void font_start( parsenode *name )
{
  font_descr *new_font;
  char       *sname = NULL;

  sname =  get_string_from_node( name );

  new_font = font_find_font( sname );
  if ( new_font != NULL )
    {
      output( 1, "Warning: font '%s' is already defined! Skip definition!\n", sname );
      current_font = NULL;
    }
  else
    {
      new_font = font_new_font( sname );
      current_font = new_font;

      printf( "starting a new fontdef '%s' \n", sname );
    }

  nfree( sname );
  free_node( name );
}


void font_end( void )
{
  if ( current_font != NULL )
    {
      printf( "finishing fontdef\n" );
      current_font = NULL;
    }
}


void font_set_font( parsenode *font )
{
  char *sfont = NULL;

  assert( current_font != NULL );

  sfont = get_string_from_node( font );
  
  nfree( current_font->font_name );
  current_font->font_name = strdup( sfont );

  nfree( sfont );
  free_node( font );
}


void font_set_bold( parsenode *bold )
{
  char *sbold = NULL;

  assert( current_font != NULL );

  sbold = get_string_from_node( bold );
  
  nfree( current_font->bold_name );
  current_font->bold_name = strdup( sbold );

  nfree( sbold );
  free_node( bold );
}


void font_set_italic( parsenode *italic )
{
  char *sitalic = NULL;

  assert( current_font != NULL );

  sitalic = get_string_from_node( italic );
  
  nfree( current_font->italic_name );
  current_font->italic_name = strdup( sitalic );

  nfree( sitalic );
  free_node( italic );
}


void font_set_bolditalic( parsenode *bolditalic )
{
  char *sbolditalic = NULL;

  assert( current_font != NULL );

  sbolditalic = get_string_from_node( bolditalic );

  nfree( current_font->bolditalic_name );
  current_font->bolditalic_name = strdup( sbolditalic );

  nfree( sbolditalic );
  free_node( bolditalic );
}


void font_set_size( parsenode *size )
{
  int isize;

  if ( current_font != NULL )
    {
      isize = get_int_from_node( size );
      current_font->font_size = isize;
    }

  free_node( size );
}


void font_set_color( parsenode *color )
{
  char *scolor = NULL;

  if ( current_font != NULL )
    {
      scolor = get_string_from_node( color );
      if ( current_font->font_color != NULL )
	free( current_font->font_color );
      current_font->font_color = strdup( scolor );
    }

  nfree( scolor );
  free_node( color );
}


void font_set_separator( parsenode *sep )
{
  int isep;

  if ( current_font != NULL )
    {
      isep = get_int_from_node( sep );
      current_font->font_separator = isep;
    }

  free_node( sep );
}



/* init/done funtions */

/* module funtions */

void font_init( void )
{
  font_descr *new_font;

  max_fonts = new_increment_fonts;

  fonts = malloc( sizeof( font_descr* ) * max_fonts );

  if ( fonts == NULL ) aabort( abort_msg_malloc, "font_init(new_fonts_array)" );
  nr_fonts = 0;

  /* add a default font */
  new_font = font_new_font( "default" );
  new_font->font_name = strdup( "Verdana-Bold" );
  new_font->italic_name = NULL;
  new_font->bolditalic_name = NULL;
  new_font->font_size = 12;
  new_font->font_color = strdup( "white" );
  new_font->font_separator = -1;
}


void font_done( void )
{
  int i;

  for (i=0;i<nr_fonts;i++)
    {
      nfree( fonts[i]->name );
      nfree( fonts[i]->font_name );
      nfree( fonts[i]->bold_name );
      nfree( fonts[i]->italic_name );
      nfree( fonts[i]->bolditalic_name ); 
      nfree( fonts[i]->font_color );
      free( fonts[i] );
    }
  free( fonts );
  fonts = NULL;
}
