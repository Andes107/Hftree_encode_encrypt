#ifndef UTILS_H
#define UTILS_H

#include <map>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <iomanip>
#include "hftree.h"
using namespace std;

class Utils
{
public:
	Utils();
	~Utils();
	//Those comments without a space after slashes are default
	bool loadDictionary(const string& text_dic); //load dictionary to content and make frequency_table
	void buildTree();	// build huffman tree
	void setEncodedTable();	// set encoded table
	void saveBinDictionary(const string& bin_file); // save bin_code into a binary file
	void decode(const string& bin_file); // decode a binary file to a plaintext file
private:
	HuffmanTree tree; //Huffmantree
	map<char, int> frequency_table; //Map original dictionary to int
	map<char, string> encoded_table; //Map original dictionary to bits, i.e., 11001
	string content; //content of original text
	//string bit_string;
};

#endif // UTILS_H
