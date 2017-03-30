
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

/* parsetree.c

   written by: Oliver Cordes 2010-07-18
   changed by: Oliver Cordes 2017-03-19

   $Id$

*/

#include <stdio.h>
#include <stdlib.h>

#include <string.h>

#include <assert.h>

#include "abort.h"
#include "amath.h"
#include "helpers.h"
#include "output.h"
#include "parsetree.h"
#include "project.h"
#include "textfile.h"
#include "variable.h"

#define max_window_levels 10
int     window_level = 0;

/* node functions */

parsenode *new_node( void )
{
  parsenode *newnode;


  newnode = (parsenode*) malloc( sizeof( parsenode ) );

  if ( newnode == NULL ) aabort( abort_msg_malloc, "new_node" );

  newnode->type = node_none;
  newnode->next = NULL;
  newnode->left = NULL;
  newnode->right = NULL;
  newnode->nrargs = 0;
  newnode->args = NULL;

  return newnode;
}


void free_node( parsenode *node)
{
  int i;

  if ( node != NULL )
    {
      switch( node->type )
	    {
	     case node_variable:
	      nfree( node->var );
	      break;
	     case node_constant:
	      free_constant( node->con );
	      break;
	    case node_if:
	     free_node( node->cond );
	     break;
	    }
      /* release arguments */
      if ( node->nrargs > 0 )
      {
	     for (i=0;i<node->nrargs;i++)
	       free_node( node->args[i] );
	     free( node->args );
	    }

      free_node( node->left );
      free_node( node->right );
      free_node( node->next );
      free( node );
    }
}


void print_node( parsenode *node )
{
  printf( "PRINT NODE START \n" );
  if ( node == NULL )
    printf( " node  = NULL\n" );
  {
    printf( " node  = not NULL\n" );
    printf( " type  = %i\n", node->type );
    switch( node->type )
      {
      case node_variable:
	printf( "  node_variable\n" );
	printf( "   name = %s\n", node->var );
	break;
      case node_constant:
	break;
      }
    printf( " left  = %s\n", (node->left==NULL?"NULL":"not NULL" ) );
    printf( " right = %s\n", (node->right==NULL?"NULL":"not NULL" ) );
    printf( " next  = %s\n", (node->next==NULL?"NULL":"not NULL" ) );
  }
  printf( "PRINT NODE END\n" );
}


void allocate_node_args( parsenode *node, int nrargs )
{
  int i;

  node->nrargs = nrargs;
  node->args = (parsenode**) malloc( sizeof( parsenode* ) * nrargs );
  for( i=0;i<nrargs;i++)
    node->args[i] = NULL;
}


parsenode *add_node_constant_int( int i )
{
  parsenode *newnode;

  newnode = new_node();

  if ( newnode != NULL )
    {
      newnode->type = node_constant;
      newnode->con  = add_constant_int( i );
    }

  return newnode;
}


parsenode *add_node_constant_double( double d )
{
  parsenode *newnode;

  newnode = new_node();

  if ( newnode != NULL )
    {
      newnode->type = node_constant;
      newnode->con  = add_constant_double( d );
    }

  return newnode;
}


parsenode *add_node_constant_string( char *s )
{
  parsenode *newnode;

  newnode = new_node();

  if ( newnode != NULL )
    {
      newnode->type = node_constant;
      newnode->con  = add_constant_string( s );
    }

  return newnode;
}


parsenode *add_node_constant_string_arglist( parsenode *s, parsenode *arglist )
{
  parsenode *newnode;

  newnode = new_node();

  if ( newnode != NULL )
    {
      newnode->type = node_string_fmt;
      newnode->left = s;
      newnode->right = arglist;
    }

  return newnode;
}


parsenode *add_node_constant_bool( int b )
{
  parsenode *newnode;

  newnode = new_node();

  if ( newnode != NULL )
    {
      newnode->type = node_constant;
      newnode->con  = add_constant_bool( b );
    }

  return newnode;
}



parsenode *add_node_variable_r( char *s )
{
  parsenode *newnode;
  int        ps;
  int        nr;


  newnode = new_node();

  if ( newnode != NULL )
  {
    newnode->type = node_variable;
    newnode->var  = strdup( s );
  }

  /* check if the variable is already defined */

  ps = get_parser_status();

  nr = find_variable( main_project->vars, s );

  if ( nr == -1 )
    switch( ps )
    {
      case parser_status_block:
      case parser_status_window:
	      nr = find_variable( current_block->vars, s );
	      break;
      case parser_status_macro:
	      nr = find_variable( current_macro->vars, s );
	     break;
    }

  if ( nr == -1 )
    output( 1,  "Warning: variable `%s`is not defined!\n", s );


  return newnode;
}


parsenode *add_node_variable( char *s )
{
  parsenode *newnode;

  newnode = new_node();

  if ( newnode != NULL )
    {
      newnode->type = node_variable;
      newnode->var  = strdup( s );
    }

  return newnode;
}


