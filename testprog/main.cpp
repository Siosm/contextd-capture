#include <iostream>
#include <fstream>
#include <string>


int openfile(std::string filename)
{
	std::string tmpLine;
	
	std::cout << "Trying to open " << filename << std::endl;
	
	std::ifstream file(filename.c_str(), std::ios::in);
	
	if (!file) {
		std::cerr << "File " << filename << " can't be open (or doesn't exist)" << std::endl;
		return -1;
	}else{
		while(std::getline(file, tmpLine)){
			std::cout << tmpLine << std::endl;
		}
		file.close();
		std::cout << "Closing file " << filename << std::endl;
	}

	return 0;
}


int main(int argc, char ** argv)
{
	for(int i = 0; i < 2 ; ++i){
		openfile("/home/user/impots/test");
		openfile("/home/user/ecommerce/test");
	}

	return 0;
}