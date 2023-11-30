#include "InternalNode.hpp"

// creates internal node pointed to by tree_ptr or creates a new one
InternalNode::InternalNode(const TreePtr &tree_ptr) : TreeNode(INTERNAL, tree_ptr) {
    this->keys.clear();
    this->tree_pointers.clear();
    if (!is_null(tree_ptr))
        this->load();
}

// max element from tree rooted at this node
Key InternalNode::max() {
    Key max_key = DELETE_MARKER;
    TreeNode *last_tree_node = TreeNode::tree_node_factory(this->tree_pointers[this->size - 1]);
    max_key = last_tree_node->max();
    delete last_tree_node;
    return max_key;
}

// if internal node contains a single child, it is returned
TreePtr InternalNode::single_child_ptr() {
    if (this->size == 1)
        return this->tree_pointers[0];
    return NULL_PTR;
}

//*****************************************| INSERTION PART START |**************************************

// inserts <key, record_ptr> into subtree rooted at this node.
// returns pointer to split node if exists

TreePtr InternalNode::insert_key(const Key &key, const RecordPtr &record_ptr) {
    cout << "[InternalNode] Inserting key: " << key << endl;
    TreePtr newChildPtr = NULL_PTR;

    // Determine the position for insertion and fetch the child node to insert into
    int insertIndex = lower_bound(this->keys.begin(), this->keys.end(), key) - this->keys.begin();
    TreePtr childPtr = this->tree_pointers[insertIndex];

    TreeNode *child = TreeNode::tree_node_factory(childPtr);

    // Insert the key into the fetched child and check for potential overflow (split)
    TreePtr split_ptr = child->insert_key(key, record_ptr);

    // If child had to split, manage the new key and potential child node split otherwise return null;
    if (is_null(split_ptr)) {
        this->dump();
        cout << "[InternalNode] Finished Insertion when split pointer is null." << endl;
        return newChildPtr;
    } else {
        TreePtr newInternalPtr = NULL_PTR;
        TreeNode *extraNode = TreeNode::tree_node_factory(split_ptr);
        Key splitKey = child->max();
        // If there's room, insert key into this internal node, else split the node
        if (this->keys.size() < FANOUT - 1) {
            this->insert_into_existing_node(splitKey, split_ptr);
        } else {
            newInternalPtr = this->split_and_create_new_node(splitKey, split_ptr);
        }
        this->dump();
        cout << "[InternalNode] Finished Insertion." << endl;
        return newInternalPtr;
    }
}

// Insert a new key into the current node without having to split
void InternalNode::insert_into_existing_node(const Key &splitKey, const TreePtr &split_ptr) {
    cout << "[InternalNode] Inserting into existing node." << endl;

    int pos = upper_bound(this->keys.begin(), this->keys.end(), splitKey) - this->keys.begin();
    this->keys.insert(this->keys.begin() + pos, splitKey);
    this->tree_pointers.insert(this->tree_pointers.begin() + pos + 1, split_ptr);

    this->size = this->tree_pointers.size();
}

// Split the current node and create a new one
TreePtr InternalNode::split_and_create_new_node(const Key &splitKey, const TreePtr &split_ptr) {
    cout << "[InternalNode] Splitting the node." << endl;

    // Prepare temporary containers and insert the split key
    vector<Key> tempKeys = this->keys;
    vector<TreePtr> tempTreePointers = this->tree_pointers;

    int pos = upper_bound(tempKeys.begin(), tempKeys.end(), splitKey) - tempKeys.begin();
    tempKeys.insert(tempKeys.begin() + pos, splitKey);
    tempTreePointers.insert(tempTreePointers.begin() + pos + 1, split_ptr);

    // Determine the partition index for the split
    int partitionIndex = ceil(double(FANOUT) / 2.0) - 1;

    vector<Key> newKeys(begin(tempKeys) + partitionIndex + 1, end(tempKeys));
    vector<TreePtr> newTreePointers(begin(tempTreePointers) + partitionIndex + 1, end(tempTreePointers));

    // Adjust the current node's keys and pointers to represent the left side of the split
    this->keys.assign(begin(tempKeys), begin(tempKeys) + partitionIndex);
    this->tree_pointers.assign(begin(tempTreePointers), begin(tempTreePointers) + partitionIndex + 1);

    // Create the new internal node with the right side of the split and assing values accordingly .
    InternalNode *newInternalNode = new InternalNode();
    newInternalNode->keys = newKeys;
    newInternalNode->tree_pointers = newTreePointers;
    newInternalNode->size = newTreePointers.size();
    this->size = this->tree_pointers.size();
    newInternalNode->dump();
    return newInternalNode->tree_ptr;
}

