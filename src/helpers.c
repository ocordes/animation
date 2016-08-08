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



/* helpers.c


   written by: Oliver Cordes 2010-06-29
   changed by: Oliver Cordes 2010-08-01

   $Id: helpers.c 343 2013-01-02 19:02:00Z ocordes $

*/


#include <string.h>

#include "helpers.h"
#include "scanner.h"


typedef struct{
  char *fname;
  int   lineno;
} incl_save;


incl_save incl_saves[MAX_INCLUDE_DEPTH];
int       incl_nr_saves = 0;

char     *yy_filename;


/* includes routines */
int push_incl_savings( char *fname, int lineno )
{
  if ( incl_nr_saves < MAX_INCLUDE_DEPTH )
    {
      incl_saves[incl_nr_saves].fname = strdup( fname );
      incl_saves[incl_nr_saves].lineno = lineno;
      incl_nr_saves++;
      return 0;
    }
  else
    return -1;
}



int pop_incl_savings( char **fname, int *lineno )
{
  if ( incl_nr_saves > 0 )
    {
      incl_nr_saves--;
      (*fname)  = incl_saves[incl_nr_saves].fname;
      (*lineno) = incl_saves[incl_nr_saves].lineno;
      return 0;
    }
  else
    return -1;
}


/* simple strip rountines for the scanner */

char *strip_variable( char *s )
{
  s++;
  return s;
}


char *strip_string( char *s )
{
  if ( ( s[0] == '"' ) || ( s[0] == '\'' ) )
    {
      s[strlen(s)-1] = '\0';

      return s+1;
    }
  else
    return s;
}


/* open parser files */

void open_parser_source( char *fname )
{
  yy_filename = fname;
  yyin = fopen( yy_filename , "r" );

  if ( ! yyin )
    {
      fprintf( stderr, 
	       "Can't open file '%s' (%s)! Abort program!\n", 
	       yy_filename, strerror( errno ) );
      exit( 1 );
    }

}



/* additional parser functions */

void yyerror(const char *str)
{
  fprintf(stderr,"error: %s(%i): %s near \"%s\"\n",
	  yy_filename,
	  yyget_lineno(), str, yytext );
}

