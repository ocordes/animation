Block definitions
=================

.. index:: Blocks

Animation uses blocks of images to form a video sequence. Typically one can put
all images for one block in one directory. Inside a single block all images will
be processed with the same routines and commands, e.g. fade in, fade out, image
manipulations etc. There is one special block which is used for post processing
in which all images can be manipulated again. This is useful e.g. for generating
water marks and a duration counter or simply copyright information.

Normal blocks
-------------

.. index:: Normal blocks


Normal blocks consists at first of image definitions in the form of a loop command or loop commands.
The sequence of images should be of the same size but can be mixture of different files, static files or
empty files. The there is a load command, which prepares a image for further processing and the program
around to do the actual processing.

#. Define the images

   .. index:: loop (empty)

   .. function:: loop empty nr_images [dim_x dim_y [fps]]

     This command generate empty file for processing. These files can be used for titles and
     other stuff. dim_x and dim_y are optional and can set with number of variables. If no
     dimensions are given, animation uses the dimension of the output images. fps is optional
     to create some frames per second feeling (not implemented yet!)

     Empty files are created with the background color, defined in the global part of the animation file.

   .. index:: loop (files)

   .. function:: loop files pattern [fps]

     This loop variant includes all files matching the pattern string. fps is optional
     to create some frames per second feeling (not implemented yet!)


   .. index:: loop (static)

   .. function:: loop static file nr_images

      This loop generates nr_images all based on the same static image which is given in file.


#. Load the images

   In each image loop of the block sequence the current image filename is stored in *$FILE* .

   .. index:: load

   .. function:: load variable

      Loads the image given by VARIABLE . It is highly recommended to use the variable *$FILE* .


#. Processing

   You can use the whole list of commands available for animation ...


#. Examples

   This is a minimal example, which generate 10s empty files: ::

      ...
      block "ex1"
         loop empty 250
         load $FILE
      endblock


#. Processing commands

Post-Processing block
---------------------

.. index:: Post processing block
