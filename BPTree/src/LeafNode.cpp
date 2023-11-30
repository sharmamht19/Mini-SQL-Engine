#include "LeafNode.hpp"

#include "InternalNode.hpp"
#include "RecordPtr.hpp"

LeafNode::LeafNode(const TreePtr &tree_ptr) : TreeNode(LEAF, tree_ptr) {
    this->data_pointers.clear();
    this->next_leaf_ptr = NULL_PTR;
    if (!is_null(tree_ptr))
        this->load();
}

// returns max key within this leaf
Key LeafNode::max() {
    auto it = this->data_pointers.rbegin();
    return it->first;
}

//*****************************************| INSERTION PART START |**************************************

// inserts <key, record_ptr> to leaf. If overflow occurs, leaf is split
// split node is returned
// TODO: LeafNode::insert_key to be implemented

TreePtr LeafNode::insert_key(const Key &key, const RecordPtr &record_ptr) {
    cout << "Entering insert_key()" << endl;

    // If not full, just insert the key and return
    if (!isFull()) {
        cout << "Not full, inserting into current leaf." << endl;
        insertIntoCurrentLeaf(key, record_ptr);
        return NULL_PTR;
    }

    cout << "Node is full. Splitting node..." << endl;

    // Split the node into two parts
    LeafNode *newLeaf = splitLeafNode(key, record_ptr);

    cout << "Setting next pointer to the new leaf..." << endl;

    // Set the next pointer to the new leaf
    this->next_leaf_ptr = newLeaf->tree_ptr;

    this->dump();
    newLeaf->dump();

    return newLeaf->tree_ptr;
}

bool LeafNode::isFull() {
    cout << "Checking if node is full..." << endl;
    return this->data_pointers.size() >= FANOUT;
}

void LeafNode::insertIntoCurrentLeaf(const Key &key, const RecordPtr &record_ptr) {
    cout << "Entering insertIntoCurrentLeaf()" << endl;
    this->data_pointers.insert({key, record_ptr});
    this->size++;
    this->dump();
}

LeafNode *LeafNode::splitLeafNode(const Key &key, const RecordPtr &record_ptr) {
    cout << "Entering splitLeafNode()" << endl;
    // Create a temporary map and insert the new key-value pair
    map<Key, RecordPtr> temp_data_ptr = this->data_pointers;
    temp_data_ptr.insert({key, record_ptr});

    this->data_pointers.clear();

    // Calculate where to split the entries
    int splitPosition = ceil(double(FANOUT) / 2.0);
    auto iter = temp_data_ptr.begin();

    cout << "Populating original leaf with the first half of entries..." << endl;

    while (iter != temp_data_ptr.end() && this->data_pointers.size() < splitPosition) {
        this->data_pointers.insert(*iter);
        iter++;
    }

    LeafNode *newLeaf = new LeafNode();
    cout << "Populating the new leaf with the second half of entries..." << endl;
    while (iter != temp_data_ptr.end()) {
        newLeaf->data_pointers.insert(*iter);
        iter++;
    }
    this->size = this->data_pointers.size();
    newLeaf->size = temp_data_ptr.size() - this->size;

    cout << "Exiting splitLeafNode()" << endl;

    return newLeaf;
}
//*****************************************| INSERTION PART END |**************************************

//*****************************************| DELETION PART START |**************************************

