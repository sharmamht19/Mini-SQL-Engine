#ifndef BPTREE_GLOBAL_HPP
#define BPTREE_GLOBAL_HPP

#include <unistd.h>

#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <fstream>
#include <iostream>
#include <numeric>
#include <random>
#include <stack>
#include <string>
#include <vector>

using namespace std;

extern const uint BLOCK_SIZE;     // in records i.e. block can store 10 records
extern const uint FANOUT;         // fanout of b+tree
extern const uint MIN_OCCUPANCY;  // minimum number of children in each node

extern const string DATA_PATH;
extern const string TEMP_PATH;
extern const int DELETE_MARKER;
extern const string NULL_PTR;
extern const string BREAK;

extern uint BLOCK_ACCESSES;
class Block;
class RecordPtr;
class UnorderedHeap;
class TreeNode;
class InternalNode;
class LeafNode;
class BPTree;

string gen_random_file_name();
vector<int> generate_random_file(const string& filename);
vector<int> read_random_file(const string& filename);
void save_to_csv(const vector<int>& data_i_values, const vector<vector<int>>& block_accesses, int fanout);
void initialize();
void clear_temp_folder();
void delete_file(string file_name);
bool is_null(string pointer);
extern UnorderedHeap heap;
extern BPTree bptree;
#endif  // BPTREE_GLOBAL_HPP
