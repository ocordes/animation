%{
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


/* parser.y

   written by; Oliver Cordes 2010-06-28
   changed by: Oliver Cordes 2017-03-15

   $Id$

*/

#include <stdio.h>
#include <string.h>

#include "blocks.h"
#include "controls.h"
#include "execute.h"
#include "filldef.h"
#include "font.h"
#include "helpers.h"
#include "imagedef.h"
#include "macro.h"
#include "output.h"
#include "parsecmd.h"
#include "parsetree.h"
#include "project.h"
#include "scanner.h"
#include "type_array.h"
#include "type_point.h"

int yywrap()
{
        return 1;
}

#define amx_lang_version "0.9.10"

/*  history:
0.9.10:  2017-03-17
  - change the command parameter syntax, parameter must be comma seperated
  - add array_element syntax

*/


char *get_amx_lang_version( void )
{
  return strdup( amx_lang_version );
}

%}


%token TCONSTANT
%token TVARIABLE
%token TLVARIABLE
%token TSTRING

%token TPROJECT TOUTPUT TOUTPUTFILE TOUTPUTDIR TGEOMETRY TFPS TBITRATE TBACKGROUND TGLOBAL TBLOCKMOVIES TOVERWRITE

%token TRETURN TSEMICOLON TASSIGN TCOMMA TCOLON
%token TTRUE TFALSE
%token TL_BRACKET TR_BRACKET TL_ARRAY TR_ARRAY
%token TIF TELSE TENDIF
%token TEQ TNEQ TGREATER TLOWER TGREQ TLOEQ TAND TOR TNOT
%token TPLUS TMINUS TMULTIPLY TDIVIDE TMOD TSTRING_FMT
%token TSIN TCOS TTAN TASIN TACOS TATAN TLOG10 TLN TEXP
%token TRANDOM TRANDOM_SEED TRANDOM_POINT
%token TBLOCK TENDBLOCK
%token TCONTROL TENDCONTROL
%token TPOSTPROC TENDPOSTPROC
%token TWINDOW TENDWINDOW
%token TMACRO TENDMACRO TRETURNMACRO TMACRONAME
%token TLOAD
%token TPRINT TPRINTF
%token TLOOP TEMPTY TFILES TSTATIC
%token TPENDEF TENDPENDEF TFILLCOLOR
%token TTEXTFILE TTEXTFILEALPHA TTEXT TTEXTALPHA
%token TLINE TCIRCLE TCIRCLEFILL TRECTANGLE TRECTANGLEFILL TROUNDRECTANGLE TROUNDRECTANGLEFILL
%token TPOLYGON TPOLYGONFILL TPOLYLINE TPOLYLINEFILL
%token TPROPERTY
%token TSYSTEM
%token TCROP
%token TMIRRORX TMIRRORY TMIRRORXY

%token TQUIT TBREAK TEXIT

%token TFONTDEF TENDFONTDEF TFONT TBOLD TITALIC TBOLDITALIC TSIZE TCOLOR TSEPARATOR
%token TIMAGEDEF TENDIMAGEDEF TFILE TRESCALE TIMAGE TMODE

%token TUNKNOWN


%%

instructions:    /* empty */
                 | instructions instruction
                 ;

instruction      : empty_command
                 | TPROJECT TSTRING             { project_set_project_name( $2 ); }
                 | TOUTPUT TSTRING              { project_set_output_device( $2 ); }
                 | TOUTPUTFILE TSTRING          { project_set_output_filename( $2 ); }
                 | TOUTPUTDIR TSTRING           { project_set_outputdir( $2 ); }
                 | TGEOMETRY TSTRING            { project_set_geometry( $2 ); }
                 | TFPS TCONSTANT               { project_set_fps( $2 ); }
                 | TBITRATE TCONSTANT           { project_set_bitrate( $2 ); }
                 | TBACKGROUND TSTRING          { project_set_background( $2 ); }
                 | TBLOCKMOVIES TCONSTANT       { project_set_blockmovies( $2 ); }
                 | TOVERWRITE TCONSTANT         { project_set_overwrite( $2 ); }
                 | block
                 | postproc
                 | fontdef
                 | pendef
		             | imagedef
                 | globaldef
                 | control
                 | macrodef
                 ;

