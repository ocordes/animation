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

/* variable.c

   written by: Oliver Cordes 2010-08-08
   changed by: Oliver Cordes 2017-02-27

   $Id$

*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <assert.h>

#include "abort.h"
#include "helpers.h"
#include "output.h"
#include "variable.h"
#include "type_array.h"
#include "type_point.h"


char sTRUE[]  = "True";
char sFALSE[] = "False";

/* constant functions */

constant *new_constant( void )
{
  constant *newcon;

  newcon = (constant*) malloc( sizeof( constant ) );

  if ( newcon == NULL ) aabort( abort_msg_malloc, "new_constant" );

  newcon->type = constant_none;

  return newcon;
}


void free_constant_static( constant con )
{
  switch( con.type )
    {
    case constant_string:
      free( con.s );
      break;
    case constant_array:
      free_array_constant( &con );
      break;
    case constant_point:
    case constant_none:
    case constant_int:
    case constant_double:
    case constant_bool:
    default:
      break;
    }
}


void free_constant( constant *con )
{
  if ( con != NULL )
    {
      free_constant_static( *con );
      free( con );
    }
}


void clone_constant_static( constant *newcon, constant *con )
{
  char *s;

  s = constant2str( con );
  output( 11, "clone_constant_static called: %s\n", s );
  free( s );

  memcpy( newcon, con, sizeof( constant ) );
  switch( con->type )
  {
    case constant_string:
      newcon->s = strdup( con->s );
      break;
    case constant_array:
      clone_array_constant( newcon, con );
      break;
  }
}


constant *clone_constant( constant *con )
{
  constant *newcon;

  newcon = new_constant();

  clone_constant_static( newcon, con );

  return newcon;
}


constant *add_constant_int( int i )
{
  constant *con;

  con = new_constant();

  if ( con != NULL )
    {
      con->type = constant_int;
      con->i = i;
      output( 2, "add_constant_int=%i\n", i );
    }

  return con;
}


constant *add_constant_double( double d )
{
  constant *con;

  con = new_constant();

  if ( con != NULL )
    {
      con->type = constant_double;
      con->d = d;
      output( 2, "add_constant_double=%f\n", d );
    }

  return con;
}


constant *add_constant_string( char *s )
{
  constant *con;

  con = new_constant();

  if ( con != NULL )
    {
      con->type = constant_string;
      con->s = strdup( s );
      output( 2, "add_constant_string=%s\n", s );
    }

  return con;
}


constant *add_constant_bool( int b )
{
  constant *con;

  con = new_constant();

  if ( con != NULL )
    {
      con->type = constant_bool;
      con->b = b;
      output( 2, "add_constant_bool=%s\n", (b?"True":"False") );
    }

  return con;
}



char     *print_bool( int b )
{
  if ( b == 1 )
    return sTRUE;
  else
    return sFALSE;
}

/* variable functions */

variables *new_variables( void )
{
  variables *new_variable;

  new_variable = malloc( sizeof( variables ) );

  if ( new_variable == NULL ) aabort( abort_msg_malloc, "new_variables" );

  new_variable->vars = malloc( increment_vars * sizeof( variable ) );

  if ( new_variable->vars == NULL ) aabort( abort_msg_malloc, "new_variables(vars)" );

  new_variable->nrvars  = 0;
  new_variable->maxvars = increment_vars;
  new_variable->flags   = variable_flag_read;

  return new_variable;
}



void free_variables( variables *vars)
{
  int i;

  if ( vars == NULL ) return;

  if ( vars->vars != NULL )
    {
      for (i=0;i<vars->nrvars;i++)
	      {
	        nfree( vars->vars[i].name );
	        free_constant_static( vars->vars[i].con );
	      }
      free( vars->vars );
    }

  free( vars );
}


void print_variables( variables *vars )
{
  int i;

  output( 1, " nr. of variables: %i\n", vars->nrvars );

  for (i=0;i<vars->nrvars;i++)
    switch( vars->vars[i].con.type )
      {
      case constant_none:
	     output( 1, "  %-40s : NONE\n", vars->vars[i].name );
	     break;
      case constant_int:
	     output( 1, "  %-40s : INT\n", vars->vars[i].name );
	     break;
      case constant_double:
	     output( 1, "  %-40s : DOUBLE\n", vars->vars[i].name );
	     break;
      case constant_string:
	     output( 1, "  %-40s : STRING\n", vars->vars[i].name );
       break;
      case constant_point:
       output( 1, "  %-40s : POINT\n", vars->vars[i].name );
	     break;
      }
}


int find_variable( variables *vars, char *name )
{
  int i;
  int found = 0;

  for (i=0;i<vars->nrvars;i++)
    if ( strcmp( name, vars->vars[i].name ) == 0 )
      {
	found = 1;
	break;
      }

  if ( found == 1 )
    return i;
  else
    return -1;
}


