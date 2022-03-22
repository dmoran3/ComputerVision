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
  
	if (argc!=4) {
		printf("Usage: %s file1 file2 voting-array\n", argv[0]);
		return 0;
	}
	const string input_file(argv[1]);
	const string output_file(argv[2]);
	const string text_file(argv[3]);
	
	Image an_image;
	
	if (!ReadImage(input_file, &an_image)) {
		cout <<"Can't open file " << input_file << endl;
		return 0;
	}
	
	GenerateHoughArray(&an_image, 50, text_file);
		
	if (!WriteImage(output_file, an_image)){
		cout << "Can't write to file " << output_file << endl;
		return 0;
	}
}
