#ifndef AVLBST_H
#define AVLBST_H

#include <iostream>
#include <exception>
#include <cstdlib>
#include <cstdint>
#include <algorithm>
#include "bst.h"

struct KeyError { };

/**
* A special kind of node for an AVL tree, which adds the balance as a data member, plus
* other additional helper functions. You do NOT need to implement any functionality or
* add additional data members or helper functions.
*/
template <typename Key, typename Value>
class AVLNode : public Node<Key, Value>
{
public:
    // Constructor/destructor.
    AVLNode(const Key& key, const Value& value, AVLNode<Key, Value>* parent);
    virtual ~AVLNode();

    // Getter/setter for the node's height.
    int8_t getBalance () const;
    void setBalance (int8_t balance);
    void updateBalance(int8_t diff);

    // Getters for parent, left, and right. These need to be redefined since they
    // return pointers to AVLNodes - not plain Nodes. See the Node class in bst.h
    // for more information.
    virtual AVLNode<Key, Value>* getParent() const override;
    virtual AVLNode<Key, Value>* getLeft() const override;
    virtual AVLNode<Key, Value>* getRight() const override;

protected:
    int8_t balance_;    // effectively a signed char
};

/*
  -------------------------------------------------
  Begin implementations for the AVLNode class.
  -------------------------------------------------
*/

/**
* An explicit constructor to initialize the elements by calling the base class constructor
*/
template<class Key, class Value>
AVLNode<Key, Value>::AVLNode(const Key& key, const Value& value, AVLNode<Key, Value> *parent) :
    Node<Key, Value>(key, value, parent), balance_(0)
{

}

/**
* A destructor which does nothing.
*/
template<class Key, class Value>
AVLNode<Key, Value>::~AVLNode()
{

}

/**
* A getter for the balance of a AVLNode.
*/
template<class Key, class Value>
int8_t AVLNode<Key, Value>::getBalance() const
{
    return balance_;
}

/**
* A setter for the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::setBalance(int8_t balance)
{
    balance_ = balance;
}

/**
* Adds diff to the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::updateBalance(int8_t diff)
{
    balance_ += diff;
}

/**
* An overridden function for getting the parent since a static_cast is necessary to make sure
* that our node is a AVLNode.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getParent() const
{
    return static_cast<AVLNode<Key, Value>*>(this->parent_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getLeft() const
{
    return static_cast<AVLNode<Key, Value>*>(this->left_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getRight() const
{
    return static_cast<AVLNode<Key, Value>*>(this->right_);
}


/*
  -----------------------------------------------
  End implementations for the AVLNode class.
  -----------------------------------------------
*/


template <class Key, class Value>
class AVLTree : public BinarySearchTree<Key, Value>
{
public:
    virtual void insert (const std::pair<const Key, Value> &new_item) override; // TODO
    virtual void remove(const Key& key) override;  // TODO
protected:
    virtual void nodeSwap(Node<Key,Value>* n1, Node<Key,Value>* n2) override;
    // Add helper functions here
    void rotateLeft(AVLNode<Key, Value>* node);
    void rotateRight(AVLNode<Key, Value>* node);
    void rebalance(AVLNode<Key, Value>* node);

};

/*
 * Recall: If key is already in the tree, you should 
 * overwrite the current value with the updated value.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::insert (const std::pair<const Key, Value> &new_item)
{
    // TODO
    // Create a new AVLNode.
    AVLNode<Key, Value>* newNode = new AVLNode<Key, Value>(new_item.first, new_item.second, nullptr);
    if(this->root_ == nullptr) {
        this->root_ = newNode;
        return;
    }
    
    // Perform standard BST insertion.
    AVLNode<Key, Value>* parent = nullptr;
    AVLNode<Key, Value>* current = static_cast<AVLNode<Key, Value>*>(this->root_);
    while(current != nullptr) {
        parent = current;
        if(new_item.first < current->getKey())
            current = current->getLeft();
        else if(new_item.first > current->getKey())
            current = current->getRight();
        else {
            // Key already exists: update the value and clean up.
            current->setValue(new_item.second);
            delete newNode;
            return;
        }
    }
    newNode->setParent(parent);
    if(new_item.first < parent->getKey())
        parent->setLeft(newNode);
    else
        parent->setRight(newNode);
    
    // Propagate balance updates upward.
    AVLNode<Key, Value>* child = newNode;
    current = parent;
    while(current != nullptr) {
        if(child == current->getLeft())
            current->updateBalance(-1);
        else
            current->updateBalance(1);
        
        // If the balance factor becomes 0, the subtree’s height hasn’t increased.
        if(current->getBalance() == 0)
            break;
        
        // If the balance factor is now 2 or -2, rebalance the node.
        if(std::abs(current->getBalance()) == 2) {
            rebalance(current);
            break;
        }
        
        child = current;
        current = current->getParent();
    
    }
}

/*
 * Recall: The writeup specifies that if a node has 2 children you
 * should swap with the predecessor and then remove.
 */
