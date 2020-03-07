#include "utils.h"

Utils::Utils() {}

Utils::~Utils()
{
	tree.release();
}

bool Utils::loadDictionary(const string& text_dic)
{
	// load the file with name text_dic, and save all characters in this->content
	ifstream load_file(text_dic);
	if (!load_file.is_open()) {
		cout << "Failed to load the article. Exit." << endl;
		return false;
	}

	ostringstream buf;
	char ch;
	while (buf && load_file.get(ch))
		buf.put(ch);
	this->content = buf.str();
	// move out the end useless '\r' of a file
	this->content.pop_back();

	load_file.close();
	// scan the string this->content, calculate the frequency table
	// insert your code here ...
	if (!content.size())
		return false;

	string copy = content;
	while (copy.size()) {
		char first = copy[0];
		if (frequency_table.find(first) == frequency_table.end()) {
			frequency_table.insert({ first, 0 });
		}
		map<char, int>::iterator map_itr = frequency_table.find(first);
		string::iterator cur_str_itr = copy.begin();
		while (cur_str_itr != copy.end()) {
			if ((*cur_str_itr) == first) {
				map_itr->second++;
				copy.erase(cur_str_itr);
				continue;
			}
			cur_str_itr++;
		}
	}
	return true;
}

void Utils::buildTree()
{
	tree.loadMap(frequency_table);
}

void Utils::setEncodedTable()
{
	tree.encode(encoded_table);
}

void Utils::saveBinDictionary(const string& bin_file)
{
	// load key file
	ifstream ifile("xor_key.bin", ios::binary);
	if (!ifile.is_open()) {
		cout << "cannot load key file, exit." << endl;
		return;
	}
	ofstream ofile(bin_file, ios::binary);

	// assuming that the length of bin_code is multiple of 8
	// so just for every 8 continuous {0,1} characters, intepret it as a binary byte number
	// for last byte you write, its valid length may be less than 8, fill the invalid bits with 0
	// eg., last byte contains only 3 valid bits, 110. You should fill it as 1100 0000, and then
	// you need to write integer 3 (bin form: 0000 0011) as one byte at the beginning of your binary file.
	// after saving data into .bin file, you should print out its hex form in command line
	// insert your code here ...
	string bit_string = "";

	for (int i = 0; i < content.size(); ++i) { //Combining binary code
		map<char, string>::iterator en_itr = encoded_table.begin(); //Find Huffman code string as per the char
		while (en_itr != encoded_table.end()) {
			if (en_itr->first == content[i])
				break;
			++en_itr;
		}
		if (en_itr != encoded_table.end()) //If found, then put it in the back of string, 
			bit_string += en_itr->second;
	}

	int bit_size = bit_string.size(); //Get initial binary code size
	//cout << "initial bit_string bits: " << bit_size << " Initial bit_string bytes: " << bit_size / 8 << endl;

	if (bit_size % 8) { //cut-off number = valid bit in last byte = bit_size % 8, add in 8 - cut_off amount of 0's
		//Fill in the right part of string
		for (int i = 0, num_of_0 = 8 - bit_size % 8; i < num_of_0; ++i) 
			bit_string += '0';

		//Fill in the left part of string
		int cut_off = bit_size % 8; //cut_off number = initial binary code size % 8, dec to bin now
		//cout << "cut_off: " << cut_off << endl << "cut_off(bin, inverse order): ";
		for (int i = 0; i < 8; ++i, cut_off /= 2) {
			//cout << cut_off % 2;
			bit_string = (cut_off % 2 ? '1' + bit_string : '0' + bit_string);
		}
	}
	bit_size = bit_string.size(); //update bit_size
	//if (bit_string.size() % 8 == 0)
		//cout << "end bit_string bits: " << bit_size << " end bit_string bytes: " << bit_size / 8 << endl;

	//Encode in multiple of 8
	//Now encrypt
	char key;
	while (ifile.get(key) && bit_string.size()) {
		//load a byte into bin_code
		char bin_code = 0;
		for (int i = 0; i < 8; ++i) {
			bin_code = bin_code << 1;
			if (bit_string[0] == '1')
				bin_code = bin_code | 1;
			bit_string = bit_string.substr(1, (bit_string.size() - 1));
		}
		//XOR bin_code and key
		char XOR = bin_code ^ key;
		ofile << XOR;
	}
	ofile.close();

	//print encode version in hex form
	ifstream ifile2(bin_file, ios::binary);
	char get;
	while (ifile2.get(get)) {
		unsigned char unget = get;
		int x_dec = unget;
		if (x_dec < 16 && x_dec >= 0)
			cout << '0' << hex << (int)unget;
		else
			cout << hex << (int)unget;
	}
	cout << endl << endl;
	ifile2.close();
}

void Utils::decode(const string& bin_file)
{
	ifstream ifile(bin_file, ios::binary);
	if (!ifile.is_open()) {
		cout << "cannot open .bin file, stop decoding." << endl;
		return;
	}

	ifstream key_file("xor_key.bin", ios::binary);
	if (!key_file.is_open()) {
		cout << "cannot load key file, exit." << endl;
		return;
	}

	// the string used to search on huffman tree to decode as plaintext
	string bit_str = "";

	// bin_file: stores a binary huffman code with possible extra bits at the end
	// key_file: decryption XOR key
	// search in the encoded table
	// insert your code here ...

	char bin, key; //bin for binary file code, key for xor key binary file
	unsigned char decrypt;
	int cut_off_sum = 0;

	for (int loop = 0; ifile.get(bin) && key_file.get(key); ++loop) {
		decrypt = bin ^ key;
		//cout << hex << int(decrypt);
		int decrypt_dec = decrypt;
		if (loop == 0 && decrypt_dec < 8 && decrypt_dec >= 0) {
			cut_off_sum = decrypt;
			continue;
		}
		else {
			string temp = "";
			for (int i = 0; i < 8; ++i, decrypt_dec /= 2) //Must loop 8 times because must fill in 8 bits
				temp = (decrypt_dec % 2 ? '1' + temp : '0' + temp);
			bit_str += temp; //FIFO
		}
	}
	if (cut_off_sum) //if no, then you don't have to trim the right part!
		for (int i = 0, useless_0 = 8 - cut_off_sum; i < useless_0; bit_str.pop_back(), ++i);

	ifile.close();
	key_file.close();

	// using huffman tree you've built before to decode the bit string
	string plaintext = tree.decode(bit_str);
	cout << plaintext << endl << endl;
}
