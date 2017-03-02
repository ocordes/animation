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


/* type_array.c

written by: Oliver Cordes 2017-02-25
changed by: Oliver Cordes 2017-03-01

$Id$

*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "config.h"

#include "amath.h"
#include "output.h"
#include "parsetree.h"
#include "type_array.h"
#include "variable.h"


/* internal functions */
constant * new_array_constant( void )
{
  constant *con;

  con = new_constant();
  con->type = constant_array;
  con->a.alloc = 10;
  con->a.nr    = 0;
  con->a.cons  = malloc( 10 * sizeof( constant* ));
  con->a.type  = constant_none;

  return con;
}

void free_array_constant( constant *con )
{
  int i;

  for (i=0;i<con->a.nr;++i)
    free( con->a.cons[i] ) ;
  free( con->a.cons );
}

void clone_array_constant( constant* newcon, constant* con )
{
  int i;

  newcon->a.cons = malloc( sizeof( constant* ) * con->a.alloc );

  for (i=0;i<con->a.nr;++i)
    newcon->a.cons[i] = clone_constant( con->a.cons[i] );
}


void add_array_element( constant *con, constant *el )
{
  constant **p = NULL;
  int        i;
  char      *s;

  s = constant2str( el );
  output( 2, "add_array_element: %s\n", s );
  free( s );

  if ( con->a.type == constant_none )
  {
    con->a.type = el->type;
  }
  else
  {
    if ( con->a.type != el->type )
    {
      output( 1, "Cannot add element to array, wrong type!\n" );
      return;
    }
  }

  if ( con->a.nr == con->a.alloc )
  {
    con->a.alloc += 10;
    p = malloc( sizeof( constant* ) * con->a.alloc );

    for (i=0;i<con->a.nr;++i)
      p[i] = con->a.cons[i];
    free( con->a.cons );
    con->a.cons = p;
  }
  con->a.cons[con->a.nr] = el;
  con->a.nr++;
}


/* aaray 2 string conversion */
char *array2str( constant *con )
{
  char  dummy[1000] = "\0";
  char *s;
  int   i;

  for (i=0;i<con->a.nr;++i)
  {
    s = constant2str( con->a.cons[i] );
    if ( i != 0 )
    {
      strncat( dummy, ",", 1000 );
    }
    strncat( dummy, s, 1000 );
    free( s );
  }
  s = malloc( strlen( dummy ) + 3 );
  sprintf( s, "[%s]", dummy );
  return s;
}

/* array node definitions */
parsenode *add_node_array( parsenode *list )
{
  parsenode *newnode;

  newnode = new_node();

  newnode->type  = node_array;

  newnode->left  = list;

  return newnode;
}


parsenode *add_node_array_list( parsenode *array_list, parsenode *element )
{
  parsenode *newnode;

  newnode = new_node();

  newnode->type  = node_array_list;

  newnode->left  = element;
  newnode->right = array_list;

  return newnode;
}

/* node to constand conversion */
constant *add_constant_array( parsenode *node )
{
  constant *con;
  constant *el;

  parsenode *n;

  output( 2, "add_constant_array\n");
  con = new_array_constant();

  n = node;
  while( n != NULL )
  {
    el = math_execute_node( n->left );
    add_array_element( con, el );
    n = n->right;
  }

  return con;
}

/* math functions */
constant *math_evaluate_array( constant *left,
                               constant *right,
                               int mathop )
{
  constant *con;
  int       i;

  output( 2, "nath_evaluate_array called\n" );
  switch( left->a.type )
  {
    case constant_int:
    case constant_double:
      /* element wise math operation */
      for (i=0;i<left->a.nr;++i)
      {
        con = math_evaluate_node( left->a.cons[i], right, mathop );
        ree( left->a.cons[i] ); 
        left->a.cons[i] = con;
      }
      break;
    default:
      output( 1, "Math operation on this type of array is not allowed!\n");
      return left;
      break;
  }

  return left;
}


constant *math_evaluate_array_func( constant *left, int mathop )
{
  return left;
}