block            : block_header commands TENDBLOCK { output( 1, "finishing block\n" ); block_add_commands( $2); project_add_block( current_block ); }
                 ;

block_header     : TBLOCK TSTRING TRETURN       { block_new_block( $2 ); }
                 ;

postproc         : postproc_header commands TENDPOSTPROC { output( 1, "finishing postproc block\n" ); block_add_commands( $2 ); project_add_block( current_block ); }
                 ;

postproc_header  : TPOSTPROC TRETURN            { block_new_block_name( "postproc" ); }
                 ;

commands         : commands command_eol         { $$ = add_node_cmd( $1, $2 ); }
                 | command_eol                  { $$ = $1; }
                 ;

command_eol      : command TRETURN              { $$ = $1; }
                 | command TSEMICOLON           { $$ = $1; }
                 | empty_command                { $$ = NULL; }
                 ;

command          : TLOAD TVARIABLE              { $$ = add_node_cmd_load( $2 ); }
                 | TLOOP TEMPTY TCONSTANT       { $$ = block_add_files_empty( $3, NULL, NULL, NULL ); }
                 | TLOOP TEMPTY TCONSTANT TCONSTANT TCONSTANT { $$ = block_add_files_empty( $3, $4, $5, NULL ); }
                 | TLOOP TEMPTY TCONSTANT TCONSTANT TCONSTANT TCONSTANT { $$ = block_add_files_empty( $3, $4, $5, $6 ); }
                 | TLOOP TFILES TSTRING         { $$ = block_add_files_string( $3, NULL ); }
                 | TLOOP TFILES TSTRING TCONSTANT { $$ = block_add_files_string( $3, $4 ); }
                 | TLOOP TSTATIC TSTRING factor { $$ = block_add_files_static( $3, $4 ); }
                 | l_value TASSIGN r_value      { $$ = add_node_cmd_assign( $1, $3 ); }
                 | TTEXTFILE factor TCOMMA factor TCOMMA fontname TCOMMA stringf
                                                { $$ = add_node_cmd_textfile( $2, $4, $6, $8, NULL ); }
                 | TTEXTFILEALPHA factor TCOMMA factor TCOMMA factor TCOMMA fontname TCOMMA stringf
                                                { $$ = add_node_cmd_textfile( $4, $6, $8, $10, $2 ); }
                 | TTEXT factor TCOMMA factor TCOMMA fontname TCOMMA stringf
                                                { $$ = add_node_cmd_text( $2, $4, $6, $8, NULL ); }
                 | TTEXTALPHA factor TCOMMA factor TCOMMA factor TCOMMA fontname TCOMMA stringf
                                                { $$ = add_node_cmd_text( $4, $6, $8, $10, $2 ); }
                 | TIMAGE factor TCOMMA factor TCOMMA TSTRING
                                                { $$ = add_node_cmd_image( $2, $4, $6 );  }
                 | TCROP factor TCOMMA factor TCOMMA factor TCOMMA factor
                                                { $$ = add_node_cmd_crop( $2, $4, $6, $8 ); }
                 | TMIRRORX                     { $$ = add_node_cmd_mirrorx(); }
                 | TMIRRORY                     { $$ = add_node_cmd_mirrory(); }
                 | TMIRRORXY                    { $$ = add_node_cmd_mirrorxy(); }
                 | TLINE factor TCOMMA factor TCOMMA TSTRING
                                                { $$ = add_node_cmd_line( $2, $4, $6 ); }
                 | TCIRCLE factor TCOMMA factor TCOMMA TSTRING
                                                { $$ = add_node_cmd_circle( $2, $4, $6, 0 ); }
                 | TCIRCLEFILL factor TCOMMA factor TCOMMA TSTRING
                                                { $$ = add_node_cmd_circle( $2, $4, $6, 1); }
                 | TRECTANGLE factor TCOMMA factor TCOMMA TSTRING
                                                { $$ = add_node_cmd_rectangle( $2, $4, $6, 0 ); }
		             | TRECTANGLEFILL factor TCOMMA factor TCOMMA TSTRING
                                                { $$ = add_node_cmd_rectangle( $2, $4, $6, 1 ); }
                 | TROUNDRECTANGLE factor TCOMMA factor TCOMMA factor TCOMMA TSTRING
                                                { $$ = add_node_cmd_roundrectangle( $2, $4, $6, $8, 0 ); }
                 | TROUNDRECTANGLEFILL factor TCOMMA factor TCOMMA factor TCOMMA TSTRING
                                                { $$ = add_node_cmd_roundrectangle( $2, $4, $6, $6, 1 ); }
                 | TPOLYGON factor TCOMMA TSTRING
                                                { $$ = add_node_cmd_polygon( $2, $4, 0 ); }
                 | TPOLYGONFILL factor TCOMMA TSTRING
                                                { $$ = add_node_cmd_polygon( $2, $4, 1 ); }
                 | TPOLYLINE factor TCOMMA TSTRING
                                                { $$ = add_node_cmd_polyline( $2, $4, 0 ); }
                 | TPOLYLINEFILL factor TCOMMA TSTRING
                                                { $$ = add_node_cmd_polyline( $2, $4, 1 ); }
                 | TSYSTEM stringf              { $$ = add_node_cmd_system( $2 );}
		             | window
                 | if_command                   { $$ = $1; }
                 | TPRINT r_value               { $$ = add_node_cmd_print( $2 ); }
                 | TPRINTF r_value              { $$ = add_node_cmd_print( $2 ); }
                 | TQUIT                        { $$ = add_node_cmd_quit(); }
                 | TEXIT                        { $$ = add_node_cmd_exit(); }
                 | TBREAK                       { $$ = add_node_cmd_break(); }
                 | TRETURNMACRO                 { $$ = add_node_cmd_return( NULL ); }
                 | TRETURNMACRO expr            { $$ = add_node_cmd_return( $2 ); }
		             | TRETURNMACRO bool_expr       { $$ = add_node_cmd_return( $2 ); }
                 | macro_func                   { $$ = $1; }
                 | random_seed                  { $$ = $1; }
                 ;

