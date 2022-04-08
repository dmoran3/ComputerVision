// Class for representing a 2D gray-scale image,
// with support for reading/writing pgm images.
// To be used in Computer Vision class.

#include "image.h"
#include <math.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <fstream>
#include <string>


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
	/// Start the scan.
	int x = 0;
	int y = 0;
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

		if (x < an_image->num_columns()) {
			x++;
		}
		else{ //at end of line
			if(y = an_image->num_rows()){ //at end of image
				done = 1;
			}
			x = 0;
			y++;
		}
	}
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

//----------------------------------------------------------------------
// HW 4 - Dan Moran

/*
 * s1: Find the location of the sphere and its radius.
 * Threshold object using given t value.
 * Find the centroid.
 * Find the radius.
 * Output values to a text file. -> ExportObjectData()
*/
void ExportObjectData(int threshold, string text_file, Image *an_image){
	if (an_image == nullptr) abort();

	//Initialize right and bottom to 0
	//Initialize area, x_sum and y_sum to 0
	int x = 0;
	int y = 0;
	int right_val = 0;
	int bottom_val = 0;
	int area = 0;
	int x_sum = 0;
	int y_sum = 0;
	//Initialize left and top to max values
	int left_val= an_image->num_rows();
	int x_max = an_image->num_rows();
	int top_val = an_image->num_columns();
	int y_max = an_image->num_columns();
	int done = 0;
	int color = 0;;

	// Start the scan.
	while (!done) {
		if(an_image->GetPixel(x,y) < threshold)
		{
			//if pixel is below threshold, set it to 0
			color = 0;
		} else {

			//if above, set to 255
			color = 255;
			//check if the pixel if uppermost, leftmost, rightmost, or 
			//lowermost in the object to compute diameter
			if(x<left_val){left_val = x;}
			if(x>right_val){right_val = x;}
			if(y<top_val){top_val = y;}
			if(y>bottom_val){bottom_val = y;}
			
			//Increment area and summation counters
			area++;
			x_sum += x;
			y_sum += y;
			
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
	}//Scan ended
	
	//Calculate Centroid
	int x_center = x_sum / area;
	int y_center = y_sum / area;
	
	//Find radius by getting D as average of differences between right
	// and left, and top and bottom
	int radius = ((right_val - left_val) + (bottom_val - top_val)) / 4;
	
	ofstream file(text_file);
    file << x_center  << " " << y_center << " " << radius;
    file.close();
}

/*
 * s2: Compute the directions and intensities of light sources affecting 
 * and object.
 * Compute Normal Vector to surface at a given point.
 * This formula should give you the resulting normal vector in a 3-D
 * coordinate system, originating at the object’s center.
*/
void FindLightSource(string in_file, string out_file, Image *an_image){
	if (an_image == nullptr) abort();

	//Initialize x and y iterators, x and y center values, x, y, and z
	//point values, and the radius
	int x = 0;
	int y = 0;
	int x_c = 0;
	int y_c = 0;
	int x_p = 0;
	int y_p = 0;
	int z_p = 0;
	int radius = 0;
	
	//Read in values from params file
	ifstream file(in_file);
    file >> x_c >> y_c >> radius;
    file.close();
    
	// Find brightest pixel in image
	int x_max = an_image->num_rows();
	int y_max = an_image->num_columns();
	int done = 0;
	// Start the scan.
	int max_brightness = 0;
	while (!done) {
		if(an_image->GetPixel(x,y) > max_brightness){
			//new max
			x_p = x;
			y_p = y;
			max_brightness = an_image->GetPixel(x,y);			
		}

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
	}//Scan ended
		
	
	//FORMULA
	//R^2 - (x_p - x_c)^2 - (y_p - y_c)^2 = z_p^2
	//z_p = sqrt(R^2 - (x_p - x_c)^2 - (y_p - y_c)^2)
	double_t term1 = pow(radius,2);
	double_t term2 = pow((x_p - x_c),2);
	double_t term3 =pow((y_p - y_c),2);
	z_p = sqrt((term1 - term2 - term3));

	//Initialize vector values
	int x_v = 0;
	int y_v = 0;
	int z_v = 0;
	double scale_val = max_brightness / 255.0;
	x_v = (x_p-x_c);// * scale_val;
	y_v = (y_p-y_c);// * scale_val;
	z_v = z_p;// * scale_val;

	ofstream ofile(out_file, ios_base::app);
    ofile << x_v << " " << y_v << " "  << z_v << endl;;
    ofile.close();
	
}

/*
 * s3: Compute surface normals of the object
 * Given 3 images of an object and the direction of the light source
 * Compute normals at pixels that are visible in all three images
 * Compute normals at pixels every N steps apart(step is an argument)
 * From each pixel, draw a line that is the projection of the normal
 * onto an image plane
 * 
 * S = [s1, s2, s3]
 * N = S^-1[I1 I2 I3]
 * 
*/
void ComputeSurfaceNormals(string in_file,Image* image_1,Image* image_2,
		Image* image_3, int threshold, int step){
	if (image_1 == nullptr) abort();
	if (image_2 == nullptr) abort();
	if (image_3 == nullptr) abort();
	
	//Initialize light source direction vector
	int s_x = 0;
	int s_y = 0;
	int s_z = 0;
	float_t S[3][3];
	float_t S_inv[3][3];
	
	int x = 0;
	int y = 0;
	int x_max = image_1->num_rows();
	int y_max = image_1->num_columns();
	int done = 0;
	
	ifstream infile(in_file);
	for(int i = 0; i < 3; i++){
		for(int j = 0; j < 3; j++){
			infile >> S[i][j];
		}
	}
	
	float_t det = 0.0;
	//determinant of S
	for(int i = 0; i < 3; i++)
		det = det + (S[0][i] * (S[1][(i+1)%3] * S[2][(i+2)%3] - 
		S[1][(i+2)%3] * S[2][(i+1)%3]));

	//reverse the matrix
	for(int i = 0; i < 3; i++){
		for(int j = 0; j < 3; j++)
			S_inv[i][j] = ((S[(j+1)%3][(i+1)%3] * S[(j+2)%3][(i+2)%3]) -
			 (S[(j+1)%3][(i+2)%3] * S[(j+2)%3][(i+1)%3])) / det;
	}
	

	
	// N = S_inv * [I1, I2, I3] at each point x,y
	// N will be represented by N_x, N_y, and N_z
	float_t N_x = 0.0;
	float_t N_y = 0.0;
	float_t N_z = 0.0;
	int I1 = 0;
	int I2 = 0;
	int I3 = 0;
	
	// Start the scan.
	while (!done) {
		I1 = image_1->GetPixel(x,y);
		I2 = image_2->GetPixel(x,y);
		I3 = image_3->GetPixel(x,y);
		if( I1 > threshold && I2 > threshold && I3 > threshold){
			//If x,y is above the threshold for all images, compute N
			N_x = (I1*S_inv[0][0])+(I2*S_inv[0][1])+(I3*S_inv[0][2]);
			N_y = (I1*S_inv[1][0])+(I2*S_inv[1][1])+(I3*S_inv[1][2]);
			N_z = (I1*S_inv[2][0])+(I2*S_inv[2][1])+(I3*S_inv[2][2]);
			
			N_x = N_x/N_z;
			N_y = N_y/N_z;
			N_z = N_z/N_z;
			
			float_t N_mag = sqrt((N_x * N_x) + (N_y * N_y) +1);
			
			//orientation of surface normal is now [n_x/N_mag, N_y/N_mag,
			//1/N_mag]
			if(x > 2 && x < x_max - 2 && y > 2 && y < y_max - 2){
				for(int i = -1; i <= 1; i++){
					for(int j = -1; j <= 1; j++){
						image_1->SetPixel(x+i,y+j,255);
					}
				}
				image_1->SetPixel(x,y,0);
			}
			DrawLine(x, y, (N_x * 10)+x, (N_y * 10)+y, 255, image_1);
			
			
			
		}

		if (x < x_max-step-1) {
			x+=step;
		}
		else{ //at end of line
			if(y > y_max-step-1){ //at end of image
				done = 1;
			}
			x = 0;
			y+=step;
		}
	}//Scan ended
	
}

/*
 * s4: Compute surface albedo
 * compute albedo for all pixels above threshold in all images
 * scale them up or down to fit range 0 to 255
*/
void ComputeAlbedoMap(string in_file,Image* image_1,Image* image_2,
		Image* image_3, int threshold){
	if (image_1 == nullptr) abort();
	if (image_2 == nullptr) abort();
	if (image_3 == nullptr) abort();
	
	//Initialize light source direction vector
	int s_x = 0;
	int s_y = 0;
	int s_z = 0;
	float_t S[3][3];
	float_t S_inv[3][3];

	
	int x = 0;
	int y = 0;
	int x_max = image_1->num_rows();
	int y_max = image_1->num_columns();
	float_t raw_albedos[x_max][y_max];
	int done = 0;
	
	ifstream infile(in_file);
	for(int i = 0; i < 3; i++){
		for(int j = 0; j < 3; j++){
			infile >> S[i][j];
		}
	}
	
	float_t det = 0.0;
	//determinant of S
	for(int i = 0; i < 3; i++)
		det = det + (S[0][i] * (S[1][(i+1)%3] * S[2][(i+2)%3] - 
		S[1][(i+2)%3] * S[2][(i+1)%3]));
	//reverse the matrix
	for(int i = 0; i < 3; i++){
		for(int j = 0; j < 3; j++)
			S_inv[i][j] = ((S[(j+1)%3][(i+1)%3] * S[(j+2)%3][(i+2)%3]) -
			 (S[(j+1)%3][(i+2)%3] * S[(j+2)%3][(i+1)%3])) / det;
	}
	
	
	
	// N = S_inv * [I1, I2, I3] at each point x,y
	// N will be represented by N_x, N_y, and N_z
	float_t N_x = 0.0;
	float_t N_y = 0.0;
	float_t N_z = 0.0;
	int I1 = 0;
	int I2 = 0;
	int I3 = 0;
	
	float_t max_albedo = 0;
	// Start the scan.
	while (!done) {
		I1 = image_1->GetPixel(x,y);
		I2 = image_2->GetPixel(x,y);
		I3 = image_3->GetPixel(x,y);
		if( I1 > threshold && I2 > threshold && I3 > threshold){
			//If x,y is above the threshold for all images, compute N
			N_x = (I1*S_inv[0][0])+(I2*S_inv[0][1])+(I3*S_inv[0][2]);
			N_y = (I1*S_inv[1][0])+(I2*S_inv[1][1])+(I3*S_inv[1][2]);
			N_z = (I1*S_inv[2][0])+(I2*S_inv[2][1])+(I3*S_inv[2][2]);

			N_x = N_x/N_z;
			N_y = N_y/N_z;
			N_z = N_z/N_z;

			float_t N_mag = sqrt((N_x * N_x) + (N_y * N_y) +1);
			raw_albedos[x][y] = N_mag;
			
			max_albedo = max(N_mag, max_albedo);
			
		}


		if (x < x_max-1) {
			x++;
		}
		else{ //at end of line
			if(y > y_max-2){ //at end of image
				done = 1;
			}
			x = 0;
			y++;
		}
	}//Scan ended

	float_t scale_val = 255/max_albedo;
	done = 0;
	x = 0;
	y= 0;
	// Start the coloring.
	while (!done) {
		I1 = image_1->GetPixel(x,y);
		I2 = image_2->GetPixel(x,y);
		I3 = image_3->GetPixel(x,y);
		if( I1 > threshold && I2 > threshold && I3 > threshold){
			//If x,y is above the threshold for all images, compute N
			image_1->SetPixel(x,y,(int)(raw_albedos[x][y] * scale_val));			
		}
		else{
			image_1->SetPixel(x,y,0);
		}
		if (x < x_max-1) {
			x+=1;
		}
		else{ //at end of line
			if(y > y_max-2){ //at end of image
				done = 1;
			}
			x = 0;
			y+=1;
		}
	}//Coloring ended 
	
}

//----------------------------------------------------------------------

}  // namespace ComputerVisionProjects







