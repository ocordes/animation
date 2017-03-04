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


/* scanner.l

   written by: Oliver Cordes 2010-06-20
   changed by: Oliver Cordes 2017-03-04

   $Id: scanner.l 677 2014-09-03 17:42:40Z ocordes $

*/


#include <stdio.h>

#include <errno.h>

#include "parsetree.h"

#include "parser.h"

#include "helpers.h"


#define ECHO {}
#define MAX_INCLUDE_DEPTH 10

int yyget_lineno  (void);
 void yyset_lineno (int  line_number );

YY_BUFFER_STATE include_stack[MAX_INCLUDE_DEPTH];
int include_stack_ptr = 0;


%}

digit                  [0-9]
letter                 [a-zA-Z]

%x comment
%x include


%option nounput
%option noinput


%%
\#.*                    /* eat anything after a comment start */

"/*"	                BEGIN(comment);

<comment>[^*\n]*        /* eat anything that's not a '*' */
<comment>"*"+[^*/\n]*   /* eat up '*'s not followed by '/'s */
<comment>\n		/* do nothing ... */
<comment>"*"+"/"	BEGIN(INITIAL);

{digit}+                yylval = add_node_constant_int( atoi( yytext ) ); return TCONSTANT;

{digit}*(("."{digit})|({digit}"."))?{digit}*([e|e][+|-]?{digit}+)? yylval = add_node_constant_double( atof( yytext ) ); return TCONSTANT;

quit                    return TQUIT;
exit                    return TEXIT;
break                   return TBREAK;

TRUE                    yylval = add_node_constant_bool( 1 ); return TCONSTANT;
(T|t)rue                yylval = add_node_constant_bool( 1 ); return TCONSTANT;
FALSE                   yylval = add_node_constant_bool( 0 ); return TCONSTANT;
(F|f)alse               yylval = add_node_constant_bool( 0 ); return TCONSTANT;
AND                     return TAND;
OR                      return TOR;

\$[A-Z][_A-Z0-9.]*      yylval = add_node_variable_r( strip_variable( yytext) ); return TVARIABLE;
[A-Z][_A-Z0-9.]*        yylval = add_node_variable( yytext ); return TLVARIABLE;

project                 return TPROJECT;
output                  return TOUTPUT;
outputfile              return TOUTPUTFILE;
outputdir               return TOUTPUTDIR;
geometry                return TGEOMETRY;
fps                     return TFPS;
bitrate                 return TBITRATE;
background              return TBACKGROUND;
blockmovies             return TBLOCKMOVIES;
overwrite               return TOVERWRITE;

global                  return TGLOBAL;

block                   return TBLOCK;
endblock                return TENDBLOCK;
window                  return TWINDOW;
endwindow               return TENDWINDOW;
postproc                return TPOSTPROC;
endpostproc             return TENDPOSTPROC;
load                    return TLOAD;
loop                    return TLOOP;
empty                   return TEMPTY;
files                   return TFILES;
static                  return TSTATIC;
print                   return TPRINT;
printf                  return TPRINTF;

property                return TPROPERTY;
system                  return TSYSTEM;

fontdef                 return TFONTDEF;
endfontdef              return TENDFONTDEF;
font                    return TFONT;
bold                    return TBOLD;
italic                  return TITALIC;
bolditalic              return TBOLDITALIC;
size                    return TSIZE;
color                   return TCOLOR;
colour                  return TCOLOR;
separator               return TSEPARATOR;
sep                     return TSEPARATOR;

imagedef                return TIMAGEDEF;
endimagedef             return TENDIMAGEDEF;
file                    return TFILE;
rescale                 return TRESCALE;
resize                  return TRESCALE;
image                   return TIMAGE;
mode                    return TMODE;

control                 return TCONTROL;
endcontrol              return TENDCONTROL;

macro                   return TMACRO;
endmacro                return TENDMACRO;
return                  return TRETURNMACRO;

pendef                  return TPENDEF;
endpendef               return TENDPENDEF;
filldef                 return TFILLDEF;
endfilldef              return TENDFILLDEF;

crop                    return TCROP;
mirrorx                 return TMIRRORX;
mirrory                 return TMIRRORY;
mirrorxy                return TMIRRORXY;
textfile                return TTEXTFILE;
textfilealpha           return TTEXTFILEALPHA;
text                    return TTEXT;
textalpha               return TTEXTALPHA;
circle                  return TCIRCLE;
rectangle               return TRECTANGLE;
rectanglefill           return TRECTANGLEFILL;


include                 BEGIN(include);
<include>[ \r\t]+       /* eat up all white spaces */;
<include>[^ \r\t\n]+    { /* got the include file name */

		          if ( include_stack_ptr >= MAX_INCLUDE_DEPTH )
		          {
		            fprintf( stderr, "Includes nested too deeply" );
		            exit( 1 );
		          }
		          include_stack[include_stack_ptr++] = YY_CURRENT_BUFFER;
                          push_incl_savings( yy_filename, yyget_lineno() );
                          yyset_lineno( 1 );
                          open_parser_source( strdup( strip_string( yytext ) ) );
                          printf( "Switching to '%s'... \n", yy_filename );
		          yy_switch_to_buffer( yy_create_buffer( yyin, YY_BUF_SIZE ) );
		          BEGIN(INITIAL);
		        }
<<EOF>>                 {
                          int lineno;

		          if ( --include_stack_ptr < 0 )
		          {
		            yyterminate();
		          }
		          else
		          {
		            yy_delete_buffer( YY_CURRENT_BUFFER );
		            yy_switch_to_buffer( include_stack[include_stack_ptr] );
                            pop_incl_savings( &yy_filename, &lineno );
                            yyset_lineno( lineno );

                            printf( "Return to previous file `%s' (line %i) \n", yy_filename, lineno );
		          }
		        }

[\"|\'][^\"\t\n]+[\"|\']  yylval = add_node_constant_string( strip_string( yytext ) ); return TSTRING;

,                       return TCOMMA;
=                       return TASSIGN;
\+                      return TPLUS;
-                       return TMINUS;
\*                      return TMULTIPLY;
"/"                     return TDIVIDE;
mod                     return TMOD;
"%"                     return TSTRING_FMT;
if                      return TIF;
endif                   return TENDIF;
else                    return TELSE;
"=="                    return TEQ;
"!="                    return TNEQ;
">"                     return TGREATER;
"<"                     return TLOWER;
">="                    return TGREQ;
"<="                    return TLOEQ;
";"                     return TSEMICOLON;
"("                     return TL_BRACKET;
")"                     return TR_BRACKET;
"["                     return TL_ARRAY;
"]"                     return TR_ARRAY;
"&&"                    return TAND;
\|\|                    return TOR;
"!"                     return TNOT;
sin                     return TSIN;
cos                     return TCOS;
tan                     return TTAN;
asin                    return TASIN;
acos                    return TACOS;
atan                    return TATAN;
log10                   return TLOG10;
ln                      return TLN;
exp                     return TEXP;
\n                      return TRETURN;
[ \r\t]+                /* ignore whitespace */;
.                       return TUNKNOWN;


[a-z][_a-z0-9.]*         yylval =  add_node_constant_string( yytext ); return TMACRONAME;

%%
