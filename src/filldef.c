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
  changed by: Oliver Cordes 2017-03-07

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

#define          new_increment_filldefs 5
filldef_descr  **filldefs = NULL;
int              nr_filldefs = 0;
int              max_filldefs = 0;
filldef_descr   *current_filldef = NULL;


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

  new_pendef->name = NULL;
  new_pendef->color = NULL;
  new_pendef->size = 0;

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
    nfree( pendefs[i] );
  }
  nfree( pendefs );
}


filldef_descr *filldef_new_filldef( void )
{
  int              i;
  filldef_descr **new_filldefs;
  filldef_descr  *new_filldef;

  if ( nr_filldefs == max_filldefs )
  {
    max_filldefs += new_increment_filldefs;
    new_filldefs =  malloc( sizeof( filldef_descr* ) * max_filldefs );
    if ( new_filldefs == NULL ) aabort( abort_msg_malloc, "filldef_new_filldef(new_filldefs_array)" );

    for (i=0;i<nr_filldefs;++i)
      new_filldefs[i] = filldefs[i];

    nfree( filldefs );
    filldefs = new_filldefs;
  }
  new_filldef = malloc( sizeof( filldef_descr ) );
  if ( new_filldef == NULL ) aabort( abort_msg_malloc, "new_filldef" );

  new_filldef->name = NULL;

  filldefs[nr_filldefs] = new_filldef;
  nr_filldefs++;

  return new_filldef;
}


void filldef_free_filldef( void )
{
  int i;

  for (i=0;i<nr_filldefs;++i)
  {
    nfree( filldefs[i]->name );
    nfree( filldefs[i] );
  }
  nfree( filldefs );
}

/* module init/done functions */
void filldef_init( void )
{

}


void filldef_done( void )
{
  filldef_free_pendef();
  filldef_free_filldef();
}


void filldef_print_defs( void )
{
  int i;

  output( 1, "pendef descriptions: nr. of pens: %u\n", nr_pendefs );
  if ( nr_pendefs > 0 )
  {
    output( 1, " %-40s %-10s %-20s\n", "Name", "Size", "Color" );
    for (i=0;i<nr_pendefs;++i)
      output( 1, " %-40s %-10i %-20s\n", pendefs[i]->name , pendefs[i]->size, pendefs[i]->color );
  }

  output( 1, "filldef descriptions: nr. of fill pattern: %u\n", nr_filldefs );
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


void filldef_start( parsenode *n )
{
  char           *sname = NULL;

  sname = get_string_from_node( n );

  output( 1, "Starting a new filldef '%s'\n", sname );
  current_filldef       = filldef_new_filldef();
  current_filldef->name = sname;

  free_node( n );
}


void filldef_end( void )
{
  assert( current_filldef != NULL );

  output( 1, "Finishing filldef!%s\n", _em[_em_okay] );
  current_filldef = NULL;
}
