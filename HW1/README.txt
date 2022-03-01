Dan Moran

Program named p1 that converts a gray–level image to a binary one using a threshold value:

p1 {input gray–level image} {input gray–level threshold} {output binary image}

Select any threshold that results in ”clean” binary images from the gray–level ones given
to you (a binary image can be saved as a PGM file with 1 as the number of colors in the
header). You should be able to use the same threshold value for all images.

----------------------
To compile in Linux:
----------
 
   make all

To run:
---------

./image_demo objects.pgm objects_output.pgm

This will open image objects.pgm, will draw a line on the image, and will save resulting image to file objects_output.pgm

-----------

To view .pgm files you can use the open source program gimp:

https://www.gimp.org/
