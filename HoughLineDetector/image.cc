// Class for representing a 2D gray-scale image,
// with support for reading/writing pgm images.
// To be used in Computer Vision class.

#include "image.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
#include <math.h>
#include <fstream>

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

// Implements the Bresenham's incremental midpoint algorithm;
// (adapted from J.D.Foley, A. van Dam, S.K.Feiner, J.F.Hughes
// "Computer Graphics. Principles and practice", 
// 2nd ed., 1990, section 3.2.2);  
void DrawLine(int x0, int y0, int x1, int y1, int color,
	      Image *an_image) {  
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




//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
// DAN MORAN - COMPUTER VISION HW 3 - MARCH 2022
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//


//Masks applied will result in a grid that looks like this
// p1 p2 p3
// p4 p5 p6  centered around p5
// p7 p8 p9

void GenerateEdgeImage(Image *an_image){
	//i is rows, j is cols
	
	const int i_max = an_image->num_rows();
	const int j_max = an_image->num_columns();
	int max;
	int min;
	int edge_map[i_max][j_max];
	
	for(int j = 1; j < j_max-1; j+=2){
		for(int i = 1; i < i_max-1; i+=2){
			//top row
			int p1 = an_image->GetPixel(i-1,j-1) * -1;
			int p2 = an_image->GetPixel(i-1,j) * 0;
			int p3 = an_image->GetPixel(i-1,j+1) * 1;
			//middle row
			int p4 = an_image->GetPixel(i,j-1) * -2;
			int p5 = an_image->GetPixel(i,j) * 0;
			int p6 = an_image->GetPixel(i,j+1) * 2;
			//bottom row
			int p7 = an_image->GetPixel(i+1,j-1) * -1;
			int p8 = an_image->GetPixel(i+1,j) * 0;
			int p9 = an_image->GetPixel(i+1,j+1) * 1;
			
			//calculate dx from horizontal sobel mask
			int dx = p1 + p2 + p3 + p4 + p5 + p6 + p7 + p8 + p9;
			
			//top row
			p1 = an_image->GetPixel(i-1,j-1) * 1;
			p2 = an_image->GetPixel(i-1,j) * 2;
			p3 = an_image->GetPixel(i-1,j+1) * 1;
			//middle row
			p4 = an_image->GetPixel(i,j-1) * 0;
			p5 = an_image->GetPixel(i,j) * 0;
			p6 = an_image->GetPixel(i,j+1) * 0;
			//bottom row
			p7 = an_image->GetPixel(i+1,j-1) * -1;
			p8 = an_image->GetPixel(i+1,j) * -2;
			p9 = an_image->GetPixel(i+1,j+1) * -1;
			
			//calculate dy from vertical sobel mask
			int dy = p1 + p2 + p3 + p4 + p5 + p6 + p7 + p8 + p9;
			
			//get the magnitude at each pixel			
			int magnitude = sqrt((dx * dx) + (dy * dy));
			if(magnitude > max) max = magnitude;
			if(magnitude < min) min = magnitude;			
			
			//place values into an edge map array
			edge_map[i][j] = magnitude;
		}
	}	

	//
	// CAN WE REMOVE THIS DIFF AND MAX?
	//
	int diff = max;
	for(int j = 1; j < j_max-1; j++){
		for(int i = 1; i < i_max-1; i++){
			//set color of pixel to magnitude
			int color = (edge_map[i][j]);
			an_image->SetPixel(i,j,color);
		}
	}
			
}


void MakeBinary(int threshold, Image *an_image){
	if (an_image == nullptr) abort();
	
	int x = 0;
	int y = 0;
	int x_max = an_image->num_rows();
	int y_max = an_image->num_columns();
	int done = 0;
	int color;

	// Start the scan.
	while (!done) {
		if(an_image->GetPixel(x,y) < threshold)
		{
			//if pixel is below threshold, set it to 0
			color = 0;
		} else {

			//if above, set to 255
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

void GenerateHoughArray(Image *an_image, int threshold, string text_file){
	
	int i_max = an_image->num_rows();
	int j_max = an_image->num_columns();
	// max_rho = max diagonal line in the images provided
	int max_rho = (int)(sqrt((i_max * i_max) + (j_max * j_max)));
	
	// 0 <= theta < pi and using ints to represent it, use degrees
	// to 0 <= theta < 180
	
	//created array with length max_rho*2 so it can 
	//account for negative rho values
	int accumulator[max_rho*2][180] = {{0}};
	for(int rho = 0;  rho < max_rho; rho++){
		for(int theta = 0; theta < 180; theta++){
			if(accumulator[rho][theta] != 0){
				//double checking that all values 
				//initialized to 0 because I ran into
				//issues with it earlier
				accumulator[rho][theta] = 0;
			}
		}
	}
	int max_val = 0;
	for(int i = 2; i < i_max-2; i++){
		for(int j = 2; j <j_max-2; j++){
			if(an_image->GetPixel(i, j)  > threshold){
				//edge point detected
				for(int theta = 0; theta < 179; theta++){
					//add rho max/2 to rho so - rho values are allowed.  This is compensated for when calculating lines
					int rho = (int)(((j * cos(theta * (M_PI / 180))) + (i * sin(theta * (M_PI / 180))))) + (max_rho / 2);
					//~ cout << "i " << i << " j " << j << " rho " << rho <<endl;
					if(rho > 0 && rho < max_rho*2){
						accumulator[rho][theta]++;
						//~ cout << "i " << i << " j " << j << " rho " << rho << " In accum" <<endl;

						if(accumulator[rho][theta] > max_val){
							max_val = accumulator[rho][theta];
						}
					}
				}
			}
		}
	}
	GenerateHoughImage(accumulator, max_rho, max_val, an_image, text_file);
}

void GenerateHoughImage(int hough_array[][180], int max_rho, int max_val, Image *an_image, string text_file){
	//Takes the accumulator array and generates the imagae of the hough transform space, as well as writes
	// it into a text file
	an_image->AllocateSpaceAndSetSize(max_rho*2, 181);
	an_image->SetNumberGrayLevels(255);
	int i_max = an_image->num_rows();
	int j_max = an_image->num_columns();
	
	//clear the image
	for(int i = 0; i < i_max-1; i++){
		for(int j = 0; j <j_max-1; j++){
			an_image->SetPixel(i,j,0);
		}
	}
	
	//write the file into the given text file
	ofstream file(text_file);
	for(int rho = 0;  rho < max_rho*2; rho++){
		for(int theta = 0; theta < 180; theta++){
			int color = (hough_array[rho][theta] > 255) ? 255 : hough_array[rho][theta];
			an_image->SetPixel(rho, theta, color);
			file << hough_array[rho][theta]<< " ";
        }
        file << endl;
    }
    file.close();	
}

void DrawHoughLines(Image *an_image, string text_file, int threshold){
	
	int i_max = an_image->num_rows();
	int j_max = an_image->num_columns();
	int max_val = 0;
	// max_rho = max diagonal line in the images provided
	int max_rho = (int)(sqrt((i_max * i_max) + (j_max * j_max)));
	// 0 <= theta < pi and using ints to represent it, use degrees
	// to 0 <= theta < 180
	int accumulator[max_rho*2][180] = {{0}};
	ifstream file(text_file);
	for(int rho = 0;  rho < max_rho*2; rho++){
		for(int theta = 0; theta < 180; theta++){
            file >> accumulator[rho][theta];
            if(accumulator[rho][theta] > max_val){
				max_val = accumulator[rho][theta];
			}
        }
    }
    file.close();
    
	
	GenerateLines(accumulator, max_rho, max_val, an_image, threshold);
}



void GenerateLines(int hough_array[][180], int max_rho, int max_val, Image *an_image, int threshold){
	int img_w = an_image->num_columns();
	int img_h = an_image->num_rows();
	int counter = 0;

	
	
	for(int rho = 0;  rho < max_rho*2; rho++){
		//~ cout<<rho<<"/"<<max_rho<<endl;
		for(int theta = 0; theta < 179; theta++){
			//~ cout<<theta<<endl;
			if(hough_array[rho][theta] >= threshold){
				//local maxima
				int max = hough_array[rho][theta];
				

				int x1, y1, x2, y2;  
				x1 = y1 = x2 = y2 = 0; 
				
				//to account for negative rho values 
				int rho_offset = rho - ( max_rho / 2);
				
				if(theta >= 45 && theta <= 135)  
				{  
					//y = (r - x cos(theta)) / sin(theta) 
					//~ cout << rho << endl;
					x1 = 0;  
					y1 = (rho_offset- (x1 * cos(theta* (M_PI / 180)))) / sin(theta* (M_PI / 180));  
					x2 = img_w-1;  
					y2 = (rho_offset - (x2 * cos(theta* (M_PI / 180)))) / sin(theta* (M_PI / 180));
					//~ cout << rho << "  " << y2 << "  " << x2 << "  " << theta << "  " << sin(theta* (M_PI / 180)) << " " << cos(theta * (M_PI / 180)) << endl;
				}
				else  
				{  
					//x = (r - y sin(theta)) / cos(theta);
					y1 = 0;  
					x1 = (rho_offset - (y1 * sin(theta* (M_PI / 180)))) / cos(theta * (M_PI / 180));  
					y2 = img_h-1;  
					x2 = (rho_offset - (y2 * sin(theta* (M_PI / 180)))) / cos(theta* (M_PI / 180)); 
					//~ cout << rho << "  " << y2 << "  " << sin(theta* (M_PI / 180)) << " " << cos(theta* (M_PI / 180)) << endl;
					
				}
				//check if any lines are too long and cut them off
				if(x1 < 0){ x1 = 0;}
				if(y1 < 0){ y1 = 0;}
				if(x2 < 0){ x2 = 0;}
				if(y2 < 0){ y2 = 0;}
				if(x1 >= img_w){ x1 = img_w-1;}
				if(y1 >= img_h){ y1 = img_h-1;}
				if(x2 >= img_w){ x2 = img_w-1;}
				if(y2 >= img_h){ y2 = img_h-1;}
				//~ cout << theta << " " << rho  << " " << x1 << " " << y1 << " " << x2 << " " << y2 << endl;
				//Draw the line from x1,y1 to x2,y2
				DrawLine(y1, x1, y2, x2, 255, an_image);
			}
		}
	}
}

void DrawHoughLinesWithEndpoints(Image *an_image, string text_file, int threshold){
	
	int i_max = an_image->num_rows();
	int j_max = an_image->num_columns();
	int max_val = 0;
	// max_rho = max diagonal line in the images provided
	int max_rho = (int)(sqrt((i_max * i_max) + (j_max * j_max)));
	// 0 <= theta < pi and using ints to represent it, use degrees
	// to 0 <= theta < 180
	int accumulator[max_rho*2][180] = {{0}};
	ifstream file(text_file);
	for(int rho = 0;  rho < max_rho*2; rho++){
		for(int theta = 0; theta < 180; theta++){
            file >> accumulator[rho][theta];
            if(accumulator[rho][theta] > max_val){
				max_val = accumulator[rho][theta];
			}
        }
    }
    file.close();
    
	
	GenerateCutoffLines(accumulator, max_rho, max_val, an_image, threshold);
}

void GenerateCutoffLines(int hough_array[][180], int max_rho, int max_val,Image *an_image, int threshold){
	
	int img_w = an_image->num_columns();
	int img_h = an_image->num_rows();
	int counter = 0;

	
	
	for(int rho = 0;  rho < max_rho*2; rho++){
		//~ cout<<rho<<"/"<<max_rho<<endl;
		for(int theta = 0; theta < 179; theta++){
			//~ cout<<theta<<endl;
			if(hough_array[rho][theta] >= threshold){
				//local maxima
				int max = hough_array[rho][theta];
				

				int x1, y1, x2, y2;  
				x1 = y1 = x2 = y2 = 0; 
				
				//to account for negative rho values 
				int rho_offset = rho - ( max_rho / 2);
				
				if(theta >= 45 && theta <= 135)  
				{  
					//y = (r - x cos(theta)) / sin(theta) 
					//~ cout << rho << endl;
					x1 = 0;  
					y1 = (rho_offset- (x1 * cos(theta* (M_PI / 180)))) / sin(theta* (M_PI / 180));  
					x2 = img_w-1;  
					y2 = (rho_offset - (x2 * cos(theta* (M_PI / 180)))) / sin(theta* (M_PI / 180));
					//~ cout << rho << "  " << y2 << "  " << x2 << "  " << theta << "  " << sin(theta* (M_PI / 180)) << " " << cos(theta * (M_PI / 180)) << endl;
				}
				else  
				{  
					//x = (r - y sin(theta)) / cos(theta);
					y1 = 0;  
					x1 = (rho_offset - (y1 * sin(theta* (M_PI / 180)))) / cos(theta * (M_PI / 180));  
					y2 = img_h-1;  
					x2 = (rho_offset - (y2 * sin(theta* (M_PI / 180)))) / cos(theta* (M_PI / 180)); 
					//~ cout << rho << "  " << y2 << "  " << sin(theta* (M_PI / 180)) << " " << cos(theta* (M_PI / 180)) << endl;
					
				}
				//check if any lines are too long and cut them off
				if(x1 < 0){ x1 = 0;}
				if(y1 < 0){ y1 = 0;}
				if(x2 < 0){ x2 = 0;}
				if(y2 < 0){ y2 = 0;}
				if(x1 >= img_w){ x1 = img_w-1;}
				if(y1 >= img_h){ y1 = img_h-1;}
				if(x2 >= img_w){ x2 = img_w-1;}
				if(y2 >= img_h){ y2 = img_h-1;}
				//~ cout << theta << " " << rho  << " " << x1 << " " << y1 << " " << x2 << " " << y2 << endl;
				//Draw the line from x1,y1 to x2,y2
				DrawLineWithEndpoints(y1, x1, y2, x2, 255, an_image);
			}
		}
	}
}
	

	
	
	
}

// ~ // Implements the Bresenham's incremental midpoint algorithm;
// ~ // (adapted from J.D.Foley, A. van Dam, S.K.Feiner, J.F.Hughes
// ~ // "Computer Graphics. Principles and practice", 
// ~ // 2nd ed., 1990, section 3.2.2);  
// ~ void DrawLineWithEndpoints(int x0, int y0, int x1, int y1, int color,
// 	      ~ Image *an_image) {  
//   ~ if (an_image == nullptr) abort();

// ~ #ifdef SWAP
// ~ #undef SWAP
// ~ #endif
// ~ #define SWAP(a,b) {a^=b; b^=a; a^=b;}



// ~	//Plan is to make a Binary Image of the given image, then draw the 
// ~	//lines only on points that also have edges in the Binary image
	
// ~	//Make Binary Image with low threshold to find all edges
// ~    Image bin_img;
	// int i_max = an_image->num_rows();
	// int j_max = an_image->num_columns();
	// an_image->AllocateSpaceAndSetSize(i_max, j_max);
	// an_image->SetNumberGrayLevels(255);
// //Copy the input image into a copy called bin_img
// //that we will use to create a binary edge image
//	for (int i = 0; i < i_max; ++i)
//     for (int j = 0; j < j_max; ++j)
//       bin_img.SetPixel(i, j, an_image->GetPixel(i, j));
	// GenerateEdgeImage(bin_img);
	// MakeBinary(20, bin_img);


// ~	
//   ~ const int DIR_X = 0;
//   ~ const int DIR_Y = 1;
  
//   ~ // Increments: East, North-East, South, South-East, North.
//   ~ int incrE,
//     ~ incrNE,
//     ~ incrS,
//     ~ incrSE,
//     ~ incrN;     
//   ~ int d;         /* the D */
//   ~ int x,y;       /* running coordinates */
//   ~ int mpCase;    /* midpoint algorithm's case */
//   ~ int done;      /* set to 1 when done */
  
//   ~ int xmin = x0;
//   ~ int xmax = x1;
//   ~ int ymin = y0;
//   ~ int ymax = y1;
  
//   ~ int dx = xmax - xmin;
//   ~ int dy = ymax - ymin;
//   ~ int dir;

//   ~ if (dx * dx > dy * dy) {  // Horizontal scan.
//     ~ dir=DIR_X;
//     ~ if (xmax < xmin) {
//       ~ SWAP(xmin, xmax);
//       ~ SWAP(ymin , ymax);
//     ~ } 
//     ~ dx = xmax - xmin;
//     ~ dy = ymax - ymin;

//     ~ if (dy >= 0) {
//       ~ mpCase = 1;
//       ~ d = 2 * dy - dx;      
//     ~ } else {
//       ~ mpCase = 2;
//       ~ d = 2 * dy + dx;      
//     ~ }

//     ~ incrNE = 2 * (dy - dx);
//     ~ incrE = 2 * dy;
//     ~ incrSE = 2 * (dy + dx);
//   ~ } else {// vertical scan.
//     ~ dir = DIR_Y;
//     ~ if (ymax < ymin) {
//       ~ SWAP(xmin, xmax);
//       ~ SWAP(ymin, ymax);
//     ~ }
//     ~ dx = xmax - xmin;
//     ~ dy = ymax-ymin;    

//     ~ if (dx >=0 ) {
//       ~ mpCase = 1;
//       ~ d = 2 * dx - dy;      
//     ~ } else {
//       ~ mpCase = 2;
//       ~ d = 2 * dx + dy;      
//     ~ }

//     ~ incrNE = 2 * (dx - dy);
//     ~ incrE = 2 * dx;
//     ~ incrSE = 2 * (dx + dy);
//   ~ }
  
//   ~ /// Start the scan.
//   ~ x = xmin;
//   ~ y = ymin;
//   ~ done = 0;

//   ~ while (!done) {

// //If pixel has value 255 in the binary edge image,
// //we can set it to 255 on the line else, we skip it
// //and keep drawing the line
// if(bin_img.GetPixel(x,y) == 255){
	//     ~ an_image->SetPixel(x,y,color);
//}


  
//     ~ // Move to the next point.
//     ~ switch(dir) {
//     ~ case DIR_X: 
//       ~ if (x < xmax) {
// 	      ~ switch(mpCase) {
// 	      ~ case 1:
// 		~ if (d <= 0) {
// 		  ~ d += incrE;  
// 		  ~ x++;
// 		~ } else {
// 		  ~ d += incrNE; 
// 		  ~ x++; 
// 		  ~ y++;
// 		~ }
// 		~ break;
  
//             ~ case 2:
//               ~ if (d <= 0) {
//                 ~ d += incrSE; 
// 		~ x++; 
// 		~ y--;
//               ~ } else {
//                 ~ d += incrE;  
// 		~ x++;
//               ~ }
// 	      ~ break;
// 	      ~ } 
//       ~ } else {
// 	~ done=1;
//       ~ }     
//       ~ break;

//     ~ case DIR_Y: 
//         ~ if (y < ymax) {
//           ~ switch(mpCase) {
// 	  ~ case 1:
// 	    ~ if (d <= 0) {
// 	      ~ d += incrE;  
// 	      ~ y++;
// 	    ~ } else {
// 	      ~ d += incrNE; 
// 	      ~ y++; 
// 	      ~ x++;
// 	    ~ }
//             ~ break;
  
// 	  ~ case 2:
// 	    ~ if (d <= 0) {
//                 ~ d += incrSE; 
// 		~ y++; 
// 		~ x--;
//               ~ } else {
//                 ~ d += incrE;  
// 		~ y++;
// 	    ~ }
//             ~ break;
// 	  ~ } // mpCase
//         ~ } // y < ymin 
//         ~ else {
// 	  ~ done=1;
// 	~ }
// 	~ break;    
//     ~ }
//   ~ }
// ~ }


}  // namespace ComputerVisionProjects
