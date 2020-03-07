#include "hftree.h"

HuffmanTree::HuffmanTree() : hfTree(NULL) {}

/*void print(HuffmanTreeNode* root, int level) {
	if (!root) return;
	print(root->rightChild, level + 1);
	for (int i = 0; i < level; ++i)
		cout << i;
	if (!root->key) cout << "N" << root->weight << endl;
	else if (root->key == ' ') cout << "S" << root->weight << endl;
	else cout << root->key << root->weight << endl;
	print(root->leftChild, level + 1);
}*/

struct CompareDist {
	bool operator()(pair<HuffmanTreeNode*, int> n1, pair<HuffmanTreeNode*, int> n2) {
		//cout << "What's here? n1: " << n1.first->key << " n2: " << n2.first->key << endl;
		if (n1.second == n2.second) {
			//cout << "What's here in recursion? n1: " << n1.first->key << " n2: " << n2.first->key << endl;
			char first_min_n1 = min(n1.first), second_min_n1 = min(n2.first);
			return first_min_n1 > second_min_n1;
		}
		else
			return n1.second > n2.second; //I guess n1 is the new node being pushed, since when true, n1 pushed
	}
	char min(HuffmanTreeNode* root) {
		if ((!root->leftChild) && (!root->rightChild)) //only return leaf's value
			return root->key;
		char min_left = min(root->leftChild), min_right = min(root->rightChild);
		return (min_left < min_right ? min_left : min_right);
	}
};

void HuffmanTree::loadMap(map<char, int> frequency_table)
{
	// create an minimum priority queue in STL
	// insert your code here ...
	map<char, int>::iterator map_itr = frequency_table.begin(); //Use iterator, then insert, it would be fine until the frequency_table is empty
	priority_queue<pair<HuffmanTreeNode*, int>, vector<pair<HuffmanTreeNode*, int>>, CompareDist> pq; //First, make a priority queue of pairs<HuffmanTreeNode*, int>

	//Initialize priority queue with pair<HuffmanTreeNode*, int>
	while (map_itr != frequency_table.end()) {
		HuffmanTreeNode* new_node = new HuffmanTreeNode;
		new_node->key = map_itr->first;
		new_node->weight = map_itr->second; //New instruction
		pq.push(make_pair(new_node, map_itr->second));
		++map_itr;
	}

	//Build priority queue with distinct pair<HuffmanTreeNode*, int>, which HuffmanTreeNode* is the root of HuffmanTree
	while (pq.size() > 1) {
		HuffmanTreeNode* new_node = new HuffmanTreeNode;
		pair<HuffmanTreeNode*, int> first = pq.top(); pq.pop();
		pair<HuffmanTreeNode*, int> second = pq.top(); pq.pop();
		mergeTree(first.first, second.first, new_node);
		pq.push(make_pair(new_node, first.second + second.second));
	}
	this->hfTree = pq.top().first;
	this->hfTree->huffman_code = "";
	//print(hfTree, 0); //Correctness in simple example screenshot
}

void HuffmanTree::mergeTree(HuffmanTreeNode *bt1, HuffmanTreeNode *bt2,
	HuffmanTreeNode *pt)
{
	// insert your code here ...
	pt->key = '\0'; //primitive types if not initialized contains garbage value
	pt->leftChild = bt1; //No need to delete any memory afterwards since merge simply re-combine the position of pointers
	pt->rightChild = bt2; //The only thing to delete is the priority queue, which are done in pop() already
	pt->weight = bt1->weight + bt2->weight;
}


