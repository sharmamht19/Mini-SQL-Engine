/**
 * LeafNode represents leaf nodes in the B+Tree. The leaf node contains
 * <key, RecordPtrs> pairs. Each leaf (except root) have between ceil(FANOUT/2)
 * and FANOUT children.
 */

#ifndef BPTREE_LEAFNODE_HPP
#define BPTREE_LEAFNODE_HPP

#include <map>

#include "TreeNode.hpp"

class LeafNode : public TreeNode {
   public:
    map<Key, RecordPtr> data_pointers;
    TreePtr next_leaf_ptr;

    // constructors
    explicit LeafNode(const TreePtr& tree_ptr = NULL_PTR);

    // helper functions
    Key max() override;

    // key functions
    TreePtr insert_key(const Key& key, const RecordPtr& record_ptr) override;  // TO DO -> STATUS :: Task Completed.
    void insertIntoCurrentLeaf(const Key& key, const RecordPtr& record_ptr);   // New created for insert..
    LeafNode* splitLeafNode(const Key& key, const RecordPtr& record_ptr);      // New created for insert..

    //Delete Functions
    void delete_key(const Key &key, stack<pair<TreePtr, int>> parentStack
,stack<pair<TreePtr, TreePtr>> siblingStack) override;  
    bool left_redistribution(TreePtr left_sibling, TreePtr right_sibling, TreePtr parent_ptr, int parent_index);
    void left_merge(TreePtr left_sibling, TreePtr right_sibling, TreePtr parent_ptr, int parent_index);
    bool right_redistribution(TreePtr right_sibling, TreePtr parent_ptr, int parent_index);
    void right_merge(TreePtr right_sibling, TreePtr parent_ptr, int parent_index);
    bool isUnderflow(int keys);
    bool isOverflow(int keys);
    void range(ostream& os, const Key& min_key, const Key& max_key) const override;
    void export_node(ostream& os) override;
    void chart(ostream& os) override;
    bool isFull();
    // I/O
    ostream& write(ostream& os) const override;
    istream& read(istream& is) override;
};

#endif  // BPTREE_LEAFNODE_HPP