template<class Key, class Value>
void AVLTree<Key, Value>:: remove(const Key& key)
{
    // TODO
    // Find the node to remove.
    AVLNode<Key, Value>* nodeToRemove = static_cast<AVLNode<Key, Value>*>(this->internalFind(key));
    if (nodeToRemove == nullptr)
        return;  // Key not found.
    
    AVLNode<Key, Value>* parent = nodeToRemove->getParent();
    
    // If the node has two children, swap it with its predecessor.
    if (nodeToRemove->getLeft() != nullptr && nodeToRemove->getRight() != nullptr) {
        AVLNode<Key, Value>* pred = static_cast<AVLNode<Key, Value>*>(this->predecessor(nodeToRemove));
        // Use the provided base class nodeSwap (which swaps pointers, not just key/value).
        this->nodeSwap(nodeToRemove, pred);
        // After swapping, update the parent pointer.
        parent = nodeToRemove->getParent();
    }
    
    // Now nodeToRemove has at most one child.
    AVLNode<Key, Value>* child = (nodeToRemove->getLeft() != nullptr) ? nodeToRemove->getLeft() : nodeToRemove->getRight();
    if(child != nullptr)
        child->setParent(parent);
    
    if(parent == nullptr) {
        // Removing the root.
        this->root_ = child;
        delete nodeToRemove;
        return;
    }
    else if(nodeToRemove == parent->getLeft()) {
        parent->setLeft(child);
        parent->updateBalance(1);  // Removal from the left increases parent's balance.
    }
    else {
        parent->setRight(child);
        parent->updateBalance(-1); // Removal from the right decreases parent's balance.
    }
    
    delete nodeToRemove;
    
    // Propagate balance updates upward.
    AVLNode<Key, Value>* current = parent;
    while(current != nullptr) {
        // If the absolute balance factor is 1, the height didn't change; stop.
        if(std::abs(current->getBalance()) == 1)
            break;
        else if(std::abs(current->getBalance()) == 2)
            rebalance(current);
        
        current = current->getParent();
    }
}
template<class Key, class Value>
void AVLTree<Key, Value>::rotateLeft(AVLNode<Key, Value>* node)
{
    AVLNode<Key, Value>* r = node->getRight();
    node->setRight(r->getLeft());
    if (r->getLeft() != nullptr)
        r->getLeft()->setParent(node);
    r->setParent(node->getParent());
    if (node->getParent() == nullptr)
        this->root_ = r;
    else if (node == node->getParent()->getLeft())
        node->getParent()->setLeft(r);
    else
        node->getParent()->setRight(r);
    r->setLeft(node);
    node->setParent(r);
    
    // Update balance factors using standard formulas.
    int8_t rBalance = r->getBalance();
    node->setBalance(node->getBalance() - 1 - std::max(rBalance, (int8_t)0));
    r->setBalance(r->getBalance() - 1 + std::min(node->getBalance(), (int8_t)0));
}

template<class Key, class Value>
void AVLTree<Key, Value>::rotateRight(AVLNode<Key, Value>* node)
{
    AVLNode<Key, Value>* l = node->getLeft();
    node->setLeft(l->getRight());
    if (l->getRight() != nullptr)
        l->getRight()->setParent(node);
    l->setParent(node->getParent());
    if (node->getParent() == nullptr)
        this->root_ = l;
    else if (node == node->getParent()->getRight())
        node->getParent()->setRight(l);
    else
        node->getParent()->setLeft(l);
    l->setRight(node);
    node->setParent(l);
    
    int8_t lBalance = l->getBalance();
    node->setBalance(node->getBalance() + 1 - std::min(lBalance, (int8_t)0));
    l->setBalance(l->getBalance() + 1 + std::max(node->getBalance(), (int8_t)0));
}

template<class Key, class Value>
void AVLTree<Key, Value>::rebalance(AVLNode<Key, Value>* node)
{
    if (node->getBalance() == -2) {
        if (node->getLeft()->getBalance() <= 0)
            rotateRight(node);        // Left-Left case.
        else {
            rotateLeft(node->getLeft()); // Left-Right case.
            rotateRight(node);
        }
    }
    else if (node->getBalance() == 2) {
        if (node->getRight()->getBalance() >= 0)
            rotateLeft(node);         // Right-Right case.
        else {
            rotateRight(node->getRight()); // Right-Left case.
            rotateLeft(node);
        }
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::nodeSwap(Node<Key,Value>* n1, Node<Key,Value>* n2)
{
    // First, call the base class version to swap the Node parts.
    BinarySearchTree<Key, Value>::nodeSwap(n1, n2);
    
    // Then, cast to AVLNode pointers to swap the AVL-specific balance.
    AVLNode<Key, Value>* avl1 = static_cast<AVLNode<Key, Value>*>(n1);
    AVLNode<Key, Value>* avl2 = static_cast<AVLNode<Key, Value>*>(n2);
    
    int8_t tempB = avl1->getBalance();
    avl1->setBalance(avl2->getBalance());
    avl2->setBalance(tempB);
}


#endif