void HuffmanTree::encode(map<char, string>& encoded_table)
{
	// go through every path from root to leaf, then get the encoding of each character(leaf node)
	// you are NOT allowed to use any recursive method here, otherwise you'll get no marks for this function
	// try level-order traversal on this Huffman Tree
	// result encoded_table: char -- character set, string -- encoding of leaf node, like "00", "1001"...

	// insert your code here ...
	HuffmanTreeNode* current = hfTree;
	string curr_string = hfTree->huffman_code;
	queue<pair<HuffmanTreeNode*, string>> hqueue; //whenever you push sth into it, use the current string
	while (current) {
		current->huffman_code = curr_string;
		/*if (current->key == '\0')
			cout << "The current key is: Null" << endl;
		else if (current->key == ' ')
			cout << "The current key is: Space" << endl;
		else if (current->key == 'P')
			cout << "The current key is: P, WTF" << endl;
		else
			cout << "The current key is: " << current->key << endl;*/
		//is the current key inserted in the encode_table?
		map<char, string>::iterator find_itr = encoded_table.begin();
		while (find_itr != encoded_table.end()) {
			if (find_itr->first == current->key)
				break;
			++find_itr;
		}
		//is the current inserted in the encode_table and it should be inserted?
		if (find_itr == encoded_table.end() && current->key != '\0') { //current->key isn't inserted, so insert it
			/*if (current->key == ' ')
				cout << "The inserted key is: Space" << endl;
			else
				cout << "The inserted key is: " << current->key << endl;*/
			encoded_table.insert({ current->key, curr_string });
		}
		//Regardless of inserted or not, still need to store next level
		if (current->leftChild) hqueue.push(make_pair(current->leftChild, curr_string + "0"));
		if (current->rightChild) hqueue.push(make_pair(current->rightChild, curr_string + "1"));
		//Done with this node, go for the next one in queue
		if (!hqueue.empty()) {
			current = hqueue.front().first;
			curr_string = hqueue.front().second;
			hqueue.pop();
		}
		else
			current = nullptr;
	}
}

string HuffmanTree::decode(const string& bin_str)
{
	// decode a binary string to plaintext
	// insert your code here ...
	string paper_txt = "", temp = "";

	HuffmanTreeNode* root = hfTree;
	//cout << dec << "bin_str size: " << bin_str.size() << endl;
	for (int i = 0; i < bin_str.size(); ++i) {
		if (bin_str[i] == '1') {
			root = root->rightChild;
			temp += '1';
		}
		else if (bin_str[i] == '0') {
			root = root->leftChild;
			temp += '0';
		}
		if (!root->leftChild && !root->rightChild) { //check a leaf after done
			//cout << "Letter: " << temp << endl;
			temp = "";
			paper_txt += root->key;
			root = hfTree;
		}
	}
	return paper_txt;
}

void HuffmanTree::release()
{
	// release all tree nodes in post-order: left then right then center
	// you must use non-recursive traversal to release the tree
	// you need to print out the weight of nodes with post-order when you delete the node
	// hint: try to use stack
	// if you use recursive deletion, you will lose partial marks of this PA

	cout << "[start releasing the huffman tree...]" << endl;
	if (hfTree == NULL)
		return;

	// store the nodes in vector path with post-order first
	vector<HuffmanTreeNode*> path; //use to store found addresses

	// insert your code here ...
	stack<HuffmanTreeNode*> root_storage;
	path.push_back(nullptr); //assumed nullptr means stored
	HuffmanTreeNode* current = hfTree; //some necessary initialization

	while (current) { //loop when the root has something
		vector<HuffmanTreeNode*>::iterator left_BST = path.begin(), right_BST = path.begin(); //seems that find can't be used, so write it urself.
		while (left_BST != path.end()) {
			if ((*left_BST) == current->leftChild) break;
			++left_BST;
		}
		while (right_BST != path.end()) {
			if ((*right_BST) == current->rightChild) break;
			++right_BST;
		}		if (left_BST == path.end()) { //left_BST not stored in path, deal with it
			root_storage.push(current);
			current = current->leftChild;
			continue;
		}
		if (right_BST == path.end()) { //right_BST not stored in path, deal with it
			root_storage.push(current);
			current = current->rightChild;
			continue;
		}
		path.push_back(current); //both left and right BST are dealt, so store the current into path
		if (!root_storage.empty()) { //if the stack isn't empty, put the return address into current
			current = root_storage.top();
			root_storage.pop();
		}
		else //if the stack is empty, there is nothing to return, so end the loop by nullifying current
			current = nullptr;
	}
	path.erase(path.begin()); //to remove the nullptr at first, this is important since this avoid some base cases

	for (int i = 0; i < path.size(); i++) {
		if (i != path.size() - 1)
			cout << dec << path[i]->weight << " ";
		else
			cout << dec << path[i]->weight << endl;

		// delete the node
		delete path[i];
	}
	cout << "[successfully released.]" << endl;
}