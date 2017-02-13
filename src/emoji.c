/* (C) Copyright 2017-02 by Oliver Cordes
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

/* emoji.c

   written by: Oliver Cordes 2017-02-06
   changed by: Oliver Cordes 2017-02-13

   $Id$

*/

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "config.h"


/* list of all emoji and color codes */
A
char * _em_emoji_list[] = { "\x1b[31m",             /* red */
                            "\x1b[32m",             /* green */
                            "\x1b[33m",             /* yellow */
                            "\x1b[34m",             /* blue */
                            "\x1b[35m",             /* magenta */
                            "\x1b[36m",             /* cyan */
                            "\x1b[0m",              /* col full reset */
                            "\x1b[31;1m",           /* bright red */
                            "\x1b[32;1m",           /* bright green */
                            "\x1b[33;1m",           /* bright yellow */
                            "\x1b[34;1m",           /* bright blue */
                            "\x1b[35;1m",           /* bright magenta */
                            "\x1b[36;1m",           /* bright cyan */
                            "\x1b[39;49m",          /* col reset */
                            "\x1b[32;1m✔\x1b[0m",   /* okqy */
                            "🖨 ",                   /* printer */
                            "©️",                    /* copyright */
                           NULL
};


/* list of the non emoji and color codes if there is no real
   terminal behind the output device , e.g. pipes */
char * _em_no_emoji_list[] = { "",
                               "",
                               "",
                               "",
                               "",
                               "",
                               "",
                               "",
                               "",
                               "",
                               "",
                               "",
                               "",
                               "",
                               "",   /* okay */
                               "PRINT",   /* printer */
                               "",     /* okay */
                               "(C)", /* copyright */
                            NULL
};


/* some internal variables */
int _is_real_tty = 0;
char * *_em = _em_no_emoji_list;


void emoji_init( void )
{
  _is_real_tty = isatty( fileno( stdout ) );

  if ( _is_real_tty == 1 )
    _em = _em_emoji_list;
}
