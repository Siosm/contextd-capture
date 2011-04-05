#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cstdlib>
#include <map>
#include <wait.h>


int main(int argc, char ** argv)
{
	std::ifstream file("/home/user/", std::ios::in);
	Graph graph;
	std::string tmpLine;
	std::string currentNode;
	long size, decoup, lu=0;
	if(inputFile.find_first_of(".pol", inputFile.size() - 4) < 0) {
		std::cerr << "Attention : ce fichier n'est peut-être pas dans le format correct (.pol)" << std::endl;
	}

	if (!file) {
		std::cerr << "Le fichier " << inputFile << " ne peut être ouvert ou n'existe pas" << std::endl;
		throw(-1);
	}
	
	size = GetFileSize(file);
	std::cout << "Import NoGraph from POL" << std::endl << "[";
	decoup = size / 100;
	while(std::getline(file, tmpLine)) {
		lu += tmpLine.size();
		if(lu > decoup) {
			lu = 0;
			std::cout << "#";
			std::cout.flush();
		}
		if(tmpLine.size() != 0) {
			if(tmpLine.compare(0, 1, "\t") != 0) {
				currentNode = tmpLine;
				graph.addNode(tmpLine);
			} else if(tmpLine.compare(0, 2, "\t\t") != 0) {
				tmpLine.replace(0, 1, "");
				graph.addNode(tmpLine);
				graph.addEdgeNo(tmpLine, currentNode);
			}
		}
	}
	std::cout << "]" << std::endl;
	std::cout << std::endl;

	file.close();
	
	return 0;
}


Graph importNoGraphFromPol (std::string inputFile) throw(int)
{
	std::ifstream file(inputFile.c_str(), std::ios::in);
	Graph graph;
	std::string tmpLine;
	std::string currentNode;
	long size, decoup, lu=0;
	if(inputFile.find_first_of(".pol", inputFile.size() - 4) < 0) {
		std::cerr << "Attention : ce fichier n'est peut-être pas dans le format correct (.pol)" << std::endl;
	}

	if (!file) {
		std::cerr << "Le fichier " << inputFile << " ne peut être ouvert ou n'existe pas" << std::endl;
		throw(-1);
	}
	
	size = GetFileSize(file);
	std::cout << "Import NoGraph from POL" << std::endl << "[";
	decoup = size / 100;
	while(std::getline(file, tmpLine)) {
		lu += tmpLine.size();
		if(lu > decoup) {
			lu = 0;
			std::cout << "#";
			std::cout.flush();
		}
		if(tmpLine.size() != 0) {
			if(tmpLine.compare(0, 1, "\t") != 0) {
				currentNode = tmpLine;
				graph.addNode(tmpLine);
			} else if(tmpLine.compare(0, 2, "\t\t") != 0) {
				tmpLine.replace(0, 1, "");
				graph.addNode(tmpLine);
				graph.addEdgeNo(tmpLine, currentNode);
			}
		}
	}
	std::cout << "]" << std::endl;
	std::cout << std::endl;

	file.close();

	return graph;
}


Graph importGraphFromPol(std::string inputFile) throw(int)
{
	std::ifstream file(inputFile.c_str(), std::ios::in);
	Graph graph;
	std::string tmpLine;
	std::string currentNode;
	long size, decoup, lu = 0;
	

	if(inputFile.find_first_of(".pol", inputFile.size() - 4) < 0) {
		std::cerr << "Attention : ce fichier n'est peut-être pas dans le format correct (.pol)" << std::endl;
	}

	if (!file) {
		std::cerr << "Le fichier " << inputFile << " ne peut être ouvert ou n'existe pas" << std::endl;
	}

	size = GetFileSize(file);
	std::cout << "Import Graph from POL" << std::endl << "[";
	decoup = size / 100;
	while(std::getline(file, tmpLine)) {
		lu += tmpLine.size();
		if(lu > decoup) {
			lu = 0;
			std::cout << "#";
			std::cout.flush();
		}
		if(tmpLine.size() != 0) {
			if(tmpLine.compare(0, 1, "\t") != 0) {
				currentNode = tmpLine;
				graph.addNode(tmpLine);
			} else if(tmpLine.compare(0, 2, "\t\t") != 0) {
				tmpLine.replace(0, 1, "");
				graph.addNode(tmpLine);
				graph.addEdge(currentNode, tmpLine);
			}
		}
	}
	std::cout << "]" << std::endl;

	std::cout << std::endl;

	file.close();

	return graph;
}


void exportGraphToPGM(Graph graph, std::string outputFile) throw(int)
{
	std::ofstream file(outputFile.c_str(), std::ios::out | std::ios::trunc);
	unsigned int size = graph.size();
	unsigned int borderSize = 10;
	unsigned int nbNiveauGris = 1; // 1 blanc, 0 noir
	std::vector< int > tmpRow;

	if(!file) {
		std::cerr << "Le fichier " << outputFile << " ne peut être ouvert ou n'existe pas" << std::endl;
		throw(-1);
	}

	file << "P2" << " " << (size + borderSize * 2) << " " << (size + borderSize * 2) << " " << nbNiveauGris << std::endl;

	Matrix matrix = graph.getMatrix();

	if(matrix.getSize() != size) {
		std::cerr << "Le graph n'est pas de la bonne taille" << std::endl;
	}
	
	for(unsigned int i = 0; i < borderSize; ++i) {
		for(unsigned int j = 0; j < size + borderSize * 2; ++j) {
			file << 0 << " ";
		}
		file << std::endl;
	}

	for(unsigned int i = 0; i < size; ++i) {
		for(unsigned int k = 0; k < borderSize; ++k) {
			file << 0 << " ";
		}
		tmpRow = matrix.getRow(i);
		for(unsigned int j = 0; j < size; ++j) {
			file << (1 - tmpRow[j]) << " ";
		}
		for(unsigned int k = 0; k < borderSize; ++k) {
			file << 0 << " ";
		}
	}

	for(unsigned int i = 0; i < borderSize; ++i) {
		for(unsigned int j = 0; j < size + borderSize * 2; ++j) {
			file << 0 << " ";
		}
		file << std::endl;
	}

	file.close();
}