window           : window_header commands TENDWINDOW  { $$ = add_node_window_finish( $1, $2 ); }
                 ;

window_header    : TWINDOW factor TCOMMA factor TCOMMA factor TCOMMA factor TRETURN { $$ = add_node_window( $2, $4, $6, $8 ); }
                 ;


fontdef          : fontdef_header font_commands TENDFONTDEF { font_end(); }
                 ;

font_commands    : font_commands font_command_eol
                 | font_command_eol
                 ;

font_command_eol : font_command TRETURN
                 | font_command TSEMICOLON
                 | empty_command
                 ;

font_command     : TFONT TSTRING              { font_set_font( $2 ); }
                 | TBOLD TSTRING              { font_set_bold( $2 ); }
                 | TITALIC TSTRING            { font_set_italic( $2 ); }
                 | TBOLDITALIC TSTRING        { font_set_bolditalic( $2 ); }
                 | TSIZE TCONSTANT            { font_set_size( $2 ); }
                 | TCOLOR TSTRING             { font_set_color( $2 ); }
                 | TSEPARATOR TCONSTANT       { font_set_separator( $2 ); }
                 ;

fontdef_header   : TFONTDEF TSTRING TRETURN   { font_start( $2 );}
                 ;

fontname         : TSTRING                    { $$ = $1; }
                 ;

pendef           : pendef_header pendef_commands TENDPENDEF { pendef_end(); }
                 ;

pendef_commands  : pendef_commands pendef_commands_eol
                 | pendef_commands_eol
                 ;
pendef_commands_eol : pendef_command TRETURN
                 | pendef_command TSEMICOLON
                 | empty_command
                 ;

pendef_command   : TCOLOR TSTRING              { pendef_set_color( $2 ); }
                 | TSIZE  TCONSTANT            { pendef_set_size( $2 ); }
                 | TFILLCOLOR TSTRING          { pendef_set_fillcolor( $2 ); }
                 ;

pendef_header    : TPENDEF TSTRING TRETURN     { pendef_start( $2 );  }
                 ;

imagedef         : imagedef_header imagedef_commands TENDIMAGEDEF { imagedef_end(); }
                 ;

imagedef_commands: imagedef_commands imagedef_command_eol
                 | imagedef_command_eol
                 ;

