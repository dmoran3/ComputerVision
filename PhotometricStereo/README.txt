
Sample code that reads/writes pgm images and that draws lines on the images is provided.

image.* : Image class (2-D array of integers, along with size, number of colors)
                      (For our purposes the number of colors is 256)

image_demo.cc : Sample main() function for testing.

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
