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

/* mprintf.c

   written by: Oliver Cordes 2012-10-07
   changed by: Oliver Cordes 2012-11-10

   $Id: mprintf.c 687 2014-09-14 17:53:49Z ocordes $

*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "amath.h"
#include "output.h"
#include "parsetree.h"
#include "variable.h"


char *arg2str( parsenode *variable, char code, char *fmt )
{
  constant *con;

  char dummy[100];
  char dummy2[100];

  char *p = NULL;

  if ( variable == NULL )
    return strdup( "NULL" );
  
  con  = math_execute_node( variable );

  if ( con == NULL )
    {
      p = strdup( "NULL" );
      return p;
    }

  switch( con->type )
    {
    case constant_none:
      p = strdup( "NONE" );
      break;
    case constant_int:
      snprintf( dummy2, 100, "%%%si", fmt );
      snprintf( dummy, 100, dummy2, con->i );
      p = strdup( dummy );
      break;
    case constant_double:
      switch( code )
	{
	case 'f':
	case 'g':
	case 'e':
	  break;
	default:
	  output( 1, "Warning: '%%%c' doesn't fit to a double variable! Assuming %%f!\n", code );
	  code = 'f';
	  break;
	}
      snprintf( dummy2, 100, "%%%s%c", fmt, code );
      snprintf( dummy, 100, dummy2, con->d );
      p = strdup( dummy );
      break;
    case constant_string:
      p = strdup( con->s );
      break;
    case constant_bool:
      p = strdup( (con->b?"TRUE":"FALSE" ) );
      break;
    }

  free_constant( con );

  return p;
}


int is_format( char c )
{
  switch( c )
    {
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
    case '.':
      return 1;
      break;
    }
  return 0;
}



void msnprintf( char *dest, int length, char *fmt, parsenode *arglist )
{
  char *op;
  char *ip;
  char *sarg;

  parsenode *arg;

  char  dformat[1000];
  char  *df;

  ip = fmt;
  op = dest;

  
  arg = arglist;
  while ( (*ip) != '\0' )
    {
      if ( (*ip) != '%' )
	{
	  (*op) = (*ip);
	  op++;
	  ip++;
	}
      else
	{
	  ip++;
	  /* scan of %... */
	  df = (char*) &dformat;
	  (*df) = '\0';
	  while ( is_format(*ip) )
	    {
	      (*df) = (*ip);
	      df++;
	      ip++;
	    }
	  (*df) = '\0';
	  switch( (*ip) )
	    {
	    case '%':
	      (*op) = '%';
	      op++;
	      break;
	    case 'i':
	    case 'd':
	    case 's':
	    case 'f':
	    case 'g':
	    case 'e':
	    case 'c':
	      if ( arg != NULL )
		{
		  sarg = arg2str( arg->left, (*ip), dformat );
		  arg = arg->right;
		}
	      else
		sarg = strdup( "No arg available!Check Code!" );
	      strcpy( op, sarg );
	      op += strlen( sarg );
	      free( sarg );
	      break;
	    }
	  ip++;
	}
    }
  (*op) = '\0';
}


constant *evaluate_fmt_string( constant *con, parsenode *arglist )
{
  char dummy[1000];

  
  msnprintf( dummy, 1000, con->s, arglist );

  free( con->s );
  con->s = strdup( dummy ); 

  return con;
}
