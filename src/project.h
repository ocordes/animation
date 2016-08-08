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

/* project.h

   written by: Oliver Cordes 2010-07-02
   changed by: Oliver Cordes 2014-09-07

   $Id: project.h 679 2014-09-07 17:32:05Z ocordes $

*/

#ifndef __project_h 
#define __project_h 1


#include "blocks.h"
#include "controls.h"
#include "macro.h"
#include "parsetree.h"
#include "variable.h"




extern variables   *local_vars[];


#define parser_status_default 0
#define parser_status_block   1
#define parser_status_window  2
#define parser_status_macro   3

#define check_command_status(status,command) { if ( check_parser_status( status, command) == 0 ) return; }

int get_parser_status( void );
int push_parser_status( int );
int pop_parser_status( void );
int check_parser_status( int status, char *command );



typedef struct{
  char        *name;
  char        *output_filename;
  char        *outputdir;
  char        *output_device;
  int          output_type;
  int          geometry[2];
  char        *geometry_name;
  int          fps;
  int          bitrate;
  char        *background;
  /* block definitions */
  int          nrblocks;
  blockdef    *blocks[max_blocks];
  blockdef    *postproc_block;
  variables   *vars;                 /* global variables */
  /* macro definitions */
  int          nrmacros;
  macrodef    *macros[max_macros];
  /* control infos */
  int          nrcontrols;
  controlsdef *controls[max_controls];
  /* some extra flags */
  int          blockmovies;           /* default 0?False */
  int          overwrite;             /* default 1=True */
  /* runtime  informations */
  int          framenr;
} projectdef;


extern projectdef  *main_project;


void project_init( void );
void project_done( void );
void project_show( void );

int project_sanity_check();


/* project setting routines */

void project_set_project_name( parsenode* );
void project_set_output_device( parsenode* );
void project_set_output_filename( parsenode* );
void project_set_output_filename_string( char* );
void project_set_outputdir_string( char* );
void project_set_outputdir( parsenode* );
void project_set_geometry( parsenode* );
void project_set_fps( parsenode* );
void project_set_background( parsenode* );
void project_set_bitrate( parsenode* );
void project_set_blockmovies( parsenode * );
void project_set_overwrite( parsenode * );

void project_add_block( blockdef *newblock );
void project_add_macro( macrodef *newmacro );
macrodef *project_get_macro( parsenode *func );

void project_push_local_variables( variables *vars );
void project_pop_local_variables( void );


#endif
