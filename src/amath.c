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

/*
  amath.c

  written by: Oliver Cordes 2010-08-01
  changed by: Oliver Cordes 2017-03-28

  $Id$

 */


#include <stdio.h>
#include <stdlib.h>

#include <math.h>
#include <string.h>

#include "helpers.h"
#include "amath.h"
#include "execute.h"
#include "mprintf.h"
#include "output.h"
#include "parsetree.h"
#include "project.h"
#include "properties.h"
#include "random.h"
#include "type_array.h"
#include "type_point.h"


/* string routines */


/* math_evaluate_string:
 *
 * returns a new constant with the result
 *
 * frees left and right
 */
constant *math_evaluate_string( constant *left,
								constant *right,
								int mathop )
{
	char *s1, *s2;
	int   len;

	char *s;

	constant *con;

	s1 = get_string_from_constant( left );
	s2 = get_string_from_constant( right );

	output( 2, "s1=%s s2=%s\n", s1, s2 );

	con = new_constant();
	free_constant( left );
	free_constant( right );

	switch( mathop )
	{
	case node_math_add:
		len = strlen( s1 ) + strlen( s2 ) + 1;
		s = malloc( len );
		snprintf( s, len, "%s%s", s1, s2 );

		output( 2, "math: string=%s\n", s );

		con->type = constant_string;
		con->s = s;
		break;
	case node_math_eq:
		con->type = constant_bool;
		con-> b = strcmp( s1, s2 ) == 0;
		break;
	case node_math_greater:
		con->type = constant_bool;
		con->b = strcmp( s1, s2 ) > 0 ;
		break;
	case node_math_lower:
		con->type = constant_bool;
		con->b = strcmp( s1, s2 ) < 0;
		break;
	case node_math_greq:
		con->type = constant_bool;
		con->b = strcmp( s1, s2 ) >= 0;
		break;
	case node_math_loeq:
		con->type = constant_bool;
		con->b = strcmp( s1, s2 ) <= 0;
		break;
	case node_math_neq:
		con->type = constant_bool;
		con->b = strcmp( s1, s2 ) != 0;
		break;
	default:
		yyerror( "Math operation not allowed on strings" );
		con->type = constant_string;
		con->s = strdup( "foo" );

		break;
	}

	return con;
}


/* double routines */


/* math_evaluate_double
 *
 * returns a new constant with the result
 *
 * frees left and right
 */
constant *math_evaluate_double( constant *left,
				constant *right,
				int mathop )
{
  constant *con;
  double    s1, s2;
  double    d;


  con = new_constant();

  s1 = get_double_from_constant( left );
  s2 = get_double_from_constant( right );

  free_constant( left );
  free_constant( right );

  output( 2, "s1=%f s2=%f\n", s1, s2 );

  switch( mathop )
    {
    case node_math_add:
      con->type = constant_double;
      con->d = s1 + s2;
      break;
    case node_math_sub:
      con->type = constant_double;
      con->d = s1 - s2;
      break;
    case node_math_mul:
      con->type = constant_double;
      con->d = s1 * s2;
      break;
    case node_math_div:
      con->type = constant_double;
      if ( s2 == 0.0 )
			{
				output( 1, "Zero division! Using lefthand node!\n" );
	  		con->d = s1;
			}
      else
				con->d = s1 / s2;
      break;
    case node_math_mod:
      con->type = constant_double;
      /* the % operator is not defined for doubles, so emulate
	 the behaviour */
      if ( s2 == 0.0 )
	con->d = s1;
      else
	{
	  d = s1 / s2;
	  con->d = s1 - floor( d )*s2;
	}
      break;
    case node_math_eq:
      con->type = constant_bool;
      con-> b = s1 == s2;
      break;
    case node_math_greater:
      con->type = constant_bool;
      con->b = s1 > s2;
      break;
    case node_math_lower:
      con->type = constant_bool;
      con->b = s1 < s2;
      break;
    case node_math_greq:
      con->type = constant_bool;
      con->b = s1 >= s2;
      break;
    case node_math_loeq:
      con->type = constant_bool;
      con->b = s1 <= s2;
      break;
    case node_math_neq:
      con->type = constant_bool;
      con->b = s1 != s2;
      break;
    }

  switch( con->type )
    {
    case constant_double:
      output( 2, "math_double: double=%f\n", con->d );
      break;
    case constant_bool:
      output( 2, "math_double: bool=%s\n", (con->b?"TRUE":"FALSE" ) );
      break;
    }

  return con;
}


