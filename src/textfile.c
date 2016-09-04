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
  textfile.c

  written by: Oliver Cordes 2012-11-18
  changed by: Oliver Cordes 2016-09-04

  $Id: textfile.c 687 2014-09-14 17:53:49Z ocordes $

 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <errno.h>

#include <assert.h>


#include "config.h"


#include "amath.h"
#include "output.h"
#include "project.h"
#include "textfile.h"
#include "variable.h"

#define increment_fragments 5
#define increment_lines     5


/* tags */
#define tag_unknown        -1
#define tag_open_bold      1
#define tag_open_italics   2
#define tag_open_size      3
#define tag_close_bold     4
#define tag_close_italics  5
#define tag_close_size     6


/* font sizes during text parsing */

#define max_font_size_tags 100
int    font_size_tags[max_font_size_tags];
int    font_size_tag_count = 0;


/* textfile cache */

struct __textfile_cache{
  char                   *filename;
  textfiledef            *textfile;
  struct __textfile_cache *next;
};

typedef struct __textfile_cache _textfile_cache;

_textfile_cache *textfile_cache = NULL;



/* font_size_tags functions */

int get_font_size_tag( void )
{
  if ( font_size_tag_count > 0 )
    return font_size_tags[font_size_tag_count-1];
  else
    return -1;
}


void push_font_size_tag( int size )
{
  if ( font_size_tag_count < max_font_size_tags )
    font_size_tags[font_size_tag_count] = size;
  else
    output( 1, "Warning: Can't push new size variable!\n" );

  font_size_tag_count++;
}


void pop_font_size_tag( void )
{
  font_size_tag_count--;
  if ( font_size_tag_count < 0 )
    font_size_tag_count = 0;
}


/* textfile cache functions */

void textfile_cache_add_textfile( char *filename, textfiledef *textfile )
{
  _textfile_cache *newentry;

  newentry = malloc( sizeof( _textfile_cache ) );

  assert( newentry != NULL );

  newentry->filename = strdup( filename );
  newentry->textfile = textfile;
  newentry->next     = textfile_cache;

  textfile_cache = newentry;
}


textfiledef *textfile_cache_get_textfile( char *filename )
{
  struct __textfile_cache *start;

  start = textfile_cache;

  while ( start != NULL )
    {
      if ( strcmp( start->filename, filename ) == 0 )
	return start->textfile;
      start = start->next;
    }

  return NULL;
}


/* helper functions for splitting the text */

int status2fontnr( int bold, int italics )
{
  if ( bold == 1 )
    {
      if ( italics == 1 )
	return 3;
      else
	return 1;
    }
  else
    {
      if ( italics == 1 )
	return 2;
      else
	return 0;
    }
}


int str2tag( char *p, int *tagval )
{
  int tag;

  char *p2, *brk, *val;


  tag = tag_unknown;

  if ( p == NULL )
    return tag;

  switch( p[0] )
    {
    case 'b':
      output( 2, "open bold\n" );
      tag = tag_open_bold;
      break;
    case 'i':
      output( 2, "open italics\n" );
      tag = tag_open_italics;
      break;
    case 's':
      p2 = strtok_r( p, "=\0", &brk );
      if ( p2 != NULL )
	{
	  val = strtok_r( NULL, "=\0", &brk );
	  if ( strcmp( p2, "size" ) == 0 )
	    {
	      if ( val != NULL )
		{
		  (*tagval) = atoi( val );
		  tag = tag_open_size;
		}
	    }
	}
      break;
    case '/':
      switch( p[1] )
	{
	case 'b':
	  output( 2, "close bold\n" );
	  tag = tag_close_bold;
	  break;
	case 'i':
	  output( 2, "close italics\n" );
	  tag = tag_close_italics;
	  break;
	case 's':
	  p2 = strtok_r( p+1, "=\0", &brk );
	  if ( p2 != NULL )
	    {
	      if ( strcmp( p2, "size" ) == 0 )
		tag = tag_close_size;
	    }
	}
      break;
    default:
      output( 2, "unknown tag\n" );
      tag =  tag_unknown;
    }

  return tag;
}