void add_variable( variables *vars, char *name, int type )
{
  int       i;
  int       nr;
  variable *newvars;

  assert( vars != NULL );

  nr = find_variable( vars, name );

  if ( nr != -1 ) return;

  if ( vars->nrvars == vars->maxvars )
    {
      vars->maxvars += increment_vars;
      newvars = malloc( vars->maxvars * sizeof( variable ) );
      if ( newvars == NULL ) aabort( abort_msg_malloc, "new_variable_array" );
      for (i=0;i<vars->nrvars;i++)
	newvars[i] = vars->vars[i];
      free( vars->vars );
      vars->vars = newvars;
    }


  output( 10, "name=%s type=%i\n", name, type );
  vars->vars[vars->nrvars].name = strdup( name );
  vars->vars[vars->nrvars].con.type = type;

  switch( type )
    {
    case constant_int:
      vars->vars[vars->nrvars].con.i = 0;
      break;
    case constant_double:
      vars->vars[vars->nrvars].con.d = 0.0;
      break;
    case constant_string:
      vars->vars[vars->nrvars].con.s = NULL;
      break;
    }
  vars->nrvars++;
}


int set_constant_variable( variables *vars, char *name, constant *con )
{
  int nr;


  assert( vars != NULL );

  if ( con == NULL )
  {
    output( 2, "Can't set NULL constant to variable '%s'!\n", name );
    return 1;
  }

  nr = find_variable( vars, name );

  if ( nr == -1 )
    {
      output( 2, "Can't set variable '%s'!\n", name );
      return -1;
    }
  else
    {
      free_constant_static( vars->vars[nr].con );
      clone_constant_static( &vars->vars[nr].con, con );
      /*memcpy( &vars->vars[nr].con, con, sizeof( constant ) ); */
      return 0;
    }
}


void set_int_variable( variables *vars, char *name, int i )
{
  int nr;


  assert( vars != NULL );

  nr = find_variable( vars, name );

  if ( nr == -1 )
    {
      fprintf( stderr, "Can't set variable `%s`!\n", name );
    }
  else
    {
      if ( vars->vars[nr].con.type != constant_int )
	{
	  fprintf( stderr, "`%s`is not a int variable!\n", name );
	  return;
	}
      vars->vars[nr].con.i = i;
    }
}


void set_double_variable( variables *vars, char *name, double d )
{
	int nr;


	assert( vars != NULL );

	nr = find_variable( vars, name );

	if ( nr == -1 )
	{
		fprintf( stderr, "Can't set variable `%s`!\n", name );
	}
	else
	{
		if ( vars->vars[nr].con.type != constant_double )
		{
			fprintf( stderr, "`%s`is not a double variable!\n", name );
			return;
		}
		vars->vars[nr].con.d = d;
	}
}


void set_string_variable( variables *vars, char *name, char *s )
{
  int nr;


  assert( vars != NULL );

  nr = find_variable( vars, name );

  if ( nr == -1 )
    {
      fprintf( stderr, "Can't set variable `%s`!\n", name );
    }
  else
    {
      if ( vars->vars[nr].con.type != constant_string )
	{
	  fprintf( stderr, "`%s`is not a string variable!\n", name );
	  return;
	}
      if ( vars->vars[nr].con.s != NULL )
	free( vars->vars[nr].con.s );
      vars->vars[nr].con.s = strdup( s );
    }
}


constant *get_variable( variables *vars, char *name )
{
  int       nr;
  constant *con;


  assert( vars != NULL );

  nr = find_variable( vars, name );

  if ( nr == -1 ) return NULL;

  con = clone_constant( &vars->vars[nr].con );


  return con;
}


char *constant2str( constant *con )
{
  char  dummy[1000];
  char *s;

  switch( con->type )
  {
    case constant_none:
      strncpy( dummy, "NONE", 1000 );
      break;
    case constant_int:
      snprintf( dummy, 1000, "%i", con->i );
      break;
    case constant_double:
      snprintf( dummy, 1000, "%f", con->d );
      break;
    case constant_string:
      strncpy( dummy, con->s, 1000 );
      break;
    case constant_bool:
      strncpy( dummy, (con->b?"TRUE":"FALSE" ), 1000 );
      break;
    case constant_point:
      snprintf( dummy, 1000, "(%f,%f)", con->p.x, con->p.y );
      break;
    case constant_array:
      s = array2str( con );
      strncpy( dummy, s, 1000 );
      free( s );
      break;
    default:
      output( 1, "Cannot convert constant to string!\n" );
      strncpy( dummy, "NONE", 1000 );
      break;
    }

  return strdup( dummy );
}
