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
  
	if (argc!=7) {
		printf("Usage: %s param_infile img1 img2 img3 threshold param_outfile\n", argv[0]);
		return 0;
	}
	const string input_file(argv[1]);
	const string image_file_1(argv[2]);
	const string image_file_2(argv[3]);
	const string image_file_3(argv[4]);
	int threshold = atoi((argv[5]));
	const string output_file(argv[6]);

	Image image_1;
	if (!ReadImage(image_file_1, &image_1)) {
		cout <<"Can't open file " << image_file_1 << endl;
		return 0;
	}
	Image image_2;
	if (!ReadImage(image_file_2, &image_2)) {
		cout <<"Can't open file " << image_file_2 << endl;
		return 0;
	}
	Image image_3;
	if (!ReadImage(image_file_3, &image_3)) {
		cout <<"Can't open file " << image_file_3 << endl;
		return 0;
	}
	
	ComputeAlbedoMap(input_file, &image_1, &image_2, &image_3, 
		threshold);
		
	if (!WriteImage(output_file, image_1)){
		cout << "Can't write to file " << output_file << endl;
		return 0;
	}

}