void LeafNode::delete_key(const Key &key, stack<pair<TreePtr, int>> parentStack, stack<pair<TreePtr, TreePtr>> siblingStack) {
    cout << "LeafNode::delete_key Started" << endl;

    TreePtr childRightSibling = NULL_PTR;
    TreePtr childLeftSibling = NULL_PTR;
    TreePtr left_sibling = NULL_PTR;
    TreePtr right_sibling = NULL_PTR;
    TreePtr parent_ptr = NULL_PTR;
    int parent_index = DELETE_MARKER;
    cout << " stack size : " << siblingStack.size() << ": " << parentStack.size() << endl;
    if (siblingStack.size() && parentStack.size()) {
        // Extract siblings and parent information..
        left_sibling = siblingStack.top().first;
        right_sibling = siblingStack.top().second;
        parent_ptr = parentStack.top().first;
        parent_index = parentStack.top().second;
        siblingStack.pop();
        parentStack.pop();
    }

    if (this->data_pointers.count(key) == 0) {
        return;
    }

    this->data_pointers.erase(key);
    this->size = this->data_pointers.size();
    this->dump();

    if (isUnderflow(this->data_pointers.size())) {
        cout << childRightSibling << ", " << childLeftSibling << ", " << left_sibling << ", " << right_sibling << ", " << parent_ptr << endl;
        if (left_sibling != NULL_PTR) {
            if (left_redistribution(left_sibling, right_sibling, parent_ptr, parent_index))
            return;
        }

        if (left_sibling != NULL_PTR) {
            left_merge(left_sibling, right_sibling, parent_ptr, parent_index);
            return;
        }

        if (right_sibling != NULL_PTR) {
            if (right_redistribution(right_sibling, parent_ptr, parent_index)) 
            return;
        }

        if (right_sibling != NULL_PTR) {
            right_merge(right_sibling, parent_ptr, parent_index);
            return ;
        }
    }
}

bool LeafNode ::left_redistribution(TreePtr leftSibling, TreePtr rightSibling, TreePtr parentPtr, int parentIndex) {
    LeafNode *leftNode = new LeafNode(leftSibling);
    cout << "LeafNode::left_redistribution Started" << endl;
    if (!isUnderflow(leftNode->data_pointers.size() - 1)) {
        auto max_key_it = leftNode->data_pointers.rbegin();
        auto max_key_val = max_key_it->first;
        auto max_record_ptr = max_key_it->second;

        // Insert the largest key and record pointer into the current node.
        this->data_pointers.emplace(max_key_val, max_record_ptr);
        leftNode->data_pointers.erase(max_key_val);
        leftNode->size = leftNode->data_pointers.size();
        leftNode->dump();
        if (parentPtr != NULL_PTR) {
            InternalNode *parentNode = new InternalNode(parentPtr);
            Key key = leftNode->data_pointers.rbegin()->first;
            if (rightSibling == NULL_PTR) {
                parentNode->keys[parentIndex] = key;
            } else {
                parentNode->keys[parentIndex - 1] = key;
            }
            parentNode->dump();
        }

        this->size = this->data_pointers.size();

        this->dump();
        cout << "LeafNode::left redistribution Ended" << endl;
        return true;
    }
    return false;
}

void LeafNode ::left_merge(TreePtr leftSibling, TreePtr rightSibling, TreePtr parent_ptr, int parentIndex) {
    cout << "LeafNode::left_merge Started" << endl;
    InternalNode *parentNode = new InternalNode(parent_ptr);
    LeafNode *leftNode = new LeafNode(leftSibling);
    for (auto itr : this->data_pointers) {
        leftNode->data_pointers[itr.first] = itr.second;
    }

    leftNode->next_leaf_ptr = this->next_leaf_ptr;

    if (rightSibling != NULL_PTR) {
        for (int i = parentIndex - 1; i < parentNode->keys.size() - 1; i++) {
            parentNode->keys[i] = parentNode->keys[i + 1];
        }
        for (int i = parentIndex; i < parentNode->tree_pointers.size() - 1; i++) {
            parentNode->tree_pointers[i] = parentNode->tree_pointers[i + 1];
        }
    }

    parentNode->keys.resize(parentNode->keys.size() - 1);
    parentNode->tree_pointers.resize(parentNode->tree_pointers.size() - 1);
    parentNode->size = parentNode->tree_pointers.size();
    leftNode->size = leftNode->data_pointers.size();
    parentNode->dump();
    leftNode->dump();
    cout << "LeafNode::left merge Ended" << endl;
    return;
}