//*****************************************| INSERTION PART END |**************************************

//*****************************************| DELETION PART START |**************************************

// deletes key from subtree rooted at this if exists
void InternalNode::delete_key(const Key &key, stack<pair<TreePtr, int>> parentStack, stack<pair<TreePtr, TreePtr>> siblingStack) {
    int rightSiblingIndex = -1;
    int leftSiblingIndex = -1;
    cout << "InternalNode:: DeleteKey Started" << endl;

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

    for (int i = 0; i < this->keys.size(); i++) {
        rightSiblingIndex = i + 1;
        leftSiblingIndex = i - 1;

        if (this->keys[i] >= key) {
            if (i == 0) {
                childLeftSibling = NULL_PTR;
            } else {
                childLeftSibling = this->tree_pointers[leftSiblingIndex];
            }
            childRightSibling = this->tree_pointers[rightSiblingIndex];
            TreeNode *current = TreeNode::tree_node_factory(this->tree_pointers[i]);
            siblingStack.push({childLeftSibling, childRightSibling});
            parentStack.push({this->tree_ptr, i});
            cout << " 1while pushing stack size : " << siblingStack.size() << ": " << parentStack.size() << endl;
            cout << " Before returning parents Size:" <<this<<" : "<< this->size << endl;
            current->delete_key(key, parentStack, siblingStack);
            cout << " after returning parents Size:" <<this<<" : "<< this->size << endl;
            break;
        }

        if (i == this->keys.size() - 1) {
            leftSiblingIndex = i;
            childLeftSibling = this->tree_pointers[leftSiblingIndex];
            childRightSibling = NULL_PTR;
            TreeNode *current = TreeNode::tree_node_factory(this->tree_pointers[i + 1]);
            siblingStack.push({childLeftSibling, childRightSibling});
            parentStack.push({this->tree_ptr, i});
            cout << " 2while pushing stack size : " << siblingStack.size() << ": " << parentStack.size() << endl;
            current->delete_key(key, parentStack, siblingStack);

            break;
        }
    }

    InternalNode *currentNode = new InternalNode(this->tree_ptr);

    if (!isUnderflow(currentNode->keys.size())) {
        cout << " No UnderFlow in internalnode:: Returning." << endl;
        return;
    }
    cout << " UnderFlow in internalnode:: checking." << endl;
    if (left_sibling != NULL_PTR) {
        if (leftRedistribution(currentNode, left_sibling, right_sibling, parent_ptr, parent_index))
            return;
    }

    if (left_sibling != NULL_PTR) {
        leftMerge(currentNode, left_sibling, right_sibling, parent_ptr, parent_index);
        return;
    }

    if (right_sibling != NULL_PTR) {
        if (leftRedistribution(currentNode, left_sibling, right_sibling, parent_ptr, parent_index))
            return;
    }

    if (right_sibling != NULL_PTR) {
        rightMerge(currentNode, right_sibling, parent_ptr, parent_index);
        return;
    }

    cout << " InternalNode :: DeleteKey Ended;" << currentNode->size << endl;
}

