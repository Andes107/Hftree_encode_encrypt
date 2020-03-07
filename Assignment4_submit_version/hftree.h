#ifndef HFTREE_H
#define HFTREE_H

#include <iostream>
#include <string>
#include <queue>
#include <map>
#include <vector>
#include <stack>
using namespace std;

class HuffmanTreeNode
{
public:
	HuffmanTreeNode() : huffman_code(""), leftChild(NULL), rightChild(NULL) {}
	char key; //Represent character, only in leaf nodes
	int weight; //frequency of a character for leaf nodes. For non-leaf nodes, it always equals to the sum weight of its all children. 
	//Your priority queue should be built based on this value
	string huffman_code; //encoding sequence from root to this node
	HuffmanTreeNode *leftChild, *rightChild;
};

class HuffmanTree
{
public:
	HuffmanTree();

	// initialize huffman tree with a given frequency table
	void loadMap(map<char, int> frequency_table);

	// calculate the encoding table containing all characters
	void encode(map<char, string>& encoded_table);

	// decode a bit string back to plaintext
	string decode(const string& bin_str);

	// release all tree nodes
	// you must use non-recursive post-order traversal to release the tree
	// if you use recursive deletion, you will lose partial marks of this PA
	void release();

private:
		// root of tree
	HuffmanTreeNode *hfTree;

	void mergeTree(HuffmanTreeNode *bt1, HuffmanTreeNode *bt2,
		HuffmanTreeNode *pt);
};



#endif // HFTREE_H