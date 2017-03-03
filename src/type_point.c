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


/* type_point.c

written by: Oliver Cordes 2017-02-25
changed by: Oliver Cordes 2017-03-03

$Id$

*/

#include <stdio.h>
#include <stdlib.h>

#include <assert.h>


#include "config.h"

#include "helpers.h"
#include "output.h"
#include "parsetree.h"
#include "type_point.h"



/* point node definitions */
parsenode *add_node_point( parsenode *x, parsenode *y  )
{
  parsenode *newnode;

  newnode = new_node();

  newnode->type  = node_point;

  newnode->left  = x;
  newnode->right = y;

  return newnode;
}

/* node to constant conversion */

constant *add_constant_point( double x, double y )
{
  constant *con;

  con = new_constant();

  if ( con != NULL )
  {
    con->type = constant_point;
    con->p.x  = x;
    con->p.y  = y;
    output( 2, "add_constant_point=(%f,%f)\n", x, y );
  }

  return con;
}


/* type conversion */
Point get_point_from_constant( constant *con )
{
   Point p;

   assert( con != NULL );

   switch( con->type )
   {
     case constant_point:
       p = con->p;
       break;
     default:
       p.x = 0;
       p.y = 0;
       output( 1, "Warning: Converting type to Point is not supported! Returning '(0,0)\n'" );
       break;
   }
   return p;
}


/* math operations */

constant *math_evaluate_point_dir( constant *left,
			                             constant *right,
			                             int mathop,
                                   int direction  )
{
  constant *con;

  Point     s1, s2;
	double    d = 0.0;

  con = new_constant();
  con->type = constant_point;

  s1 = get_point_from_constant( left );

	if ( right->type == constant_point )
	{
    if ( direction == 1 )
    {
      s2 = s1;
      s1 = get_point_from_constant( right );
    }
    else
  	  s2 = get_point_from_constant( right );

  	output( 2, "s1=(%f,%f) s2=(%f,%f)\n", s1.x, s1.y, s2.x, s2.y );

  	switch( mathop )
    {
    	case node_math_add:
      	con->type = constant_point;
      	con->p.x  = s1.x + s2.x;
				con->p.y  = s2.y + s2.y;
      break;
     case node_math_sub:
       con->type = constant_point;
       con->p.x  = s1.x - s2.x;
			 con->p.y  = s1.y - s2.y;
       break;
		 case node_math_mul:
		 	 /* scalar product */
			 con->type = constant_double;
			 con->d    = s1.x * s2.y - s1.y * s2.x;
			 break;
		 case node_math_eq:
	     con->type = constant_bool;
			 con->b = ( s1.x == s2.x ) && ( s1.y == s2.y );
	     break;
		 case node_math_neq:
		   con->type = constant_bool;
		   con->b = ( s1.x != s2.x ) || ( s1.y != s2.y );
		   break;
		 case node_math_minus:
		 	 con->type = constant_point;
			 con->p.x = -s1.x;
			 con->p.y = -s1.y;
			 break;
		 default:
			 output( 1, "Math operation on Point not spported!\n" );
       con->type = constant_point;
			 con->p    = s1;
       break;
    }
	}
	else
	{
		/* try to get a double conversion from the right side */
    d = get_double_from_constant( right );

		switch( mathop )
    {
    	case node_math_mul:
				con->type = constant_point;
				con->p.x  = s1.x * d;
				con->p.y  = s1.y * d;
			  break;
			case node_math_div:
        if ( direction == 1 )
        {
          output( 1, "double / point is not allowed!\n");
          con->p = s1;
        }
        else
        {
				  if ( d == 0.0 )
				  {
					  output( 1, "Zero division! Using lefthand node!\n" );
					  con->p = s1;
				  }
				  else
			  	{
				  	con->type = constant_point;
					  con->p.x  = s1.x / d;
					  con->p.y  = s1.y / d;
			  	}
        }
				break;
			default:
				output( 1, "Math operation on Point not spported!\n" );
				break;
		}
	}

  switch( con->type )
    {
    case constant_point:
      output( 2, "math_point: point=(%f,%f)\n", con->p.x, con->p.y );
      break;
    case constant_bool:
      output( 2, "math_point: bool=%s\n", (con->b?"TRUE":"FALSE" ) );
      break;
    }

	free_constant( left );
  free_constant( right );


  return con;
}


constant *math_evaluate_point( constant *left,
			                         constant *right,
			                         int mathop )
{
  if ( left->type == constant_point )
    return math_evaluate_point_dir( left, right, mathop, 0 );
  else
    return math_evaluate_point_dir( right, left, mathop, 1 );

  return left;
}


constant *math_evaluate_point_func( constant *left,
			                         			int mathop )
{
  constant *con;

  Point     s1;

  con = new_constant();
  con->type = constant_point;

  s1 = get_point_from_constant( left );

  output( 2, "s1=(%f,%f)\n", s1.x, s1.y );

  switch( mathop )
  {
		case node_math_plus:
			/* do nothing */
			break;
    case node_math_minus:
		 	con->type = constant_point;
			con->p.x = -s1.x;
			con->p.y = -s1.y;
			break;
		default:
     	 yyerror( "Math function operation on Point not spported!");
			 output( 1, "Math function operation on Point not spported!" );
       con->type = constant_point;
			 con->p    = s1;
       break;
  }


	free_constant( left );

  return con;
}
