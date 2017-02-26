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

/* parsetree.h

   written by: Oliver Cordes 2010-07-18
   changed by: Oliver Cordes 2017-02-25

   $Id$

*/

#ifndef __parsetree_h
#define __parsetree_h  1


#include "variable.h"


/* parse node configurations */

#define node_none         0
#define node_variable     1
#define node_constant     2
#define node_point        3
#define node_array        4
#define node_if           5
#define node_not          6

#define node_quit         10
#define node_exit         11
#define node_break        12
#define node_return       13

#define node_math         20
#define node_math_op      21
#define node_math_add     22
#define node_math_sub     23
#define node_math_mul     24
#define node_math_div     25
#define node_math_mod     26
#define node_math_sin     27
#define node_math_cos     28
#define node_math_tan     29
#define node_math_asin    30
#define node_math_acos    31
#define node_math_atan    32
#define node_math_log10   33
#define node_math_ln      34
#define node_math_exp     35
#define node_math_minus   36
#define node_math_plus    37

#define node_math_eq      40
#define node_math_greater 41
#define node_math_lower   42
#define node_math_greq    43
#define node_math_loeq    44
#define node_math_neq     45
#define node_math_and     46
#define node_math_or      47
#define node_string_fmt   48
#define node_array_list   49

#define node_cmd_arglist  50
#define node_cmd_vararglist 51


/* commands */
#define node_cmd_assign   61
#define node_cmd_load     62
#define node_cmd_print    63
#define node_cmd_printf   64
#define node_cmd_textfile 65
#define node_cmd_text     66
#define node_cmd_crop     67
#define node_cmd_mirrorx  68
#define node_cmd_mirrory  69
#define node_cmd_mirrorxy 70
#define node_cmd_system   71
#define node_cmd_window   72
#define node_cmd_image    73
#define node_cmd_macro    74


struct _parsenode{
	int                  type;
	struct _parsenode   *next;
	struct _parsenode   *left;
	struct _parsenode   *right;
        int                  nrargs;
        struct _parsenode ** args;
	union{
		constant          *con;
		char              *var;
		int                mathop;
		struct _parsenode *cond;
	};
};

typedef struct _parsenode parsenode;


/* necessary for the scanner/parser */

#define YYSTYPE parsenode *

/* function declarations */

/* parse node functions */

parsenode *new_node( void );
void       free_node( parsenode* node );

void print_node( parsenode *node );


void allocate_node_args( parsenode *node, int nrargs );



parsenode *add_node_constant_int( int i );
parsenode *add_node_constant_double( double d );
parsenode *add_node_constant_string( char *s );
parsenode *add_node_constant_string_arglist( parsenode *s, parsenode *arglist );
parsenode *add_node_constant_bool( int b );

parsenode *add_node_variable_r( char *s );
parsenode *add_node_variable( char *s );

void       register_node_variable( parsenode *var );

parsenode *add_node_arglist( parsenode *arglist, parsenode *rvalue );
parsenode *add_node_vararglist( parsenode *vararglist, parsenode *tlvariable );
int        get_node_vararglist_len( parsenode *vararglist );

parsenode *add_node_cmd( parsenode *cmdlist, parsenode *command );



parsenode *add_node_window_finish( parsenode *window, parsenode *commands );
parsenode *add_node_window( parsenode *x1, parsenode *y1, parsenode *x2, parsenode *y2 );

parsenode *add_node_if( parsenode *cond, parsenode *then_cmds, parsenode *else_cmds );
parsenode *add_node_not( parsenode *cond );

parsenode *add_node_math( parsenode *left, parsenode *right, int mathop );
parsenode *add_node_math_op( int mathop );
parsenode *add_node_math_func( parsenode *left, parsenode *mathfuncop );



char   *get_string_from_constant( constant *con );
int     get_int_from_constant( constant *con );
double  get_double_from_constant( constant *con );
int     get_bool_from_constant( constant *con );

char   *get_string_from_node( parsenode *node );
int     get_int_from_node( parsenode *node );
double  get_double_from_node( parsenode *node );
int  get_bool_from_node( parsenode *node );

#endif