char *next_tag( char *line, char **s, int *tag, int *tagval )
{
  char *brk;

  char *p;

  (*tag) = tag_unknown;
  if ( line[0] == '<' )
    {
      line++;
      (*s) = NULL;
      p = strtok_r( line, ">\0", &brk );
      if ( p == NULL )
	return NULL;
      (*tag) = str2tag( p, tagval );
    }
  else
    {
      p = strtok_r( line, "<\0", &brk );
      if ( p != NULL )
	(*s) = strdup( p );
      else
	(*s) = strdup( line );
      p = strtok_r( NULL, ">\0", &brk );
      if ( p == NULL )
	return NULL;
      (*tag) = str2tag( p, tagval );
    }
  return strtok_r( NULL, "\0", &brk );
}


/* textfile structure commands */


textfilefragmentdef *textfile_new_fragment( char *words, int fontnr, int size_override )
{
  textfilefragmentdef *fragment;

  fragment = malloc( sizeof( textfilefragmentdef ) );

  assert( fragment != NULL );

  fragment->words         = strdup( words );
  fragment->fontnr        = fontnr;
  fragment->fm_width      = -1;
  fragment->size_override = size_override;

  return fragment;
}


void textfile_free_fragment( textfilefragmentdef *fragment )
{
  if ( fragment != NULL )
    {
      free( fragment->words );
      free( fragment );
    }
}


textfilelinedef *textfile_new_line( void )
{
  textfilelinedef *textline;

  textline               = malloc( sizeof( textfilelinedef ) );

  assert( textline != NULL );

  textline->fragments    = malloc( sizeof( textfilefragmentdef* ) * increment_fragments );

  assert( textline->fragments != NULL );

  textline->nrfragments  = 0;
  textline->maxfragments = increment_fragments;
  textline->fm_width     = -1;
  textline->fm_height    = -1;

  return textline;
}


textfiledef *textfile_new_textfile( char *fname )
{
  textfiledef *textfile;

  textfile            = malloc( sizeof( textfiledef ) );

  assert( textfile != NULL );

  textfile->lines     = malloc( sizeof( textfilelinedef* ) * increment_lines ) ;

  assert( textfile->lines != NULL );

  textfile->nrlines   = 0;
  textfile->maxlines  = increment_lines;
  textfile->fname     = strdup( fname );
  textfile->fm_width  = -1;
  textfile->fm_height = -1;

  return textfile;
}


void textfile_free_textline( textfilelinedef *textline )
{
  int i;

  if ( textline != NULL )
    {
      for (i=0;i<textline->nrfragments;i++)
	textfile_free_fragment( textline->fragments[i] );
      free( textline->fragments );
      free( textline );
    }
}


void textfile_free_textfile( textfiledef *textfile )
{
  int i;

  if ( textfile != NULL )
    {
      for (i=0;i<textfile->nrlines;i++)
	textfile_free_textline( textfile->lines[i] );
      free( textfile->lines );
      free( textfile );
    }
}


void textfile_add_linefragment( textfilelinedef *textline, textfilefragmentdef *newfragment )
{
  textfilefragmentdef **newfragments;

  int               i;


  assert( textline != NULL );

  if ( textline->nrfragments == textline->maxfragments )
    {
      textline->maxfragments += increment_fragments;
      newfragments = malloc( sizeof( textfilefragmentdef *) * textline->maxfragments );

      assert( newfragments != NULL );

      for (i=0;i<textline->nrfragments;i++)
	newfragments[i] = textline->fragments[i];
      free( textline->fragments );
      textline->fragments = newfragments;
    }

  textline->fragments[textline->nrfragments] = newfragment;
  textline->nrfragments++;
}


