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
changed by: Oliver Cordes 2017-04-02

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

parsenode *add_node_array_element( parsenode *element )
{
  parsenode *newnode;

  newnode = new_node();

  newnode->type  = node_array_element;
  newnode->left  = element;

  return newnode;
}


parsenode *add_node_array_elements( parsenode *start, parsenode *end )
{
  parsenode *newnode;

  newnode = new_node();

  newnode->type  = node_array_elements;
  newnode->left  = start;
  newnode->right = end;

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

constant *math_evaluate_array_dir( constant *left,
                                    constant *right,
                                    int mathop,
                                    int direction )
{
  constant *con;
  constant *conr;
  int       i;

  output( 2, "nath_evaluate_array_dir called\n" );
  switch( left->a.type )
  {
    case constant_int:
    case constant_double:
    case constant_point:
      /* element wise math operation */
      for (i=0;i<left->a.nr;++i)
      {
        conr = clone_constant( right );
        if ( direction == 0 )
          con = math_evaluate_node( left->a.cons[i], conr, mathop );
        else
          con = math_evaluate_node( conr, left->a.cons[i], mathop );
        /* free( left->a.cons[i] ); */
        left->a.cons[i] = con;
      }
      free_constant( right );
      break;
    default:
      output( 1, "Math operation on this type is for arrays not allowed!\n");
      return left;
      break;
  }

  return left;
}


constant *math_evaluate_array( constant *left,
                               constant *right,
                               int mathop )
{
  output( 2, "nath_evaluate_array called\n" );
  if ( left->type == constant_array )
    return math_evaluate_array_dir( left, right, mathop, 0 );
  else
    return math_evaluate_array_dir( right, left, mathop, 1 );

  return left;
}


constant *math_evaluate_array_func( constant *left, int mathop )
{
  return left;
}


constant *math_execute_node_array( parsenode *node )
{
  constant *con;

  con = math_execute_node( node );

  if ( con->type != constant_array )
  {
    output( 1, "Array constant expected! (type of constant= %i)!\n", con->type );

    output( 1, "Returning zero size array!\n");

    free_constant( con );

    con = new_array_constant();
  }

  return con;
}

constant *math_execute_array_element( parsenode *node, constant *val )
{
  constant *con = val;
  constant *element;
  int       i;

  if ( val->type != constant_array )
  {
    output( 1, "Array constant expected! (type of constant= %i)!\n", val->type );
    output( 1, "Returning original variable!\n" );

    return clone_constant( val );
  }

  element = math_execute_node( node );

  if ( ( element->type == constant_int) || ( element->type == constant_double ) )
  {
    i = get_int_from_constant( element );
    output( 10, "index i=%i\n", i );
    if ( i > val->a.nr  )
    {
      output( 1, "Element nr=%i is not in array!\n", i );
      output( 1, "Returning original variable!\n" );

      return clone_constant( val );
    }

    con = clone_constant( val->a.cons[i] );
    /*con = clone_constant( val ); */
  }

  return con;
}

constant *math_execute_array_elements( parsenode *node, constant *val )
{
  constant *con;

  constant *start;
  constant *end;

  int      istart;
  int      iend;

  int      i;

  if ( val->type != constant_array )
  {
    output( 1, "Array constant expected! (type of constant= %i)!\n", val->type );
    output( 1, "Reurning original variable!\n" );

    return clone_constant( val );
  }

  if ( val->a.nr == 0 )
  {
    output( 1, "Array is empty! Returning zero variable!\n" );
    return clone_constant( val );
  }

  if ( node->left != NULL )
  {
    start = math_execute_node( node->left );
    istart = get_int_from_constant( start );
    free_constant( start );
    if ( istart < 0 )
    {
      output( 1, "Negative array indices are not allowed!\n");
      istart = 0;
    }
    else
    {
      if ( istart >= val->a.nr )
      {
        output( 1, "Start index is higher than number of elements! (start=%i last_index=%i)\n",
          istart, val->a.nr-1 );
        istart = val->a.nr - 1;
      }
    }
  }
  else
    istart = 0;

  if ( node->right != NULL )
  {
    end   = math_execute_node( node->right );
    iend  = get_int_from_constant( end );
    free_constant( end );
    if ( iend >= val->a.nr )
    {
      output( 1, "Array index too high (%i >= %i )!\n", iend, val->a.nr-1 );
      iend = val->a.nr-1;    /* C notation number of elements -1 is the last index ;-) */
    }
  }
  else
    iend = val->a.nr-1;

  output( 10, "array get elements: start=%i end=%i\n", istart, iend );
  con = new_array_constant();
  if ( istart <= iend )
  {
    for (i=istart;i<=iend;++i)
      add_array_element( con, val->a.cons[i] );
  }
  else
  {
    output( 1, "Array ranges are wrong (start=%i. end=%i)! Returning Zero Array!\n", istart, iend );
  }


  return con;
}


int set_constant_variable_array_element( variable *var, constant *element, constant *con )
{
  int i;

  if ( var->con.type != constant_array )
  {
    output( 1, "Array constant expected! (type of constant= %i)!\n", var->con.type );
    return -1;
  }

  if ( ( element->type != constant_int) && ( element->type != constant_double ) )
  {
    output( 1, "Index element should be double or integer (type of index=%i)!\n", element->type );
    return -1;
  }

  i = get_int_from_constant( element );
  output( 10, "index i=%i\n", i );
  if ( i > var->con.a.nr  )
  {
    output( 1, "Element nr=%i is not in array!\n", i );
    return -1;
  }

  free_constant( var->con.a.cons[i] );
  var->con.a.cons[i] = clone_constant( con );

  return 0;
}
