#include "image.h"
#include <iostream>
#include <cassert>
#include <cstdlib>
#include <fstream>

using namespace std;
using namespace ComputerVisionProjects;

int main(int argc, char **argv){
  
	if (argc!=3) {
		printf("Usage: %s file1 file2\n", argv[0]);
		return 0;
	}
	const string input_file(argv[1]);
	const string output_file(argv[2]);

	Image an_image;
	if (!ReadImage(input_file, &an_image)) {
		cout <<"Can't open file " << input_file << endl;
		return 0;
	}
	
	MakeBinary(125, &an_image);
	LabelBinaryImage(&an_image);
	
	if (!WriteImage(output_file, an_image)){
		cout << "Can't write to file " << output_file << endl;
		return 0;
	}
}
