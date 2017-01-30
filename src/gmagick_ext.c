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

/* gmagick_ext.c

   written by: Oliver Cordes 2016-08-12
   changed by: Oliver Cordes 2016-08-12

   */

//
// #include <assert.h>
//
// #include <wand/magick_wand.h>
//
// #define False  0
// #define True  1
//
// /*
//   Typedef declarations.
// */
// struct _MagickWand
// {
//   char
//     id[MaxTextExtent];
//
//   ExceptionInfo
//     exception;
//
//   ImageInfo
//     *image_info;
//
//   QuantizeInfo
//     *quantize_info;
//
//   Image
//     *image,             /* Current working image */
//     *images;            /* Whole image list */
//
//   unsigned int
//   iterator;
//
//  unsigned long
//    signature;
// };
//
//
// WandExport unsigned int MagickConstituteImage(MagickWand *wand,
//   const unsigned long columns,
//   const unsigned long rows,const char *map,const StorageType storage,
//   unsigned char *pixels)
// {
//   Image
//     *constitute_image = NULL;
//
//   ExceptionInfo exception;
//
//   unsigned int
//     status = True;
//
//   assert(wand != (MagickWand *) NULL);
//   assert(wand->signature == MagickSignature);
//
//   /* setup exception */
//   GetExceptionInfo( &exception );
//
//   /* create an image from Pixels */
//   constitute_image = ConstituteImage(columns,rows,map,storage,pixels,&exception);
//
//   if ( constitute_image )
//   {
//       /*(void) CompositeImage(image,CopyCompositeOp,constitute_image,x_offset,
//                             y_offset); */
//       DestroyImage( wand->image );
//       /*wand->image = constitute_image; */
//
//       AppendImageToList(&wand->images,constitute_image);
//       wand->image=GetLastImageInList( wand->images );
//
//       status = (exception.severity == UndefinedException);
//     }
//   else
//     {
//       status = False;
//     }
//
//   if (status == False)
//     CopyException(&wand->exception, &exception );
//   return(status);
// }
