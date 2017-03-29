/* (C) Copyright 2010-2017 by Oliver Cordes
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


/* filldef.c

  written by: Oliver Cordes 2017-03-04
  changed by: Oliver Cordes 2017-03-29

  $Id$

*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <assert.h>

#include "config.h"

#if MAGICK_VERSION >= 7
#include <MagickWand/MagickWand.h>
#else
#include <wand/magick_wand.h>
#endif

#include "abort.h"
#include "helpers.h"
#include "emoji.h"
#include "filldef.h"
#include "output.h"
#include "parsetree.h"
#include "project.h"


#define          new_increment_pendefs 5
pendef_descr   **pendefs = NULL;
int              nr_pendefs = 0;
int              max_pendefs = 0;
pendef_descr    *current_pendef = NULL;



/* internal functions */

pendef_descr *filldef_new_pendef( void )
{
  int              i;
  pendef_descr **new_pendefs;
  pendef_descr  *new_pendef;

  if ( nr_pendefs == max_pendefs )
  {
    max_pendefs += new_increment_pendefs;
    new_pendefs =  malloc( sizeof( pendef_descr* ) * max_pendefs );
    if ( new_pendefs == NULL ) aabort( abort_msg_malloc, "pendef_new_pendef(new_pendefs_array)" );

    for (i=0;i<nr_pendefs;++i)
      new_pendefs[i] = pendefs[i];

    nfree( pendefs );
    pendefs = new_pendefs;
  }
  new_pendef = malloc( sizeof( pendef_descr ) );
  if ( new_pendef == NULL ) aabort( abort_msg_malloc, "new_pendef" );

  new_pendef->name      = NULL;
  new_pendef->color     = NULL;
  new_pendef->size      = 0;
  new_pendef->fillcolor = NULL;

  pendefs[nr_pendefs] = new_pendef;
  nr_pendefs++;

  return new_pendef;
}


void filldef_free_pendef( void )
{
  int i;

  for (i=0;i<nr_pendefs;++i)
  {
    nfree( pendefs[i]->name );
    nfree( pendefs[i]->color );
    nfree( pendefs[i]->fillcolor );
    nfree( pendefs[i] );
  }
  nfree( pendefs );
}



/* module init/done functions */
void filldef_init( void )
{

}


void filldef_done( void )
{
  filldef_free_pendef();
}


void filldef_print_defs( void )
{
  int i;

  output( 1, "pendef descriptions: nr. of pens: %u\n", nr_pendefs );
  if ( nr_pendefs > 0 )
  {
    output( 1, " %-25s %-10s %-20s %-20s\n", "Name", "Size", "Color", "FillColor" );
    for (i=0;i<nr_pendefs;++i)
      output( 1, " %-25s %-10i %-20s %-20s\n",
              pendefs[i]->name,
              pendefs[i]->size,
              pendefs[i]->color,
              pendefs[i]->fillcolor );
  }
}


void pendef_start( parsenode *n )
{
  char           *sname = NULL;

  sname = get_string_from_node( n );

  output( 1, "Starting a new pendef '%s'\n", sname );
  current_pendef       = filldef_new_pendef();
  current_pendef->name = sname;

  free_node( n );
}


void pendef_end( void )
{
  assert( current_pendef != NULL );

  output( 1, "Finishing pendef!%s\n", _em[_em_okay] );
  current_pendef = NULL;
}

void pendef_set_color( parsenode *color  )
{
  assert( current_pendef != NULL );

  current_pendef->color = get_string_from_node( color );
  free_node( color );
}


void pendef_set_size( parsenode *size )
{
  assert( current_pendef != NULL );

  current_pendef->size = get_int_from_node( size );
  free_node( size );
}


void pendef_set_fillcolor( parsenode *fillcolor )
{
  assert( current_pendef != NULL );

  current_pendef->fillcolor = get_string_from_node( fillcolor );
  free_node( fillcolor );
}


pendef_descr *get_pendef_from_string( char *pen_name )
{
  pendef_descr  *pendef = NULL;
  int           i;

  if ( nr_pendefs > 0 )
  {
    for (i=0;i<nr_pendefs;++i)
    {
      if ( strcmp( pendefs[i]->name, pen_name ) == 0 )
      {
        pendef = pendefs[i];
        break;
      }
    }
    if ( pendef == NULL )
    {
      output( 1, "pendef structure for name '%s' not found!\n", pen_name );
    }
  }

  return pendef;
}


pendef_descr *get_pendef_from_node( parsenode *node )
{
  char              *pen_name;

  pendef_descr      *pendef = NULL;

  pen_name = get_string_from_node( node );
  output( 2, "pendef name: %s\n", pen_name );

  pendef = get_pendef_from_string( pen_name );

  nfree( pen_name );

  return pendef;
}


constant *get_pendef_property( char* pen_name, char* element )
{
  pendef_descr *pendef;
  constant     *con = NULL;

  pendef = get_pendef_from_string( pen_name );
  if ( pendef != NULL )
  {
    if ( strcmp( element, "color") == 0 )
      con = add_constant_string( pendef->color );
    if ( strcmp( element, "size") == 0 )
      con = add_constant_int( pendef->size );
    if ( strcmp( element, "fillcolor" ) == 0 )
      con = add_constant_string( pendef->fillcolor );

    if ( con == NULL )
    {
      output( 1, "pendef description has no element '%s'\n", element );
      con = add_constant_string( element );
    }
  }

  return con;
}