constant *math_evaluate_double_func( constant *left,
				                  int mathop )
{
  double  s1;

  double  d = 0.0;

  s1 = get_double_from_constant( left );

  output( 2, "s1=%f\n", s1 );

  switch( mathop )
    {
    case node_math_minus:
      d = - s1;
      break;
    case node_math_plus:
      d = s1;
      break;
    case node_math_sin:
      d = sin( s1 );
      break;
    case node_math_cos:
      d = cos( s1 );
      break;
    case node_math_tan:
      d = tan( s1 );
      break;
    case node_math_asin:
      d = asin( s1 );
      break;
    case node_math_acos:
      d = acos( s1 );
      break;
    case node_math_atan:
      d = atan( s1 );
      break;
    case node_math_log10:
      d = log10( s1 );
      break;
    case node_math_ln:
      d = log( s1 );
      break;
    case node_math_exp:
      d = exp( s1 );
      break;
    }

  output( 2, "math: double_func=%f\n", d );

  left->d    = d;
  left->type = constant_double;

  return left;
}


/* int routines */


/* math_evaluate_string:
 *
 * returns a new constant with the result
 *
 * frees left and right
 */
constant *math_evaluate_int( constant *left,
			     constant *right,
			     int mathop )
{
  constant *con;

  int s1, s2;


  con = new_constant();

  s1 = get_int_from_constant( left );
  s2 = get_int_from_constant( right );

  free_constant( left );
  free_constant( right );

  output( 2, "s1=%i s2=%i\n", s1, s2 );

  switch( mathop )
    {
    case node_math_add:
      con->type = constant_int;
      con->i = s1 + s2;
      break;
    case node_math_sub:
      con->type = constant_int;
      con->i = s1 - s2;
      break;
    case node_math_mul:
      con->type = constant_int;
      con->i = s1 * s2;
      break;
    case node_math_div:
      con->type = constant_int;
      if ( s2 == 0 )
			{
				output( 1, "Zero division! Using lefthand node!\n" );
	  		con->i = s1;
			}
      else
				con->i = s1 / s2;
      break;
    case node_math_mod:
      con->type = constant_int;
      con->i = s1 % s2;
      break;
    case node_math_eq:
      con->type = constant_bool;
      con-> b = s1 == s2;
      break;
    case node_math_greater:
      con->type = constant_bool;
      con->b = s1 > s2;
      break;
    case node_math_lower:
      con->type = constant_bool;
      con->b = s1 < s2;
      break;
    case node_math_greq:
      con->type = constant_bool;
      con->b = s1 >= s2;
      break;
    case node_math_loeq:
      con->type = constant_bool;
      con->b = s1 <= s2;
      break;
    case node_math_neq:
      con->type = constant_bool;
      con->b = s1 != s2;
      break;
    }

  switch( con->type )
    {
    case constant_int:
      output( 2, "math_int: int=%i\n", con->i );
      break;
    case constant_bool:
      output( 2, "math_int: bool=%s\n", (con->b?"TRUE":"FALSE" ) );
      break;
    }

  return con;
}


constant *math_evaluate_int_func( constant *left,
				  int mathop )
{
  int  s1;

  int  i = 0;

  s1 = get_int_from_constant( left );

  output( 2, "s1=%i\n", s1 );

  switch( mathop )
    {
    case node_math_minus:
      i = - s1;
      break;
    case node_math_plus:
      i = s1;
      break;
    }

  output( 2, "math: int_func=%i\n", i );

  left->i    = i;
  left->type = constant_int;

  return left;
}


/* bool operations */

