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
  textfile.h

  written by: Oliver Cordes 2012-11-18
  changed by: Oliver Cordes 2013-03-03
        
  $Id: textfile.h 687 2014-09-14 17:53:49Z ocordes $

 */

#ifndef __textfile_h

#define __textfile_h 1

#include "parsetree.h"


typedef struct{
  char *words;
  int   fontnr;
  int   fm_width;
  int   size_override;
} textfilefragmentdef;

typedef struct{
  int                   nrfragments;
  int                   maxfragments;
  textfilefragmentdef **fragments;
  int                   fm_width;
  int                   fm_height;
  int                   fm_asc_height;
  int                   fm_separator;
} textfilelinedef;

typedef struct{
  char             *fname;
  int               nrlines;
  int               maxlines;
  textfilelinedef **lines;
  int               fm_width;
  int               fm_height;
} textfiledef;


textfiledef *textfile_get_textfile( parsenode *filename );

textfilelinedef *textfile_get_textline( char *line );


void textfile_free_textline( textfilelinedef *textline );
void textfile_free_textfile( textfiledef *textfile );

#endif
