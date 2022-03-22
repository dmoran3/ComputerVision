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
		printf("Usage: %s file1 threshold file2\n", argv[0]);
		return 0;
	}
	const string input_file(argv[1]);
	int t = atoi(argv[2]);
	const string output_file(argv[3]);

	
	//~ int threshold = (int)(threshold_input);
	Image an_image;
	if (!ReadImage(input_file, &an_image)) {
		cout <<"Can't open file " << input_file << endl;
		return 0;
	}
	
	MakeBinary(t, &an_image);
	
	if (!WriteImage(output_file, an_image)){
		cout << "Can't write to file " << output_file << endl;
		return 0;
	}
}
