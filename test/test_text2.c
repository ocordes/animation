#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include <string.h>

#include <wand/magick_wand.h>


/*
  Typedef declarations.
*/
struct _MagickWand
{
  char
    id[MaxTextExtent];

  ExceptionInfo
    exception;

  ImageInfo
    *image_info;

  QuantizeInfo
    *quantize_info;

  Image
    *image,             /* Current working image */
    *images;            /* Whole image list */

  unsigned int
  iterator;

 unsigned long
   signature;
};


/*

gcc -o test_text test_text.c -I/sw/include/GraphicsMagick -L/sw/lib/graphicsmagick1322 -lGraphicsMagickWand -lGraphicsMagick

*/

WandExport unsigned int MagickConstituteImage(MagickWand *wand,
  const unsigned long columns,
  const unsigned long rows,const char *map,const StorageType storage,
  unsigned char *pixels)
{
  Image
    *constitute_image = NULL;

  ExceptionInfo exception;

  unsigned int
    status = true;

  assert(wand != (MagickWand *) NULL);
  assert(wand->signature == MagickSignature);

  /* setup exception */
  GetExceptionInfo( &exception );

  /* create an image from Pixels */
  constitute_image = ConstituteImage(columns,rows,map,storage,pixels,&exception);

  if ( constitute_image )
  {
      /*(void) CompositeImage(image,CopyCompositeOp,constitute_image,x_offset,
                            y_offset); */
      DestroyImage( wand->image );
      /*wand->image = constitute_image; */

      AppendImageToList(&wand->images,constitute_image);
      wand->image=GetLastImageInList( wand->images );

      status = (exception.severity == UndefinedException);
    }
  else
    {
      status = false;
    }

  if (status == false)
    CopyException(&wand->exception, &exception );
  return(status);
}



int main( int argc, char *argv[] )
{
  MagickWand          *im, *im_draw;

  unsigned char       *pixels;

  long                 width, height;

  MagickPassFail       result = MagickPass;

  DrawingWand   *dwand = NULL;
  PixelWand     *pwand = NULL;

  unsigned char text[] = "Hallo";
  int i;

  /* start here */
  InitializeMagick( *argv );

  width  = 1024;
  height = 768;

  pixels = (unsigned char*) calloc( (width * height * 3), 1);

  for (i=0;i<width*height*3;i++)
    pixels[i] = 'a';

  if ( pixels == NULL )
    printf( "Memory allocation failed!" );
  else
  {
    printf( "MagickPass=%i MagickFail=%i\n", MagickPass, MagickFail );
    printf( "True=%i False=%i\n", true, false );
    im = NewMagickWand();

    if ( im == NULL )
      printf( "NULL pointer\n" );

    //result = MagickReadImage( im, "olli.jpg" );
    result = MagickConstituteImage( im, width, height, "RBG", CharPixel, pixels );
    free( pixels );

    printf( "MagickConstituteImage result: %i\n", result );


    /* test text drawing */
    pixels = (unsigned char*) calloc( (width * height * 3), 1);

    im_draw =  NewMagickWand();
    result = MagickConstituteImage( im_draw, width, height, "RBG", CharPixel, pixels );
    free( pixels );

    /* setup the font and image*/
    dwand = NewDrawingWand();
    pwand = NewPixelWand();


    DrawSetFontSize( dwand, 12 );
    PixelSetColor( pwand, "yellow" );
    DrawSetFont( dwand, "Helvetica" ) ;
    DrawSetFillColor( dwand, pwand );

    DrawAnnotation( dwand, 10, 10, text );

    // Turn antialias on - not sure this makes a difference
    DrawSetTextAntialias( dwand, MagickTrue );

    //MagickDrawImage( im, dwand);

    DestroyPixelWand( pwand );
    DestroyDrawingWand( dwand );


    //MagickSetImageFilename( im, "test.png" );
    //MagickSetImageFormat( im, "PNG" );
    result = MagickWriteImage( im, "test.png" );

    printf( "MagickWriteImage result: %i\n", result );
  }

  DestroyMagick();
  return 0;
}