/* just modify the variable node */
parsenode *add_node_opt_variable( parsenode *var, parsenode *opt )
{
  var->type = node_opt_variable;
  var->left = opt;

  return var;
}


void register_node_variable( parsenode *var )
{
  int        ps;
  int        nr;

  /* add variable descriptions */
  ps = get_parser_status();

  switch( ps )
  {
    case parser_status_block:
    case parser_status_window:
      nr = find_variable( main_project->vars, var->var );
      if ( nr != -1 )
	    {
	      output( 1, "Warning: variable `%s` is already defined as a global variable!\n", var->var );
	      return;
	    }
      else
	      add_variable( current_block->vars, var->var, constant_none );
      break;
    case parser_status_macro:
      nr = find_variable( main_project->vars, var->var );
      if ( nr != -1 )
	    {
	       output( 1, "Warning: variable `%s` is already defined as a global variable!\n", var->var );
	       return;
	    }
      else
	      add_variable( current_macro->vars, var->var, constant_none );
      break;
    default:
      add_variable( main_project->vars, var->var, constant_none );
      break;
    }
}



/* add_node_cmd
 *
 * adds command to the list of commands cmdlist, command can be a single
 * command or a list of commands (resulting from a simplified if condition)
 */

parsenode *add_node_arglist( parsenode *arglist, parsenode *rvalue )
{
  parsenode *newnode;

  newnode = new_node();

  newnode->type  = node_cmd_arglist;

  newnode->left  = rvalue;
  newnode->right = arglist;

  return newnode;
}


parsenode *add_node_vararglist( parsenode *vararglist, parsenode *tlvariable )
{
  parsenode *newnode;

  newnode = new_node();

  newnode->type  = node_cmd_vararglist;

  newnode->left  = tlvariable;
  newnode->right = vararglist;

  return newnode;
}



int get_node_vararglist_len( parsenode *vararglist )
{
  parsenode *varg;

  int        len = 0;

  varg = vararglist;
  while (  varg != NULL )
    {
      len++;
      varg = varg->right;
    }

  return len;
}



parsenode *add_node_cmd( parsenode *cmdlist, parsenode *command )
{
  parsenode *start;


  if ( cmdlist == NULL )
    {
      /* command list is empty ! */
      return command;
    }


  /* check for empty command */
  if ( command != NULL )
    {
      start = cmdlist;
      while( start->next != NULL ) start = start->next;

      start->next = command;
    }

  return cmdlist;
}


parsenode *add_node_window_finish( parsenode *window, parsenode *commands )
{

  if ( window == NULL )
    return NULL;

  window->left = commands;

  pop_parser_status();
  window_level--;

  return window;
}


parsenode *add_node_window( parsenode *x1, parsenode *y1, parsenode *x2, parsenode *y2 )
{
  parsenode *newnode;

  if ( window_level == max_window_levels )
    {
      output( 1, "Warning: max levels of windows reached!Ignore window command!\n" );
      return NULL;
    }
  window_level++;

  newnode = new_node();

  newnode->type = node_cmd_window;
  newnode->left = NULL;
  newnode->right = NULL;

  allocate_node_args( newnode, 4 );
  newnode->args[0] = x1;
  newnode->args[1] = y1;
  newnode->args[2] = x2;
  newnode->args[3] = y2;

  push_parser_status( parser_status_window );

  return newnode;
}




parsenode *add_node_if( parsenode *cond, parsenode *then_cmds, parsenode *else_cmds )
{
  parsenode *newnode;

  assert( cond );
  assert( then_cmds );

  if ( cond->type == node_constant )
    {
      output( 2, "add_node_if: constant detected!\n" );
      if ( cond->con->b )
	{
	  newnode = then_cmds;
	  free_node( else_cmds );
	}
      else
	{
	  newnode = else_cmds;
	  free_node( then_cmds );
	}
    }
  else
    {
      newnode = new_node();

      newnode->type = node_if;
      newnode->cond = cond;
      newnode->left = then_cmds;
      newnode->right = else_cmds;
    }

  return newnode;
}

parsenode *add_node_not( parsenode *cond )
{
	parsenode *newnode;

	assert( cond != NULL );

	output( 2, "add_node_not\n" );
	if ( cond->type == node_constant )
	{
		newnode = cond;
		newnode->con->b = ! newnode->con->b;
	}
	else
	{
		newnode = new_node();
		newnode->type = node_not;
		newnode->cond = cond;
	}

	return newnode;
}


parsenode *add_node_math( parsenode *left, parsenode *right, int mathop )
{
	parsenode *newnode;

	assert( left != NULL );
	assert( right != NULL );

	if ( ( left->type == node_constant ) && ( right->type == node_constant ) )
	{
		left->con = math_evaluate_node( left->con, right->con, mathop );

		newnode = left;
		free_node( right );
	}
	else
	{
		newnode = new_node();
		newnode->type = node_math;

		newnode->left = left;
		newnode->right = right;
		newnode->mathop = mathop;
	}

	return newnode;
}


