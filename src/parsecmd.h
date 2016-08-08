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

/* parsecmd.h

   written by: Oliver Cordes 2014-08-30
   changed by: Oliver Cordes 2014-09-07


*/

#ifndef __parsecmd_h

#define __parsecmd_h 1


#include "parsetree.h"


parsenode *add_node_cmd_assign( parsenode *variable, parsenode *rvalue );
void       add_node_cmd_assign_global( parsenode *variable, parsenode *rvalue );
parsenode *add_node_cmd_break( void );
parsenode *add_node_cmd_crop( parsenode *varx1, parsenode *vary1, parsenode *varx2, parsenode *vary2 );
parsenode *add_node_cmd_exit( void );
parsenode *add_node_cmd_return( parsenode *value );

parsenode *add_node_cmd_image( parsenode *varx, parsenode *vary, parsenode *imagename );
parsenode *add_node_cmd_load( parsenode *variable );
parsenode *add_node_cmd_mirrorx( void );
parsenode *add_node_cmd_mirrory( void );
parsenode *add_node_cmd_mirrorxy( void );
parsenode *add_node_cmd_print( parsenode *variable );
parsenode *add_node_cmd_printf( parsenode *variable, parsenode *arguments ); 
parsenode *add_node_cmd_quit( void );
parsenode *add_node_cmd_system( parsenode *cmd );
parsenode *add_node_cmd_textfile( parsenode *varx, parsenode *vary, parsenode *font, parsenode *filename, parsenode *alpha );
parsenode *add_node_cmd_text( parsenode *varx, parsenode *vary, parsenode *font, parsenode *s, parsenode *alpha );

parsenode *add_node_cmd_macro( parsenode *func, parsenode *args );

#endif