constant *math_evaluate_bool( constant *left,
			      constant *right,
			      int mathop )
{
  constant *con;

  int s1, s2, b = 0;

  con = new_constant();
  con->type = constant_bool;

  s1 = get_bool_from_constant( left );
  s2 = get_bool_from_constant( right );

  output( 2, "s1=%s s2=%s\n", (s1?"TRUE":"FALSE"), (s2?"TRUE":"FALSE") );

  free_constant( left );
  free_constant( right );


  switch( mathop )
    {
    case node_math_and:
      b = s1 && s2;
      break;
    case node_math_or:
      b = s1 || s2;
      break;
    case node_math_eq:
      b = s1 == s2;
      break;
    case node_math_neq:
      b = s1 != s2;
      break;
    case node_math_greater:
      output( 1, "Warning: '>' on bool constants/variables! Return FALSE!\n" );
      b = 0;
      break;
    case node_math_lower:
      output( 1, "Warning: '<' on bool constants/variables! Return FALSE!\n" );
      b = 0;
      break;
    case node_math_greq:
      output( 1, "Warning: '>=' on bool constants/variables! Return FALSE!\n" );
      b = 0;
      break;
    case node_math_loeq:
      output( 1, "Warning: '<=' on bool constants/variables! Return FALSE!\n" );
      b = 0;
      break;
    }

  con->b = b;
  output( 2, "math: bool=%s\n", (b?"TRUE":"FALSE") );

  return con;
}





/* main routines */

int  math_maximum_type( constant *left,
						constant *right,
						int mathop )
{
	int type;
	int ltype;
	int rtype;

	ltype = left->type;
	rtype = right->type;

	if ( ltype == rtype )
		/* clear, both types are the same */
		type = ltype;
	else
	{
		/* sorting parameters, int -> double -> string -> bool */
		if ( rtype > ltype )
		{
			type = ltype;
			ltype = rtype;
			rtype = type;
		}

		switch( ltype )
		{
		case constant_string:
			/* convert everything into strings */
			type = constant_string;
			break;
		case constant_double:
			type = constant_double;
			break;
		case constant_int:
			type = constant_int;
			break;
		case constant_bool:
			type = constant_bool;
			break;
		case constant_point:
			type = constant_point;
			break;
		case constant_array:
			type = constant_array;
			break;
		default:
			type = constant_none;
			break;
		}
	}

	return type;
}


constant *math_evaluate_node( constant *left,
			      constant *right,
			      int mathop )
{
  int       type;
  constant *con = NULL;

  if ( ( left == NULL ) || ( right == NULL ) ) return NULL;

  type = math_maximum_type( left, right, mathop );

  output( 2, "eval_node: type=%i\n", type );

  switch( type )
    {
    case constant_none:
      return left;
      break;
    case constant_string:
      con = math_evaluate_string( left, right, mathop );
      break;
    case constant_int:
      con = math_evaluate_int( left, right, mathop );
      break;
    case constant_double:
      con = math_evaluate_double( left, right, mathop );
      break;
    case constant_bool:
      con = math_evaluate_bool( left, right, mathop );
      break;
		case constant_point:
			con = math_evaluate_point( left, right, mathop );
			break;
		case constant_array:
		  con = math_evaluate_array( left, right, mathop );
			break;
    }

  return con;
}


constant *math_evaluate_node_func( constant *left, int mathop )
{
	if ( left == NULL ) return NULL;

	switch( left->type )
	{
	  case constant_none:
			return left;
			break;
		case constant_int:
			switch( mathop )
			{
				case node_math_minus:
				case node_math_plus:
					left = math_evaluate_int_func( left, mathop );
					break;
				default:
					left = math_evaluate_double_func( left, mathop );
					break;
			}
			break;
		case constant_double:
			left = math_evaluate_double_func( left, mathop );
			break;
		case constant_string:
			output( 1, "Can't use functions on strings!\n" );
			free_constant( left );
			return NULL;
			break;
		case constant_point:
			switch( mathop )
			{
				case node_math_minus:
				case node_math_plus:
					left = math_evaluate_point_func( left, mathop );
					break;
				default:
					output( 1, "Can't use functions on Points!\n" );
					return NULL;
					break;
			}
			break;
	}

	return left;
}