bool InternalNode::leftRedistribution(InternalNode *currentNode, TreePtr leftSibling, TreePtr rightSibling,
                                      TreePtr parentPtr, int parentIndex) {
    cout << "InternalNode:: left Redistribution Started" << endl;
    InternalNode *parentNode = new InternalNode(parentPtr);
    InternalNode *leftNode = new InternalNode(leftSibling);
    if (!isUnderflow(leftNode->keys.size() - 1)) {
        TreePtr maxRecordPtr = leftNode->tree_pointers.back();
        Key maxKeyVal = leftNode->keys.back();
        auto keyItr = currentNode->keys.begin();

        if (rightSibling != NULL_PTR) {
            currentNode->keys.insert(keyItr, parentNode->keys[parentIndex - 1]);
            parentNode->keys[parentIndex - 1] = maxKeyVal;
        } else {
            currentNode->keys.insert(keyItr, parentNode->keys[parentIndex]);
            parentNode->keys[parentIndex] = maxKeyVal;
        }

        leftNode->keys.pop_back();
        leftNode->tree_pointers.pop_back();
        auto tpItr = currentNode->tree_pointers.begin();
        currentNode->tree_pointers.insert(tpItr, maxRecordPtr);
        currentNode->size = currentNode->tree_pointers.size();
        leftNode->size = leftNode->tree_pointers.size();

        leftNode->dump();
        currentNode->dump();
        parentNode->dump();
        cout << "InternalNode:: left redistribution Ended" << endl;

        return true;
    }
    return false;
}

void InternalNode::leftMerge(InternalNode *currentNode, TreePtr leftSibling, TreePtr rightSibling,
                             TreePtr parentPtr, int parentIndex) {
    cout << "InternalNode:: left merge Started" << endl;

    InternalNode *parentNode = new InternalNode(parentPtr);
    InternalNode *leftNode = new InternalNode(leftSibling);
    int borrow_index = parentIndex;
    if (rightSibling != NULL_PTR) {
        --borrow_index;
        leftNode->keys.push_back(parentNode->keys[borrow_index]);
    } else {
        leftNode->keys.push_back(parentNode->keys[borrow_index]);
    }

    for (auto key : currentNode->keys) {
        leftNode->keys.push_back(key);
    }

    for (auto tp : currentNode->tree_pointers) {
        leftNode->tree_pointers.push_back(tp);
    }

    currentNode->keys.clear();
    currentNode->tree_pointers.clear();

    parentNode->keys.erase(parentNode->keys.begin() + borrow_index);
    parentNode->tree_pointers.erase(parentNode->tree_pointers.begin() + borrow_index + 1);

    parentNode->size = parentNode->tree_pointers.size();
    leftNode->size = leftNode->tree_pointers.size();

    parentNode->dump();
    leftNode->dump();
    cout << "InternalNode:: left merge Ended" << endl;
    return;
}

bool InternalNode::rightRedistribution(InternalNode *currentNode, TreePtr leftSibling, TreePtr rightSibling,
                                       TreePtr parentPtr, int parentIndex) {
    cout << "InternalNode:: rightRedistribution Started" << endl;
    InternalNode *rightNode = new InternalNode(rightSibling);
    InternalNode *parentNode = new InternalNode(parentPtr);
    if (!isUnderflow(rightNode->keys.size() - 1)) {
        Key max_key = rightNode->keys.front();
        TreePtr max_record_ptr = rightNode->tree_pointers.front();
        rightNode->keys.erase(rightNode->keys.begin());
        rightNode->tree_pointers.erase(rightNode->tree_pointers.begin());

        currentNode->keys.push_back(parentNode->keys[parentIndex]);
        currentNode->tree_pointers.push_back(max_record_ptr);

        parentNode->keys[parentIndex] = max_key;

        rightNode->size = rightNode->tree_pointers.size();
        currentNode->size = currentNode->tree_pointers.size();

        currentNode->dump();
        rightNode->dump();
        parentNode->dump();
        cout << "InternalNode:: right redistribution ended" << endl;
        return true;
    }
    return false;
}