bool LeafNode ::right_redistribution(TreePtr rightSibling, TreePtr parent, int parentIndex) {
    LeafNode *rightNode = new LeafNode(rightSibling);
    cout << "LeafNode::right_redistribution Started" << endl;
    if (!isUnderflow(rightNode->data_pointers.size() - 1)) {
        auto keyItr = begin(rightNode->data_pointers);
        auto keyVal = keyItr->first;
        auto record_ptr = keyItr->second;

        this->data_pointers.emplace(keyVal, record_ptr);
        rightNode->data_pointers.erase(keyItr);
        rightNode->size = rightNode->data_pointers.size();
        rightNode->dump();

        cout << "New Right Node size = " << rightNode->size << endl;

        if (parent != NULL_PTR) {
            InternalNode *parentNode = new InternalNode(parent);
            parentNode->keys[parentIndex] = keyVal;
            parentNode->dump();
        }

        this->size = this->data_pointers.size();
        this->dump();
        cout << "LeafNode::right_redistribution Ended" << endl;
        return true;
    }
    return false;
}

void LeafNode ::right_merge(TreePtr right_sibling, TreePtr parent_ptr, int parent_index) {
    cout << "LeafNode::right_merge Started" << endl;
  
    InternalNode *parentNode = new InternalNode(parent_ptr); 
    LeafNode *rightNode = new LeafNode(right_sibling);
     for (auto itr : rightNode->data_pointers) {
        this->data_pointers[itr.first] = itr.second;
    }
     this->next_leaf_ptr = rightNode->next_leaf_ptr;
    
    parentNode->tree_pointers.erase(parentNode->tree_pointers.begin() + parent_index + 1 );
    parentNode->keys.erase(parentNode->keys.begin() + parent_index);
    parentNode->size = parentNode->tree_pointers.size();
    
    cout<<" parent size : "<<parentNode<<parentNode->size<<endl;
    parentNode->dump();
    this->size = this->data_pointers.size();
    this->dump();
    cout << "LeafNode::right_merge Ended" << endl;
    return;
}

bool LeafNode ::isUnderflow(int keys) {
    return ((FANOUT + 1) / 2 > keys);
}
bool LeafNode::isOverflow(int keys) {
    return keys == FANOUT;
}

//*****************************************| DELETION PART END |**************************************

// runs range query on leaf
void LeafNode::range(ostream &os, const Key &min_key, const Key &max_key) const {
    BLOCK_ACCESSES++;
    for (const auto &data_pointer : this->data_pointers) {
        if (data_pointer.first >= min_key && data_pointer.first <= max_key)
            data_pointer.second.write_data(os);
        if (data_pointer.first > max_key)
            return;
    }
    if (!is_null(this->next_leaf_ptr)) {
        auto next_leaf_node = new LeafNode(this->next_leaf_ptr);
        next_leaf_node->range(os, min_key, max_key);
        delete next_leaf_node;
    }
}

// exports node - used for grading
void LeafNode::export_node(ostream &os) {
    TreeNode::export_node(os);
    for (const auto &data_pointer : this->data_pointers) {
        os << data_pointer.first << " ";
    }
    os << endl;
}

// writes leaf as a mermaid chart
void LeafNode::chart(ostream &os) {
    string chart_node = this->tree_ptr + "[" + this->tree_ptr + BREAK;
    chart_node += "size: " + to_string(this->size) + BREAK;
    for (const auto &data_pointer : this->data_pointers) {
        chart_node += to_string(data_pointer.first) + " ";
    }
    chart_node += "]";
    os << chart_node << endl;
}

ostream &LeafNode::write(ostream &os) const {
    TreeNode::write(os);
    for (const auto &data_pointer : this->data_pointers) {
        if (&os == &cout)
            os << "\n"
               << data_pointer.first << ": ";
        else
            os << "\n"
               << data_pointer.first << " ";
        os << data_pointer.second;
    }
    os << endl;
    os << this->next_leaf_ptr << endl;
    return os;
}

istream &LeafNode::read(istream &is) {
    TreeNode::read(is);
    this->data_pointers.clear();
    for (int i = 0; i < this->size; i++) {
        Key key = DELETE_MARKER;
        RecordPtr record_ptr;
        if (&is == &cin)
            cout << "K: ";
        is >> key;
        if (&is == &cin)
            cout << "P: ";
        is >> record_ptr;
        this->data_pointers.insert(pair<Key, RecordPtr>(key, record_ptr));
    }
    is >> this->next_leaf_ptr;
    return is;
}