constant *math_execute_node_opt_variable( parsenode *node, constant *val )
{
	constant *con;

	switch( node->type )
	{
		case node_array_element:
			con = math_execute_array_element( node->left, val );
			break;
		case node_array_elements:
			con = math_execute_array_element( node->left, val );
			break;
		case node_property_variable:
			con =	math_execute_property_variable( node->left, val );
			break;
	}

	return con;
}


/* math execution functions */

constant *math_execute_node( parsenode *node )
{
  constant *conl, *conr, *con = NULL;

  output( 10, "math_execute_node: type = %i\n", node->type );
  switch( node->type )
    {
    case node_not:
      con = math_execute_node( node->cond );
      con->b = ! con->b;
      break;
    case node_math:
      conl = math_execute_node( node->left );
      conr = math_execute_node( node->right );

      con = math_evaluate_node( conl, conr, node->mathop );
      break;
    case node_math_op:
      conl = math_execute_node( node->left );
      con = math_evaluate_node_func( conl, node->mathop );
      break;
    case node_constant:
      con = clone_constant( node->con );
      break;
    case node_variable:
		case node_opt_variable:
      output( 10, "node_variable=%s\n", node->var );
      con = get_variable( main_project->vars, node->var );
      if ( con == NULL )
	       con = get_variable( local_vars[0], node->var );

      if ( con == NULL )
	       output( 1, "NULL variable!\n" );
      else
	    {
	      if ( con->type == constant_none )
	        fprintf( stderr, "Type of %s is undefined!\n", node->var );
				else
				{
					if ( node->type == node_opt_variable )
					 con = math_execute_node_opt_variable( node->left, con );
				}
	    }
      break;
    case node_string_fmt:
      conl = math_execute_node( node->left );
      con = evaluate_fmt_string( conl, node->right );
      break;

    case node_cmd_macro:
      execute_cmd_macro( node );
      con = return_value;
      return_value = NULL;
      break;
		case node_point:
			con = add_constant_point( math_execute_node_double( node->left ),
																math_execute_node_double( node->right ) );
			break;
		case node_array:
		  con = add_constant_array( node->left );
			break;

    /* random functions */
		case node_cmd_random:
			con = execute_cmd_random();
			break;
		case node_cmd_random_point:
			con = execute_cmd_random_point();
			break;

		/* property functions */
		case node_property_definition:
			con = execute_property_definition( node->left, node->right );
			break;
    }

  return con;
}



int      math_execute_node_int( parsenode *node )
{
  constant *con;
  int       i = 0;

  con = math_execute_node( node );

  switch( con->type )
    {
    case constant_int:
      i = con->i;
      break;
    case constant_double:
      i = floor( con->d );
      break;
    case constant_string:
      i = atoi( con->s );
      break;
    case constant_bool:
      i = con->b;
      break;
    }

  free_constant( con );

  return i;
}


double   math_execute_node_double( parsenode *node )
{
  constant *con;
  double    d = 0.0;

  con = math_execute_node( node );

  switch( con->type )
    {
    case constant_int:
      d = con->i;
      break;
    case constant_double:
      d = con->d;
      break;
    case constant_string:
      d = atof( con->s );
      break;
    case constant_bool:
      d = con->b;
      break;
    }

  free_constant( con );

  return d;
}



char*  math_execute_node_string( parsenode *node )
{
  constant *con;
  char      dummy[100];
  char     *s;

  con = math_execute_node( node );

  switch( con->type )
    {
    case constant_int:
      snprintf( dummy, 100, "%i", con->i );
      s = strdup( dummy );
      break;
    case constant_double:
      snprintf( dummy, 100, "%f", con->d );
      s = strdup( dummy );
      break;
    case constant_string:
      s = strdup( con->s );
      break;
    case constant_bool:
      if ( con->b )
	s = strdup( "TRUE" );
      else
	s = strdup( "FALSE" );
      break;
    }

  free_constant( con );

  return s;
}
