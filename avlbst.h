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
    virtual void insert (const std::pair<const Key, Value> &new_item); // TODO
    virtual void remove(const Key& key);  // TODO
protected:
    virtual void nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2);

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
    AVLNode<Key, Value>* newNode = new AVLNode<Key, Value>(new_item.first, new_item.second, nullptr);
    
    // If the tree is empty, simply set root_.
    if (this->root_ == nullptr) {
        this->root_ = newNode;
        return;
    }
    
    AVLNode<Key, Value>* current = static_cast<AVLNode<Key, Value>*>(this->root_);
    AVLNode<Key, Value>* parent = nullptr;
    while (current != nullptr) {
        parent = current;
        if (new_item.first < current->getKey())
            current = current->getLeft();
        else if (new_item.first > current->getKey())
            current = current->getRight();
        else {
            // Key already exists; overwrite the value.
            current->setValue(new_item.second);
            delete newNode;
            return;
        }
    }
    newNode->setParent(parent);
    if (new_item.first < parent->getKey())
        parent->setLeft(newNode);
    else
        parent->setRight(newNode);
    
    // After insertion, update balance factors up the tree.
    AVLNode<Key, Value>* child = newNode;
    current = parent;
    while (current != nullptr) {
        if (child == current->getLeft())
            current->updateBalance(-1);
        else
            current->updateBalance(1);
        
        // If balance becomes 0, the height hasn't changed; stop.
        if (current->getBalance() == 0)
            break;
        // If the node is unbalanced, rebalance it.
        if (current->getBalance() == -2 || current->getBalance() == 2) {
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
     // Find the node to remove (cast to AVLNode).
     AVLNode<Key, Value>* nodeToRemove = static_cast<AVLNode<Key, Value>*>(this->internalFind(key));
     if (nodeToRemove == nullptr){
         return;  // Key not found.
     }
     
     AVLNode<Key, Value>* parent = nodeToRemove->getParent();
     
     // If node has two children, swap with its predecessor.
     if (nodeToRemove->getLeft() != nullptr && nodeToRemove->getRight() != nullptr) {
        AVLNode<Key, Value>* pred = static_cast<AVLNode<Key, Value>*>(this->predecessor(nodeToRemove));
        this->nodeSwap(nodeToRemove, pred);
        parent = nodeToRemove->getParent();
     }
     
     // Now nodeToRemove has at most one child.
     AVLNode<Key, Value>* child = (nodeToRemove->getLeft() != nullptr) ? nodeToRemove->getLeft() : nodeToRemove->getRight();
     if (child != nullptr){
        child->setParent(parent);
     }
     
     if (parent == nullptr) {
        // Removing the root.
        this->root_ = child;
     }
     else if (nodeToRemove == parent->getLeft()) {
         parent->setLeft(child);
         parent->updateBalance(1);  // Removal from left increases parent's balance.
     }
     else {
         parent->setRight(child);
         parent->updateBalance(-1); // Removal from right decreases parent's balance.
     }
     
     delete nodeToRemove;
     
     // Propagate balance updates upward.
     AVLNode<Key, Value>* current = parent;
     while (current != nullptr) {
         int bal = current->getBalance();
         if (bal == 2 || bal == -2)
             rebalance(current);
         
         // If after rebalancing the balance factor is non-zero, stop propagation.
         if (current->getBalance() != 0)
             break;
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
void AVLTree<Key, Value>::nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2)
{
    BinarySearchTree<Key, Value>::nodeSwap(n1, n2);
    int8_t tempB = n1->getBalance();
    n1->setBalance(n2->getBalance());
    n2->setBalance(tempB);
}


#endif
