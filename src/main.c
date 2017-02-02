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
   main.c

   written by: Oliver Cordes 2010-06-30
   changed by: Oliver Cordes 2017-02-01


   $Id$

*/


#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <unistd.h>
#include <getopt.h>

#include <errno.h>

#include "execute.h"
#include "font.h"
#include "helpers.h"
#include "image.h"
#include "imagedef.h"
#include "output.h"
#include "project.h"
#include "scanner.h"



static struct option longopts[] = {
  { "outfile",        required_argument, NULL, 'o' },
  { "infile",         required_argument, NULL, 'i' },
  { "debug",          required_argument, NULL, 'd' },
  { "dry-run",        0,                 NULL, 'n' },
  { "output-formats", 0,                 NULL, 'l' },
  { NULL,             0,                 NULL, 0   }
};
#define options "o:d:n"


char *srcfile = NULL;
int   dry_run = 0;


/* prototype declarations */
int yyparse (void);



/* option parser */

void parse_options( int argc, char *argv[] )
{
  int ch;

  int dl;

  while ( ( ch =getopt_long( argc, argv,
			     options,  longopts, NULL  ) ) != -1 )
    {
      switch( ch )
	{
	case 'i':
	  srcfile = strdup( optarg );
	  break;
	case 'o':
	  printf( "outfile=%s\n", optarg );
	  project_set_output_filename_string( optarg );
	  break;
	case 'd':
	  dl = atoi( optarg );
	  if ( dl < 1 )
	    {
	      fprintf( stderr, "Ingnoring debug parameter '%s'!\n", optarg );
	    }
	  else
	    debug_init( dl );
	  break;
	case 'n':
	  dry_run = 1;
	  break;
  case 'l':
    image_list_output_formats();
    exit( 0 );
	case '?':
	default:
	  printf( "unknown option\n" );
	  break;
	}
    }

  argc -= optind;
  argv += optind;

  if ( argc > 0 )
    {
      if ( srcfile != NULL ) free( srcfile );
      srcfile = strdup( argv[0] );
    }

  if ( srcfile == NULL )
    {
      printf( "No source input file specified!\n" );
      exit( -1 );
    }
  else
    printf( "Source input = '%s'\n", srcfile );
}


/* the main program */

int main( int argc, char* argv[] )
{
  int    erg;
  char  *s;

  printf( "ANIMATION v%s (build %s) (C) 2010-2016 Oliver Cordes\n",
	  VERSION, BUILD );

  s = get_amx_lang_version();
  printf( " amx_lang_version=%s\n", s);
  free( s );

  image_init( argv[0] );
  project_init();
  font_init();
  imagedef_init();

  project_set_outputdir_string( "." );
  parse_options( argc, argv );

  open_parser_source( srcfile );

  printf( "Parsing file ...\n" );
  erg = yyparse();
  fclose( yyin );
  printf( "Done.\n" );
  free( srcfile );

  if ( erg != 0 )
    {
      fprintf( stderr,
	       "Parsing not successful! Abort Program!\n" );
      return 1;
    }

  erg = execute_check();

  if ( erg == 0 )
    {
      project_show();
    }
  else
    {
      fprintf( stderr, "Abort program due to configuration errors!\n" );
      return 2;
    }

  if ( dry_run == 0 )
    erg = execute_execute();
  else
    printf( "Dry run requested!\n" );

  imagedef_done();
  font_done();
  project_done();

  image_done();
  return 0;
}