void textfile_add_line( textfiledef *textfile, textfilelinedef *newline )
{
  textfilelinedef **newlines;

  int               i;


  assert( textfile != NULL );

  if ( textfile->nrlines == textfile->maxlines )
    {
      textfile->maxlines += increment_lines;
      newlines = malloc( sizeof( textfilelinedef *) * textfile->maxlines );

      assert( newlines != NULL );

      for (i=0;i<textfile->nrlines;i++)
	newlines[i] = textfile->lines[i];
      free( textfile->lines );
      textfile->lines = newlines;
    }

  textfile->lines[textfile->nrlines] = newline;
  textfile->nrlines++;
}





/* split a ASCII line into fragments with TAGS */

textfilelinedef *create_textfile_line( char *line )
{
  textfilelinedef *newline;

  int                  tag;
  int                  tagval;
  char                *words;
  char                *p;
  int                  bold = 0;
  int                  italics = 0;

  textfilefragmentdef *fragment;


  newline = textfile_new_line();

  p = line;
  do{
    p = next_tag( p, &words, &tag, &tagval );

    if ( words != NULL )
      {
	if ( p != NULL )
	  output( 2, ">>%s<< tag=%i >>%s<<\n", words, tag, p );
	else
	  output( 2, ">>%s<< tag=%i endline\n", words, tag );
      }
    else
      {
	if ( p != NULL )
	  output( 2, "tag=%i >>%s<<\n", tag, p );
	else
	  output( 2, "tag=%i endline\n", tag );
      }

    if ( words != NULL )
      {
	/* add text fragment with the current flags */
	fragment = textfile_new_fragment( words,
					  status2fontnr( bold, italics ),
					  get_font_size_tag() );
	free( words );
	words = NULL;
	textfile_add_linefragment( newline, fragment );
      }

    switch( tag )
      {
      case tag_open_bold:
	if ( bold == 1 )
	  output( 1, "Warning: Bold tag already opened! New tag will be ignored!\n" );
	else
	  bold = 1;
	break;
      case tag_open_italics:
	if ( italics == 1 )
	  output( 1, "Warning: Italics tag already opened! New tag will be ignored!\n" );
	else
	  italics = 1;
	break;
      case tag_close_bold:
	if ( bold == 0 )
	  output( 1, "Warning: Bold tag already closed! New tag will be ignored!\n" );
	else
	  bold = 0;
	break;
      case tag_close_italics:
	if ( italics == 0 )
	  output( 1, "Warning: Bold tag already closed! New tag will be ignored!\n" );
	else
	  italics = 0;
	break;
      case tag_open_size:
	push_font_size_tag( tagval );
	break;
      case tag_close_size:
	pop_font_size_tag();
	break;
      }
  }
  while ( p != NULL );

  return newline;
}


textfiledef *create_textfile_data( char *filename )
{
  textfiledef     *textfile;
  textfilelinedef *textline;

  FILE        *file;
  char         line[1000];

  output( 1, "Reading textfile '%s' ...\n", filename );

  textfile = textfile_new_textfile( filename );

  file = fopen( filename, "r" );

  if ( file != NULL )
  {
    while( fgets( line, sizeof( line )-2, file ) != NULL )
    {
      line[strlen(line)-1] = '\0';
      textline = create_textfile_line( line );
      textfile_add_line( textfile, textline );
    }

    fclose( file );

    textfile_cache_add_textfile( filename, textfile );

    output( 1, "Done.\n" );
  }
  else
  {
    output( 1, "Can't load textfile '%s' (%s)!\n", filename, strerror( errno ));
  }

  return textfile;
}


textfiledef *create_textfile_data_cached( char *filename )
{
  textfiledef *textfile;

  textfile = textfile_cache_get_textfile( filename );
  if ( textfile == NULL )
    textfile = create_textfile_data( filename );
  else
    {
      output( 1, "Reusing textfile '%s'!\n", filename );
    }

  return textfile;
}


textfiledef *textfile_get_textfile( parsenode *filename )
{
  char        *s;
  textfiledef *textfile;

  s = math_execute_node_string( filename );

  textfile = create_textfile_data_cached( s );


  /* free variables */
  free( s );

  return textfile;
}



textfilelinedef *textfile_get_textline( char *line )
{
  return create_textfile_line( line );
}