imagedef_command_eol : imagedef_command TRETURN
                 | imagedef_command TSEMICOLON
                 | empty_command
                 ;

imagedef_command : TFILE TSTRING              { imagedef_set_filename( $2 ); }
                 | TRESCALE TSTRING           { imagedef_set_resize_factor( $2 ); }
                 | TRESCALE TCONSTANT TCONSTANT { imagedef_set_resize_value( $2, $3 ); }
                 | TMODE TSTRING              { imagedef_set_mode( $2 ); }
                 ;

imagedef_header  : TIMAGEDEF TSTRING TRETURN   { imagedef_start( $2 ); }
                 ;


control          : control_header control_commands TENDCONTROL {}
                 ;

control_commands : control_commands control_command_eol
                 | control_command_eol
                 ;

control_command_eol : control_command TRETURN
                 | control_command TSEMICOLON
                 | empty_command
                 ;

control_command  : TBLOCK TSTRING               { controls_add_control( $2); }
                 ;

control_header   : TCONTROL TRETURN
                 ;


macrodef         : macro_header commands TENDMACRO      { macro_add_commands( $2); project_add_macro( current_macro ); }
                 ;

macro_header     : TMACRO TMACRONAME TL_BRACKET TR_BRACKET { macro_new_macro( $2, NULL ); }
                 | TMACRO TMACRONAME TL_BRACKET vararg_list TR_BRACKET { macro_new_macro( $2, $4 ); }
                 ;

vararg_list      : TLVARIABLE TCOMMA vararg_list        { $$ = add_node_vararglist( $3, $1 ); }
                 | TLVARIABLE                           { $$ = add_node_vararglist( NULL, $1 ); }
                 ;

globaldef        : TGLOBAL TLVARIABLE TASSIGN TCONSTANT { add_node_cmd_assign_global( $2, $4 ); }
                 | TGLOBAL TLVARIABLE TASSIGN TSTRING   { add_node_cmd_assign_global( $2, $4 ); }
                 ;


empty_command    : TRETURN
                 ;

l_value          : TLVARIABLE                           { $$ = $1; }
                 | TLVARIABLE TL_ARRAY expr TR_ARRAY    { $$ = $1; }
                 ;

r_value          : expr                                 { $$ = $1; }
		             | bool_expr                            { $$ = $1; }
                 ;

if_command       : TIF if_cond commands TENDIF          { $$ = add_node_if( $2, $3, NULL ); }
                 | TIF if_cond commands TELSE commands TENDIF { $$ = add_node_if( $2, $3, $5 ); }
                 ;

if_cond          : bool_expr TRETURN
                 ;

bool_expr        : bool_term TAND bool_term             { $$ = add_node_math( $1, $3, node_math_and ); }
                 | bool_term TOR bool_term              { $$ = add_node_math( $1, $3, node_math_or ); }
                 | TNOT bool_term	                      { $$ = add_node_not( $2 ); }
                 | bool_term                            { $$ = $1; }
                 ;

bool_term        : bool_factor TEQ bool_factor          { $$ = add_node_math( $1, $3, node_math_eq ); }
		             | bool_factor TGREATER bool_factor     { $$ = add_node_math( $1, $3, node_math_greater ); }
		             | bool_factor TLOWER bool_factor       { $$ = add_node_math( $1, $3, node_math_lower ); }
		             | bool_factor TGREQ bool_factor        { $$ = add_node_math( $1, $3, node_math_greq ); }
		             | bool_factor TLOEQ bool_factor        { $$ = add_node_math( $1, $3, node_math_loeq ); }
		             | bool_factor TNEQ bool_factor         { $$ = add_node_math( $1, $3, node_math_neq ); }
		             | TL_BRACKET  bool_expr TR_BRACKET     { $$ = $2; }
                 ;

bool_factor      : expr                                 { $$ = $1; }
                 ;


expr              : expr TPLUS term                     { $$ = add_node_math( $1, $3, node_math_add ); }
                  | expr TMINUS term                    { $$ = add_node_math( $1, $3, node_math_sub ); }
                  | TMINUS term                         { $$ = add_node_math_func( $2, add_node_math_op( node_math_minus ) ); }
                  | TPLUS term                          { $$ = add_node_math_func( $2, add_node_math_op( node_math_plus ) ); }
                  | term                                { $$ = $1; }
                  ;