parsenode *add_node_math_op( int mathop )
{
	parsenode *newnode;


	newnode = new_node();
	newnode->type = node_math_op;
	newnode->mathop = mathop;

	return newnode;
}


parsenode *add_node_math_func( parsenode *left, parsenode *mathfuncop )
{
	parsenode *newnode;


	assert( left != NULL );
	assert( mathfuncop->type == node_math_op );

	output( 2, "type=%i op=%i\n", left->type, mathfuncop->mathop );

	if ( left->type == node_constant )
	{
		left->con = math_evaluate_node_func( left->con, mathfuncop->mathop );
		newnode = left;
	}
	else
	{
		newnode = new_node();
		newnode->type = node_math_op;
		newnode->left = left;
		newnode->right = NULL;
		newnode->mathop = mathfuncop->mathop;
	}

	return newnode;
}



/* node extraction routines */

char   *get_string_from_constant( constant *con )
{
	static char dummy[1000];

	assert( con != NULL );

	switch( con->type )
	{
	case constant_string:
		snprintf( dummy, 1000, "%s", con->s );
		break;
	case constant_double:
		snprintf( dummy, 1000, "%f", con->d );
		break;
	case constant_int:
		snprintf( dummy, 1000, "%i", con->i );
		break;
	case constant_bool:
		snprintf( dummy, 1000, "%s", (con->b?"TRUE":"FALSE" ) );
		break;
	}

	return strdup( dummy );
}


int     get_int_from_constant( constant *con )
{
  int i = 0;

  assert( con != NULL );

  switch( con->type )
    {
    case constant_int:
      i = con->i;
      break;
    case constant_double:
      i = con->d;
      break;
    case constant_string:
      output( 1, "Warning: Converting 'string' into 'int'! Returning 'atoi(%s)'!\n", con->s );
      i = atof( con->s );
      break;
    case constant_bool:
      output( 1, "Warning: Converting 'bool' into 'int'! Returning '%i'!\n", con->b );
      i = con->b;
      break;
    default:
      output( 1, "Warning: Converting unknown type '%i' cannot be converted to int! Returning 0!\n", con->type );
      i = 0;
      break;
    }

  return i;
}


double  get_double_from_constant( constant *con )
{
  double d = 0.0;

  assert( con != NULL );

  switch( con->type )
    {
    case constant_int:
      d = con->i;
      break;
    case constant_double:
      d = con->d;
      break;
    case constant_string:
      output( 1, "Warning: Converting 'string' into 'double'! Returning 'atof(%s)'!\n", con->s );
      d = atof( con->s );
      break;
    case constant_bool:
      output( 1, "Warining: Converting 'bool' into 'double'! Returning '%f'!\n", (double) con->b );
      d = con->b;
      break;
    }

  return d;
}


int   get_bool_from_constant( constant *con )
{
  int b = 0;

  assert( con != NULL );

  switch( con->type )
    {
    case constant_int:
      if ( con->i > 1 )
	{
	  output( 1, "Warning: Converting 'integer' into 'bool'! Returning 'FALSE'!\n" );
	  b = 0;
	}
      else
	{
	  output( 1, "Warning: Converting 'integer' into 'bool'! Returning 'bool(%i)!\n", con->i );
	  b = con->i;
	}
      break;
    case constant_double:
      output( 1, "Warning: Converting 'double' into 'boo'! Returning 'FALSE'\n" );
      b = 0;
      break;
    case constant_string:
      if ( strcmp( con->s, "TRUE") == 0 )
	b = 1;
      else
	b = 0;
      break;
    case constant_bool:
      b = con->b;
      break;
    }

  return b;
}




char *get_string_from_node( parsenode *node )
{
	int erg = 0;

	if ( node == NULL )
		erg = 1;
	else
		if ( node->type != node_constant )
			erg = 1;

	if ( erg == 0 )
		return get_string_from_constant( node->con );
	else
		return "foo";
}


int     get_int_from_node( parsenode *node )
{
	int erg = 0;
	int i   = 0;

	if ( node == NULL )
		erg = 1;
	else
		if ( node->type != node_constant )
			erg = 1;

	if ( erg == 0 )
		return get_int_from_constant( node->con );
	else
		i = 0;

	return i;
}


double  get_double_from_node( parsenode *node )
{
	int    erg = 0;
	double d   = 0.0;

	if ( node == NULL )
		erg = 1;
	else
		if ( node->type != node_constant )
			erg = 1;

	if ( erg == 0 )
		return get_double_from_constant( node->con );
	else
		d = 0.0;

	return d;
}


int  get_bool_from_node( parsenode *node )
{
  int    erg = 0;
  int    i = 0;

  if ( node == NULL )
    erg = 1;
  else
    if ( node->type != node_constant )
      erg = 1;

  if ( erg == 0 )
    return get_bool_from_constant( node->con );
  else
    i = 0;

  return i;
}
