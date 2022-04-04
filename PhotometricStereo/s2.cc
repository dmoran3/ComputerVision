//DAN MORAN
//COMPUTER VISION

#include "image.h"
#include <iostream>
#include <cassert>
#include <cstdlib>
#include <fstream>

using namespace std;
using namespace ComputerVisionProjects;

int main(int argc, char **argv){
  
	if (argc!=6) {
		printf("Usage: %s param_infile img1 img2 img3 param_outfile\n", argv[0]);
		return 0;
	}
	const string input_file(argv[1]);
	const string image_file_1(argv[2]);
	const string image_file_2(argv[3]);
	const string image_file_3(argv[4]);
	const string output_file(argv[5]);

	Image an_image;
	if (!ReadImage(image_file_1, &an_image)) {
		cout <<"Can't open file " << image_file_1 << endl;
		return 0;
	}
	
	FindLightSource(input_file, output_file, &an_image);

	if (!ReadImage(image_file_2, &an_image)) {
		cout <<"Can't open file " << image_file_2 << endl;
		return 0;
	}
	FindLightSource(input_file, output_file, &an_image);

	if (!ReadImage(image_file_3, &an_image)) {
		cout <<"Can't open file " << image_file_3 << endl;
		return 0;
	}
	FindLightSource(input_file, output_file, &an_image);


}
