// Class for representing a 2D gray-scale image,
// with support for reading/writing pgm images.
// To be used in Computer Vision class.

#include "image.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <vector>

using namespace std;

namespace ComputerVisionProjects {

Image::Image(const Image &an_image){
  AllocateSpaceAndSetSize(an_image.num_rows(), an_image.num_columns());
  SetNumberGrayLevels(an_image.num_gray_levels());

  for (size_t i = 0; i < num_rows(); ++i)
    for (size_t j = 0; j < num_columns(); ++j){
      SetPixel(i,j, an_image.GetPixel(i,j));
    }
}

Image::~Image(){
  DeallocateSpace();
}

void Image::AllocateSpaceAndSetSize(size_t num_rows, size_t num_columns) {
  if (pixels_ != nullptr) DeallocateSpace();
  pixels_ = new int*[num_rows];
  for (size_t i = 0; i < num_rows; ++i)
    pixels_[i] = new int[num_columns];

  num_rows_ = num_rows;
  num_columns_ = num_columns;
}

void Image::DeallocateSpace() {
  for (size_t i = 0; i < num_rows_; i++)
    delete pixels_[i];
  delete pixels_;
  pixels_ = nullptr;
  num_rows_ = 0;
  num_columns_ = 0;
}

bool ReadImage(const string &filename, Image *an_image) {  
  if (an_image == nullptr) abort();
  FILE *input = fopen(filename.c_str(),"rb");
  if (input == 0) {
    cout << "ReadImage: Cannot open file" << endl;
    return false;
  }
  
  // Check for the right "magic number".
  char line[1024];
  if (fread(line, 1, 3, input) != 3 || strncmp(line,"P5\n",3)) {
    fclose(input);
    cout << "ReadImage: Expected .pgm file" << endl;
    return false;
  }
  
  // Skip comments.
  do
    fgets(line, sizeof line, input);
  while(*line == '#');
  
  // Read the width and height.
  int num_columns,num_rows;
  sscanf(line,"%d %d\n", &num_columns, &num_rows);
  an_image->AllocateSpaceAndSetSize(num_rows, num_columns);
  

  // Read # of gray levels.
  fgets(line, sizeof line, input);
  int levels;
  sscanf(line,"%d\n", &levels);
  an_image->SetNumberGrayLevels(levels);

  // read pixel row by row.
  for (int i = 0; i < num_rows; ++i) {
    for (int j = 0;j < num_columns; ++j) {
      const int byte=fgetc(input);
      if (byte == EOF) {
        fclose(input);
        cout << "ReadImage: short file" << endl;
        return false;
      }
      an_image->SetPixel(i, j, byte);
    }
  }
  
  fclose(input);
  return true; 
}

bool WriteImage(const string &filename, const Image &an_image) {  
  FILE *output = fopen(filename.c_str(), "w");
  if (output == 0) {
    cout << "WriteImage: cannot open file" << endl;
    return false;
  }
  const int num_rows = an_image.num_rows();
  const int num_columns = an_image.num_columns();
  const int colors = an_image.num_gray_levels();

  // Write the header.
  fprintf(output, "P5\n"); // Magic number.
  fprintf(output, "#\n");  // Empty comment.
  fprintf(output, "%d %d\n%03d\n", num_columns, num_rows, colors);

  for (int i = 0; i < num_rows; ++i) {
    for (int j = 0; j < num_columns; ++j) {
      const int byte = an_image.GetPixel(i , j);
      if (fputc(byte,output) == EOF) {
	    fclose(output);
            cout << "WriteImage: could not write" << endl;
	    return false;
      }
    }
  }

  fclose(output);
  return true; 
}

void MakeBinary(int threshold, Image *an_image){
	if (an_image == nullptr) abort();
	// Start the scan.
	size_t x = 0;
	size_t y = 0;
	size_t x_max = an_image->num_rows();
	size_t y_max = an_image->num_columns();
	int done = 0;
	int color;
	while (!done) {
		if(an_image->GetPixel(x,y) < threshold)
		{
			color = 0;
		} else {
			color = 255;
		}
		
		an_image->SetPixel(x,y,color);

		if (x < x_max-1) {
			x++;
		}
		else{ //at end of line
			if(y == y_max-1){ //at end of image
				done = 1;
			}
			x = 0;
			y++;
		}
	}
}

void LabelBinaryImage(Image *an_image){
	
	if (an_image == nullptr) abort();
	// Start the scan.
	size_t x = 1;
	size_t y = 1;
	size_t x_max = an_image->num_rows();
	size_t y_max = an_image->num_columns();
	size_t label = 1;
	int first_pass = 0;
	int second_pass = 0;
	int third_pass = 0;
	int colors[x_max][2];
	int counter = 0;
	
	cout<< "SCANNING" <<endl;
	while (!first_pass) {
		
		if(an_image->GetPixel(x,y) != 0 ){  //pixel is not background
			if(an_image->GetPixel(x,y-1) != 0 || an_image->GetPixel(x-1,y-1) != 0 || an_image->GetPixel(x-1,y) != 0){
				 //set pixel to max label of pixels to: Left, Left and Up, Up (this is to prevent a 0 since it only needs one value to be nonzero
				an_image->SetPixel(x,y,max(max(an_image->GetPixel(x,y-1),an_image->GetPixel(x-1,y-1)),an_image->GetPixel(x-1,y)));
			}
			else{
				label++;
				an_image->SetPixel(x,y,label);
			}
			
		}
	
		if (x < x_max-1) {
			x++;
		}
		else{ //at end of line
			if(y == y_max-1){ //at end of image
				first_pass = 1;
			}
			x = 0;
			y++;
		}
	}
	
	x = x_max-1;
	y = y_max-1;

	cout<< "LABELING" <<endl;
	while (!second_pass) {
		
		bool not_in = true;
		if(an_image->GetPixel(x,y) != 0){  //pixel is not background
			if((an_image->GetPixel(x,y) != an_image->GetPixel(x-1,y)) && an_image->GetPixel(x-1,y) != 0){ //pixel to left is labelled
				for(int i = 0; i < counter; i++){
					//check if in equiv_table already. It is built with left column as lower value for easy sorting
					if(colors[i][0] == min(an_image->GetPixel(x,y),an_image->GetPixel(x-1,y)) && colors[i][1] == max(an_image->GetPixel(x,y),an_image->GetPixel(x-1,y))){
						not_in = false;
					}
				}
				if(not_in){
					colors[counter][0] = min(an_image->GetPixel(x,y),an_image->GetPixel(x-1,y));
					colors[counter][1] = max(an_image->GetPixel(x,y),an_image->GetPixel(x-1,y));
					//~ cout << counter << " -> " << colors[counter][0] << " ||| " << colors[counter][1] << endl;
					counter++;
				}                                                                                                                                                                                                                             
			}
			if((an_image->GetPixel(x,y) != an_image->GetPixel(x-1,y-1)) && an_image->GetPixel(x-1,y-1) != 0){ //pixel to left is labelled
				for(int i = 0; i < counter; i++){
					//check if in equiv_table already. It is built with left column as lower value for easy sorting
					if(colors[i][0] == min(an_image->GetPixel(x,y),an_image->GetPixel(x-1,y-1)) && colors[i][1] == max(an_image->GetPixel(x,y),an_image->GetPixel(x-1,y-1))){
						not_in = false;
					}
				}
				if(not_in){
					colors[counter][0] = min(an_image->GetPixel(x,y),an_image->GetPixel(x-1,y-1));
					colors[counter][1] = max(an_image->GetPixel(x,y),an_image->GetPixel(x-1,y-1));
					//~ cout << counter << " -> " << colors[counter][0] << " ||| " << colors[counter][1] << endl;
					counter++;
				}
			}
			if((an_image->GetPixel(x,y) != an_image->GetPixel(x,y-1)) && an_image->GetPixel(x,y-1) != 0){ //pixel to left is labelled
				for(int i = 0; i < counter; i++){
					//check if in equiv_table already. It is built with left column as lower value for easy sorting
					if(colors[i][0] == min(an_image->GetPixel(x,y),an_image->GetPixel(x,y-1)) && colors[i][1] == max(an_image->GetPixel(x,y),an_image->GetPixel(x,y-1))){
						not_in = false;
					}
				}
				if(not_in){
					colors[counter][0] = min(an_image->GetPixel(x,y),an_image->GetPixel(x,y-1));
					colors[counter][1] = max(an_image->GetPixel(x,y),an_image->GetPixel(x,y-1));
					//~ cout << counter << " -> " << colors[counter][0] << " ||| " << colors[counter][1] << endl;
					counter++;
				}
			}
		}			
		
		if (x > 2) {
			x--;
		}
		else{ //at end of line
			if(y == 1){ //at end of image
				second_pass = 1;
			}
			x = x_max-1;
			y--;
		}
	}
	x = x_max-1;
	y = y_max-1;
	cout << "COLORING" << endl;
	an_image->SetNumberGrayLevels(counter);
	while(!third_pass){
		if(an_image->GetPixel(x,y) != 0){  //pixel is not background
			for(int i = counter-1; i >=0; i--){
				if(colors[i][0] == an_image->GetPixel(x,y))
				{
					an_image->SetPixel(x,y,colors[i][1]);
				}
			}
		}		
		if (x > 0) {
			x--;
		}
		else{ //at end of line
			if(y == 0){ //at end of image
				third_pass = 1;
			}
			x = x_max-1;
			y--;
		}
	}
				
}

// Implements the Bresenham's incremental midpoint algorithm;
// (adapted from J.D.Foley, A. van Dam, S.K.Feiner, J.F.Hughes
// "Computer Graphics. Principles and practice", 
// 2nd ed., 1990, section 3.2.2);  
void DrawLine(int x0, int y0, int x1, int y1, int color, Image *an_image) {  
  if (an_image == nullptr) abort();

#ifdef SWAP
#undef SWAP
#endif
#define SWAP(a,b) {a^=b; b^=a; a^=b;}

  const int DIR_X = 0;
  const int DIR_Y = 1;
  
  // Increments: East, North-East, South, South-East, North.
  int incrE,
    incrNE,
    incrS,
    incrSE,
    incrN;     
  int d;         /* the D */
  int x,y;       /* running coordinates */
  int mpCase;    /* midpoint algorithm's case */
  int done;      /* set to 1 when done */
  
  int xmin = x0;
  int xmax = x1;
  int ymin = y0;
  int ymax = y1;
  
  int dx = xmax - xmin;
  int dy = ymax - ymin;
  int dir;

  if (dx * dx > dy * dy) {  // Horizontal scan.
    dir=DIR_X;
    if (xmax < xmin) {
      SWAP(xmin, xmax);
      SWAP(ymin , ymax);
    } 
    dx = xmax - xmin;
    dy = ymax - ymin;

    if (dy >= 0) {
      mpCase = 1;
      d = 2 * dy - dx;      
    } else {
      mpCase = 2;
      d = 2 * dy + dx;      
    }

    incrNE = 2 * (dy - dx);
    incrE = 2 * dy;
    incrSE = 2 * (dy + dx);
  } else {// vertical scan.
    dir = DIR_Y;
    if (ymax < ymin) {
      SWAP(xmin, xmax);
      SWAP(ymin, ymax);
    }
    dx = xmax - xmin;
    dy = ymax-ymin;    

    if (dx >=0 ) {
      mpCase = 1;
      d = 2 * dx - dy;      
    } else {
      mpCase = 2;
      d = 2 * dx + dy;      
    }

    incrNE = 2 * (dx - dy);
    incrE = 2 * dx;
    incrSE = 2 * (dx + dy);
  }
  
  /// Start the scan.
  x = xmin;
  y = ymin;
  done = 0;

  while (!done) {
    an_image->SetPixel(x,y,color);
  
    // Move to the next point.
    switch(dir) {
    case DIR_X: 
      if (x < xmax) {
	      switch(mpCase) {
	      case 1:
		if (d <= 0) {
		  d += incrE;  
		  x++;
		} else {
		  d += incrNE; 
		  x++; 
		  y++;
		}
		break;
  
            case 2:
              if (d <= 0) {
                d += incrSE; 
		x++; 
		y--;
              } else {
                d += incrE;  
		x++;
              }
	      break;
	      } 
      } else {
	done=1;
      }     
      break;

    case DIR_Y: 
        if (y < ymax) {
          switch(mpCase) {
	  case 1:
	    if (d <= 0) {
	      d += incrE;  
	      y++;
	    } else {
	      d += incrNE; 
	      y++; 
	      x++;
	    }
            break;
  
	  case 2:
	    if (d <= 0) {
                d += incrSE; 
		y++; 
		x--;
              } else {
                d += incrE;  
		y++;
	    }
            break;
	  } // mpCase
        } // y < ymin 
        else {
	  done=1;
	}
	break;    
    }
  }
}

}  // namespace ComputerVisionProjects