term              : term TMULTIPLY factor               { $$ = add_node_math( $1, $3, node_math_mul ); }
                  | term TDIVIDE factor                 { $$ = add_node_math( $1, $3, node_math_div ); }
                  | term TMOD factor                    { $$ = add_node_math( $1, $3, node_math_mod ); }
                  | factor	                            { $$ = $1; }
                  ;

factor            : TL_BRACKET expr TR_BRACKET          { $$ = $2; }
		              | function TL_BRACKET expr TR_BRACKET { $$ = add_node_math_func( $3, $1 ); }
                  | TVARIABLE                           { $$ = $1; }
                  | TCONSTANT                           { $$ = $1; }
                  | stringf                             { $$ = $1; }
                  | macro_func                          { $$ = $1; }
                  | point_expr                          { $$ = $1; }
                  | array_expr                          { $$ = $1; }
                  | random_expr                         { $$ = $1; }
                  | random_point_expr                   { $$ = $1; }
                  | array_element                       { $$ = $1; }
                  ;


function          : TSIN                                { $$ = add_node_math_op( node_math_sin ); }
                  | TCOS                                { $$ = add_node_math_op( node_math_cos ); }
                  | TTAN			                          { $$ = add_node_math_op( node_math_tan ); }
                  | TASIN                               { $$ = add_node_math_op( node_math_asin ); }
                  | TACOS      			                    { $$ = add_node_math_op( node_math_acos ); }
                  | TATAN			                          { $$ = add_node_math_op( node_math_atan ); }
                  | TLOG10			                        { $$ = add_node_math_op( node_math_log10 ); }
                  | TLN			     	                      { $$ = add_node_math_op( node_math_ln ); }
                  | TEXP			                          { $$ = add_node_math_op( node_math_exp ); }
                  ;

macro_func        : TMACRONAME TL_BRACKET TR_BRACKET    { $$ = add_node_cmd_macro( $1, NULL ); }
                  | TMACRONAME TL_BRACKET arg_list TR_BRACKET { $$ = add_node_cmd_macro( $1, $3 ); }
                  ;

stringf           : TSTRING                             { $$ = $1; }
                  | TSTRING TSTRING_FMT TL_BRACKET arg_list TR_BRACKET { $$ = add_node_constant_string_arglist( $1, $4 ); }
                  | TSTRING TSTRING_FMT simple_args     { $$ = add_node_constant_string_arglist( $1, $3 ); }
                  ;


simple_args       : TVARIABLE                          { $$ = add_node_arglist( NULL, $1 ); }
                  | TCONSTANT                          { $$ = add_node_arglist( NULL, $1 ); }
                  | stringf                            { $$ = add_node_arglist( NULL, $1 ); }
                  ;

arg_list          : r_value TCOMMA arg_list            { $$ = add_node_arglist( $3, $1 ); }
                  | r_value                            { $$ = add_node_arglist( NULL, $1 ); }
                  ;

array_expr        : TL_ARRAY array_list TR_ARRAY       { $$ = add_node_array( $2 ); }
                  | TL_ARRAY TR_ARRAY                  { $$ = add_node_array( NULL ); }
                  ;

array_list        : expr TCOMMA array_list             { $$ = add_node_array_list( $3, $1 ); }
                  | expr                               { $$ = add_node_array_list( NULL, $1 ); }
                  ;

point_expr        : TL_BRACKET expr TCOMMA expr TR_BRACKET { $$ =  add_node_point( $2, $4 ); }
                  ;

random_expr       : TRANDOM TL_BRACKET TR_BRACKET      { $$ = add_node_random(); }
                  ;

random_point_expr : TRANDOM_POINT TL_BRACKET TR_BRACKET { $$ = add_node_random_point(); }
                  ;

random_seed       : TRANDOM_SEED TL_BRACKET TR_BRACKET { $$ = add_node_random_seed( NULL ); }
                  | TRANDOM_SEED TL_BRACKET factor TR_BRACKET { $$ = add_node_random_seed( $3 ); }
                  ;

array_element     : TVARIABLE TL_ARRAY array_elements  TR_ARRAY
                  ;

array_elements    : expr
                  | expr TCOLON expr
                  ;