void InternalNode::rightMerge(InternalNode *currentNode, TreePtr rightSibling,
                              TreePtr parentPtr, int parentIndex) {
    cout << "InternalNode:: right merge Started" << endl;
    InternalNode *right_node = new InternalNode(rightSibling);
    InternalNode *parent_node = new InternalNode(parentPtr);

    currentNode->keys.push_back(parent_node->keys[parentIndex]);

    for (auto key : right_node->keys) {
        currentNode->keys.push_back(key);
    }

    for (auto tp : right_node->tree_pointers) {
        currentNode->tree_pointers.push_back(tp);
    }

    right_node->keys.clear();
    right_node->tree_pointers.clear();

    parent_node->keys.erase(parent_node->keys.begin() + parentIndex);
    parent_node->tree_pointers.erase(parent_node->tree_pointers.begin() + parentIndex + 1);
    parent_node->size = parent_node->tree_pointers.size();
    currentNode->size = currentNode->tree_pointers.size();

    currentNode->dump();
    parent_node->dump();

    delete right_node;
    cout << "InternalNode:: right merge Ended" << endl;
    return;
}

bool InternalNode ::isUnderflow(int keys) {
    return ((FANOUT + 1) / 2 > keys);
}
bool InternalNode::isOverflow(int keys) {
    return keys == FANOUT;
}

//*****************************************| DELETION PART END |**************************************

// runs range query on subtree rooted at this node
void InternalNode::range(ostream &os, const Key &min_key, const Key &max_key) const {
    BLOCK_ACCESSES++;
    for (int i = 0; i < this->size - 1; i++) {
        if (min_key <= this->keys[i]) {
            auto *child_node = TreeNode::tree_node_factory(this->tree_pointers[i]);
            child_node->range(os, min_key, max_key);
            delete child_node;
            return;
        }
    }
    auto *child_node = TreeNode::tree_node_factory(this->tree_pointers[this->size - 1]);
    child_node->range(os, min_key, max_key);
    delete child_node;
}

// exports node - used for grading
void InternalNode::export_node(ostream &os) {
    TreeNode::export_node(os);
    for (int i = 0; i < this->size - 1; i++)
        os << this->keys[i] << " ";
    os << endl;
    for (int i = 0; i < this->size; i++) {
        auto child_node = TreeNode::tree_node_factory(this->tree_pointers[i]);
        child_node->export_node(os);
        delete child_node;
    }
}

// writes subtree rooted at this node as a mermaid chart
void InternalNode::chart(ostream &os) {
    string chart_node = this->tree_ptr + "[" + this->tree_ptr + BREAK;
    chart_node += "size: " + to_string(this->size) + BREAK;
    chart_node += "]";
    os << chart_node << endl;

    for (int i = 0; i < this->size; i++) {
        auto tree_node = TreeNode::tree_node_factory(this->tree_pointers[i]);
        tree_node->chart(os);
        delete tree_node;
        string link = this->tree_ptr + "-->|";

        if (i == 0)
            link += "x <= " + to_string(this->keys[i]);
        else if (i == this->size - 1) {
            link += to_string(this->keys[i - 1]) + " < x";
        } else {
            link += to_string(this->keys[i - 1]) + " < x <= " + to_string(this->keys[i]);
        }
        link += "|" + this->tree_pointers[i];
        os << link << endl;
    }
}

ostream &InternalNode::write(ostream &os) const {
    TreeNode::write(os);
    for (int i = 0; i < this->size - 1; i++) {
        if (&os == &cout)
            os << "\nP" << i + 1 << ": ";
        os << this->tree_pointers[i] << " ";
        if (&os == &cout)
            os << "\nK" << i + 1 << ": ";
        os << this->keys[i] << " ";
    }
    if (&os == &cout)
        os << "\nP" << this->size << ": ";
    os << this->tree_pointers[this->size - 1];
    return os;
}

istream &InternalNode::read(istream &is) {
    TreeNode::read(is);
    this->keys.assign(this->size - 1, DELETE_MARKER);
    this->tree_pointers.assign(this->size, NULL_PTR);
    for (int i = 0; i < this->size - 1; i++) {
        if (&is == &cin)
            cout << "P" << i + 1 << ": ";
        is >> this->tree_pointers[i];
        if (&is == &cin)
            cout << "K" << i + 1 << ": ";
        is >> this->keys[i];
    }
    if (&is == &cin)
        cout << "P" << this->size;
    is >> this->tree_pointers[this->size - 1];
    return is;
}