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


/* helpers.h


   written by: Oliver Cordes 2010-06-29
   changed by: Oliver Cordes 2013-01-01

   $Id: helpers.h 343 2013-01-02 19:02:00Z ocordes $

*/

#ifndef __helpers_h

#define __helpers_h 1


#define nfree(x) { if ( (x) != NULL ) free( (x) );  }


/* some structures for includes */

#define MAX_INCLUDE_DEPTH 10

int push_incl_savings( char *fname, int lineno );
int pop_incl_savings( char **fname, int *lineno );


extern char *yy_filename;


/* some routines for stings in scanner */

char *strip_variable( char* );
char *strip_string( char* );


/* open parser files */

void open_parser_source( char * );


/* additional parser functions */

void yyerror(const char *str);

#endif

