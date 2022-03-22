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
  
	if (argc!=5) {
		printf("Usage: %s input_image text_file threshold_value output_image\n", argv[0]);
		return 0;
	}
	const string input_file(argv[1]);
	const string text_file(argv[2]);
	int t = atoi(argv[3]);
	const string output_file(argv[4]);
	
	Image an_image;
	if (!ReadImage(input_file, &an_image)) {
		cout <<"Can't open file " << input_file << endl;
		return 0;
	}
	
	DrawHoughLines(&an_image, text_file, t);
		
	if (!WriteImage(output_file, an_image)){
		cout << "Can't write to file " << output_file << endl;
		return 0;
	}
}
