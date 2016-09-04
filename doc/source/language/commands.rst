Commands / Functions
====================

.. function:: project "name"

  This sets the project name to *name*.

.. function:: output "output_format"

  *(global)*

  This method sets the format of the output sequence. Valid formats
  are:

  .. list-table::
     :widths: 15 50
     :header-rows: 1

     * - Format
       - Description
     * - ffmpeg
       - avi output via ffmpeg
     * - jpeg
       - image sequence in *JPEG* format
     * - png
       - image sequence in *PNG* format
