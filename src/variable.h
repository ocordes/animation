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

/* variable.h

   written by: Oliver Cordes 2010-08-08
   changed by: Oliver Cordes 2017-03-29

   $Id$

*/

#ifndef __variable_h

#define __variable_h 1

/* constant definition  */

#define constant_none   0
#define constant_bool   1
#define constant_int    2
#define constant_double 3
#define constant_string 4
#define constant_point  5
#define constant_array  6

/* forward declaration */
struct _constant;


/* special types */

/* point definition */
typedef struct {
  double x,y;
} Point;

/* Array definition */
typedef struct {
  int  nr;
  int  alloc;
  int  type;
  struct _constant **cons;
} Array;


/* standard constand including all types */

struct _constant{
        int type;
        union{
                int     i;
                double  d;
                char   *s;
                int     b;
                Point   p;
                Array   a;
        };
};

typedef struct _constant constant;


/* variable definition */

typedef struct{
	char     *name;

	constant  con;     /* used for static variables */
} variable;


#define increment_vars 10

#define variable_flag_read  1
#define variable_flag_write 2

typedef struct{
	variable *vars;
	int       flags;
	int       nrvars;
	int       maxvars;
} variables;


/* constant functions */

constant *new_constant( void );
void free_constant_static( constant con );
void      free_constant( constant* con );
constant *clone_constant( constant *con );

constant *add_constant_int( int i );
constant *add_constant_double( double d );
constant *add_constant_string( char *s );
constant *add_constant_bool( int b );


char     *print_bool( int b );


/* variable functions */

variables *new_variables( void );
void       free_variables( variables *vars );
void       print_variables( variables *vars );
int find_variable( variables *vars, char *name );
void       add_variable( variables *vars, char *name, int type );

int set_constant_variable( variables *vars, char *name, constant *con );
int set_constant_variable_element( variables*, char*, constant*, constant* );
void set_int_variable( variables *vars, char *name, int i );
void set_double_variable( variables *vars, char *name, double d );
void set_string_variable( variables *vars, char *name, char *s );


constant  *get_variable( variables *vars, char *name );

char *constant2str( constant *);

char *typeofconstant( constant * );

#endif
