/// --------------------------------------------------------------------------
/// @file bst-map.cpp
/// @author Aidan Straker (aidan.straker@digipen.edu)
/// @brief 
/// @version 0.1
/// @date 2024-02-25
/// 
/// @copyright Copyright (c) 2024
/// 
///---------------------------------------------------------------------------
#include "bst-map.h"

// static data members
template< typename KEY_TYPE, typename VALUE_TYPE >
typename CS280::BSTmap<KEY_TYPE,VALUE_TYPE>::BSTmap_iterator        
		CS280::BSTmap<KEY_TYPE,VALUE_TYPE>::end_it        = CS280::BSTmap<KEY_TYPE,VALUE_TYPE>::BSTmap_iterator(nullptr);

template< typename KEY_TYPE, typename VALUE_TYPE >
typename CS280::BSTmap<KEY_TYPE,VALUE_TYPE>::BSTmap_iterator_const  
		CS280::BSTmap<KEY_TYPE,VALUE_TYPE>::const_end_it  = CS280::BSTmap<KEY_TYPE,VALUE_TYPE>::BSTmap_iterator_const(nullptr);

////////////////////////////////////////////////////////////
// typename is used to help compiler to parse
template< typename KEY_TYPE, typename VALUE_TYPE >
typename CS280::BSTmap<KEY_TYPE,VALUE_TYPE>::BSTmap_iterator CS280::BSTmap<KEY_TYPE,VALUE_TYPE>::begin()
{
	if (pRoot)
	{
		return BSTmap<KEY_TYPE,VALUE_TYPE>::BSTmap_iterator(pRoot->first());
	}
	else
	{
		return end_it;
	}
}

//-------------------------------------------------------------------------------------------------------------------------
/// @brief Create an iterator to the last node in the tree.
/// @tparam KEY_TYPE   - The data type of the key.
/// @tparam VALUE_TYPE - The data type of the value.
/// @return Iterator to the last node in the tree (BSTmap_iterator).
//-------------------------------------------------------------------------------------------------------------------------
template <typename KEY_TYPE, typename VALUE_TYPE>
typename CS280::BSTmap<KEY_TYPE,VALUE_TYPE>::BSTmap_iterator CS280::BSTmap<KEY_TYPE,  VALUE_TYPE>::end()
{
	return end_it;
}

//-------------------------------------------------------------------------------------------------------------------------
/// @brief Set an iterator to point at the node at the key.
/// @tparam KEY_TYPE 	 - The data type of the key
/// @tparam VALUE_TYPE - The data type of the value.
/// @param key 				 - The key to search for.
/// @return An iterator to the node at the key (BSTmap_iterator).
//-------------------------------------------------------------------------------------------------------------------------
template <typename KEY_TYPE, typename VALUE_TYPE>
typename CS280::BSTmap<KEY_TYPE,VALUE_TYPE>::BSTmap_iterator CS280::BSTmap<KEY_TYPE, VALUE_TYPE>::find(KEY_TYPE const &key)
{
	Node* temp = pRoot;

	// Traverse through the tree.
	while (temp)
	{
		// If the key is lesser, go left
		if (key < temp->Key())
		{
			temp = temp->left;
		}
		// If key is greater, go right
		else if (key > temp->Key())
		{
			temp = temp->right;
		}
		// If the key matches return an iterator to the node.
		else
		{
			return BSTmap<KEY_TYPE,VALUE_TYPE>::BSTmap_iterator(temp);
		}
	}

	// If the tree is empty return null.
	return end_it;
}

//-------------------------------------------------------------------------------------------------------------------------
/// @brief Erases a node in the tree.
/// @tparam KEY_TYPE 	 - The data type of the key
/// @tparam VALUE_TYPE - The data type of the value.
/// @param it 				 - Iterator to the node we want to erase.
//-------------------------------------------------------------------------------------------------------------------------
template <typename KEY_TYPE, typename VALUE_TYPE>
void CS280::BSTmap<KEY_TYPE, VALUE_TYPE>::erase(BSTmap_iterator it)
{
	// If the iterator is at the end.
	if (it == end_it)
	{
		return;
	}
	
	DeleteNode(it.p_node);
}

//-------------------------------------------------------------------------------------------------------------------------
/// @brief Const Iterator to the first node in the tree.
/// @tparam KEY_TYPE   - The data type of the key
/// @tparam VALUE_TYPE - The data type of the value.
/// @return Const iterator to the first node in the tree (BSTmap_iterator_const).
//-------------------------------------------------------------------------------------------------------------------------
template <typename KEY_TYPE, typename VALUE_TYPE>
typename CS280::BSTmap<KEY_TYPE,VALUE_TYPE>::BSTmap_iterator_const CS280::BSTmap<KEY_TYPE, VALUE_TYPE>::begin() const
{
	if (pRoot)
	{
		return BSTmap<KEY_TYPE,VALUE_TYPE>::BSTmap_iterator_const(pRoot->first());
	}
	else
	{
		return end_it;
	}
}

//-------------------------------------------------------------------------------------------------------------------------
/// @brief Const Iterator to the last node in the tree.
/// @tparam KEY_TYPE   - The data type of the key
/// @tparam VALUE_TYPE - The data type of the value
/// @return A const iterator to the last node in the tree (BSTmap_iterator_const).
//-------------------------------------------------------------------------------------------------------------------------
template <typename KEY_TYPE, typename VALUE_TYPE>
typename CS280::BSTmap<KEY_TYPE,VALUE_TYPE>::BSTmap_iterator_const CS280::BSTmap<KEY_TYPE, VALUE_TYPE>::end() const
{
	if (pRoot)
	{
		return BSTmap<KEY_TYPE,VALUE_TYPE>::BSTmap_iterator_const(pRoot->last());
	}
	else
	{
		return end_it;
	}
}

//-------------------------------------------------------------------------------------------------------------------------
/// @brief Constant iterator to the node at the specified key
/// @tparam KEY_TYPE 	 - The data type of the key
/// @tparam VALUE_TYPE - The data type of the value.
/// @param key 				 - The key to search for.
/// @return A constant iterator to the node at the key (BSTmap_iterator_const).
//-------------------------------------------------------------------------------------------------------------------------
template <typename KEY_TYPE, typename VALUE_TYPE>
typename CS280::BSTmap<KEY_TYPE,VALUE_TYPE>::BSTmap_iterator_const CS280::BSTmap<KEY_TYPE, VALUE_TYPE>::find(KEY_TYPE const &key) const
{
	Node* temp = pRoot;

	// Traverse through the tree.
	while (temp)
	{
		// If the key is less, go left
		if (key < temp->Key())
		{
			temp = temp->left;
		}
		// If key is greater, go right
		else if (key > temp->Key())
		{
			temp = temp->right;
		}
		// If the key matches return an iterator to the node.
		else
		{
			return BSTmap<KEY_TYPE,VALUE_TYPE>::BSTmap_iterator_const(temp);
		}
	}

	// If the tree is empty return null.
	return end_it;
}

//-------------------------------------------------------------------------------------------------------------------------
/// @brief Default Constructor.
/// @tparam KEY_TYPE - The type of key
/// @tparam VALUE_TYPE - the type of data in the node.
/// @param k 	 - The key
/// @param val - The value
/// @param p   - The node itself.
/// @param h 	 - The height of the node.
/// @param b 	 - the balance of the node.
/// @param l   - Pointer to the Node to the left of this one.
/// @param r 	 - Pointer to the Node to the right of this one.
//-------------------------------------------------------------------------------------------------------------------------
template<typename KEY_TYPE, typename VALUE_TYPE>
CS280::BSTmap<KEY_TYPE, VALUE_TYPE>::Node::Node(KEY_TYPE k, VALUE_TYPE val, Node * p, int h, int b, Node * l, Node * r)
	: key(k)
	, value(val)
	, height(h)
	, balance(b)
	, parent(p)
	, left(l)
	, right(r)
{}

//-------------------------------------------------------------------------------------------------------------------------
/// @brief Get the key of a node.
/// @tparam KEY_TYPE 	 - The data type of the key
/// @tparam VALUE_TYPE - The data type of the value.
/// @return The key of the node (KEY_TYPE)
//-------------------------------------------------------------------------------------------------------------------------
template <typename KEY_TYPE, typename VALUE_TYPE>
KEY_TYPE const &CS280::BSTmap<KEY_TYPE, VALUE_TYPE>::Node::Key() const { return key; }

//-------------------------------------------------------------------------------------------------------------------------
/// @brief Get the data of the node.
/// @tparam KEY_TYPE   - The data type of the key
/// @tparam VALUE_TYPE - The data type of the value.
/// @return The data in the node (VALUE_TYPE).
//-------------------------------------------------------------------------------------------------------------------------
template <typename KEY_TYPE, typename VALUE_TYPE>
VALUE_TYPE &CS280::BSTmap<KEY_TYPE, VALUE_TYPE>::Node::Value() { return value; }

//-------------------------------------------------------------------------------------------------------------------------
/// @brief Get the value in the node
/// @tparam KEY_TYPE   - The data type of the key
/// @tparam VALUE_TYPE - The data type of the value
/// @return Const reference to the value in the node (VALUE_TYPE const&).
//-------------------------------------------------------------------------------------------------------------------------
template <typename KEY_TYPE, typename VALUE_TYPE>
VALUE_TYPE const &CS280::BSTmap<KEY_TYPE, VALUE_TYPE>::Node::Value() const { return value; }

//-------------------------------------------------------------------------------------------------------------------------
/// @brief Get the left-most node.
/// @tparam KEY_TYPE   - The data type of the key.
/// @tparam VALUE_TYPE - The data type of the value in the node.
/// @return Pointer to the left-most node (Node*).
//-------------------------------------------------------------------------------------------------------------------------
template <typename KEY_TYPE, typename VALUE_TYPE>
typename CS280::BSTmap<KEY_TYPE, VALUE_TYPE>::Node *CS280::BSTmap<KEY_TYPE, VALUE_TYPE>::Node::first()
{
	Node* temp = this;

	// Search through the list.
	while (temp && temp->left != nullptr)
	{
		temp = temp->left;
	}
	
  return temp;
}

//-------------------------------------------------------------------------------------------------------------------------
/// @brief Get the right-most node.
/// @tparam KEY_TYPE   - The data type of the key
/// @tparam VALUE_TYPE - The data type of the data.
/// @return The right-most node (Node*).
//-------------------------------------------------------------------------------------------------------------------------
template <typename KEY_TYPE, typename VALUE_TYPE>
typename CS280::BSTmap<KEY_TYPE, VALUE_TYPE>::Node *CS280::BSTmap<KEY_TYPE, VALUE_TYPE>::Node::last()
{
	Node* temp = this; 

	// Search through the list.
	while (temp && temp->right != nullptr)
	{
		temp = temp->right;
	}
	
  return temp;
}

//-------------------------------------------------------------------------------------------------------------------------
/// @brief Find the successor.
/// @tparam KEY_TYPE - The data type of the key.
/// @tparam VALUE_TYPE - The data type of the value.
/// @return This node's immediate successor (Node*).
//-------------------------------------------------------------------------------------------------------------------------
template <typename KEY_TYPE, typename VALUE_TYPE>
typename CS280::BSTmap<KEY_TYPE, VALUE_TYPE>::Node *CS280::BSTmap<KEY_TYPE, VALUE_TYPE>::Node::increment()
{
	// Temp node for traversing the search tree.
	Node* current = nullptr;

	// Go to the right node.
	if (this->right != nullptr)
	{
		current = this->right;
		// Return the last node.
		return current->first();
	}
	current = this;

	// Find a node that has no siblings.
	while (current->parent && current->parent->right == current)
	{
		current = current->parent;
	}
	
  return current->parent;
}

//-------------------------------------------------------------------------------------------------------------------------
/// @brief Find the predecessor
/// @tparam KEY_TYPE   - The data type of the key
/// @tparam VALUE_TYPE - The data type of the value.
/// @return This node's immediate predecessor (Node*).
//-------------------------------------------------------------------------------------------------------------------------
template <typename KEY_TYPE, typename VALUE_TYPE>
typename CS280::BSTmap<KEY_TYPE, VALUE_TYPE>::Node *CS280::BSTmap<KEY_TYPE, VALUE_TYPE>::Node::decrement()
{
	// Temp node for traversing the search tree.
	Node* current = nullptr;

	// Go to the right node.
	if (left != nullptr)
	{
		current = left;
		// Return the last node.
		return current->last();
	}
	current = this;

	// Find a node that has no siblings.
	while (current->parent && current->parent->left == current)
	{
		current = current->parent;
	}
	
  return current->parent;
}

//-------------------------------------------------------------------------------------------------------------------------
/// @brief Prints the key and data stored in the node.
/// @tparam KEY_TYPE   - The data type of the key.
/// @tparam VALUE_TYPE - The data type of value.
/// @param os 				 - Output stream
//-------------------------------------------------------------------------------------------------------------------------
template<typename KEY_TYPE, typename VALUE_TYPE>
void CS280::BSTmap<KEY_TYPE, VALUE_TYPE>::Node::print(std::ostream& os) const{}

//-------------------------------------------------------------------------------------------------------------------------
/// @brief Finds a node's parent's left or right.
/// @tparam KEY_TYPE - The data type of the key
/// @tparam VALUE_TYPE - The data type of the value.
/// @return A pointer to the node.
//-------------------------------------------------------------------------------------------------------------------------
template <typename KEY_TYPE, typename VALUE_TYPE>
typename CS280::BSTmap<KEY_TYPE, VALUE_TYPE>::Node *&CS280::BSTmap<KEY_TYPE, VALUE_TYPE>::Node::FindNode()
{
	// Check if the node has any family.
  if (this->parent->right == this)
	{
		return this->parent->right;
	}
	else
	{
		return this->parent->left;
	}
}



//-------------------------------------------------------------------------------------------------------------------------
/// @brief Assignment Operator overload
/// @tparam KEY_TYPE 	 - The data type of the key
/// @tparam VALUE_TYPE - The data type of the value.
/// @param rhs 				 - A reference to another iterator.
/// @return A reference to BSTmap iterator.
//-------------------------------------------------------------------------------------------------------------------------
template <typename KEY_TYPE, typename VALUE_TYPE>
typename CS280::BSTmap<KEY_TYPE, VALUE_TYPE>::BSTmap_iterator& CS280::BSTmap<KEY_TYPE, VALUE_TYPE>::BSTmap_iterator::operator=(const CS280::BSTmap<KEY_TYPE, VALUE_TYPE>::BSTmap_iterator &rhs)
{
	// Copy the data from the other iterator.
	p_node = rhs.p_node;
	return *this;
}

//-------------------------------------------------------------------------------------------------------------------------
/// @brief Prefix Operator Overload
/// @tparam KEY_TYPE - The data type of the key
/// @tparam VALUE_TYPE - The data type of the value.
/// @return A reference to a BSTmap_iterator.
//-------------------------------------------------------------------------------------------------------------------------
template <typename KEY_TYPE, typename VALUE_TYPE>
typename CS280::BSTmap<KEY_TYPE, VALUE_TYPE>::BSTmap_iterator& CS280::BSTmap<KEY_TYPE, VALUE_TYPE>::BSTmap_iterator::operator++()
{
  p_node = p_node->increment();
	return *this;
}

//-------------------------------------------------------------------------------------------------------------------------
/// @brief Postfix Operator Overload
/// @tparam KEY_TYPE 	 - The data type of the key
/// @tparam VALUE_TYPE - The data type of the value.
/// @param - Marks this as postfix.
/// @return A BSTmap_iterator.
//-------------------------------------------------------------------------------------------------------------------------
template <typename KEY_TYPE, typename VALUE_TYPE>
typename CS280::BSTmap<KEY_TYPE, VALUE_TYPE>::BSTmap_iterator CS280::BSTmap<KEY_TYPE, VALUE_TYPE>::BSTmap_iterator::operator++(int)
{
	// Make a copy of the iterator.
	BSTmap_iterator iter(p_node);
	// Increment this p_node.
	++(*this);
	// Return the copy.
  return iter;
}

//-------------------------------------------------------------------------------------------------------------------------
/// @brief Dereference Operator Overload
/// @tparam KEY_TYPE   - The data type of the key
/// @tparam VALUE_TYPE - The data type of the key
/// @return A BSTmap node (Node).
//-------------------------------------------------------------------------------------------------------------------------
template <typename KEY_TYPE, typename VALUE_TYPE>
typename CS280::BSTmap<KEY_TYPE, VALUE_TYPE>::Node &CS280::BSTmap<KEY_TYPE, VALUE_TYPE>::BSTmap_iterator::operator*()
{
  return *p_node;
}

//-------------------------------------------------------------------------------------------------------------------------
/// @brief Arrow Operator Overload
/// @tparam KEY_TYPE 	 - The data type of the key
/// @tparam VALUE_TYPE - The data type of the value.
/// @return Pointer to the node the iterator is at (Node*).
//-------------------------------------------------------------------------------------------------------------------------
template <typename KEY_TYPE, typename VALUE_TYPE>
typename CS280::BSTmap<KEY_TYPE, VALUE_TYPE>::Node *CS280::BSTmap<KEY_TYPE, VALUE_TYPE>::BSTmap_iterator::operator->()
{
  return p_node;
}

//-------------------------------------------------------------------------------------------------------------------------
/// @brief Not Equal To Operator overload
/// @tparam KEY_TYPE 	 - The data type of the key
/// @tparam VALUE_TYPE - The data type of the value.
/// @param rhs 				 - The iterator containing the node to check.
/// @return The result of the comparison (bool).
//-------------------------------------------------------------------------------------------------------------------------
template <typename KEY_TYPE, typename VALUE_TYPE>
bool CS280::BSTmap<KEY_TYPE, VALUE_TYPE>::BSTmap_iterator::operator!=(const BSTmap_iterator &rhs)
{
  return (p_node != rhs.p_node);
}

//-------------------------------------------------------------------------------------------------------------------------
/// @brief Equals To Operator overload
/// @tparam KEY_TYPE 	 - The data type of the key
/// @tparam VALUE_TYPE - The data type of the value.
/// @param rhs 				 - The iterator containing the node to check.
/// @return The result of the comparison (bool).
//-------------------------------------------------------------------------------------------------------------------------
template <typename KEY_TYPE, typename VALUE_TYPE>
bool CS280::BSTmap<KEY_TYPE, VALUE_TYPE>::BSTmap_iterator::operator==(const BSTmap_iterator &rhs)
{
  return (p_node == rhs.p_node);
}

//-------------------------------------------------------------------------------------------------------------------------
/// @brief Constructor for the const BSTmap iterator.
/// @tparam KEY_TYPE 	 - The data type of the key.
/// @tparam VALUE_TYPE - The data type of the value.
/// @param p 					 - The node the iterator points too.
//-------------------------------------------------------------------------------------------------------------------------
template<typename KEY_TYPE, typename VALUE_TYPE>
CS280::BSTmap<KEY_TYPE, VALUE_TYPE>::BSTmap_iterator_const::BSTmap_iterator_const(Node * p)
	: p_node(p)
{}

//-------------------------------------------------------------------------------------------------------------------------
/// @brief Assignment Operator overload for const iterators
/// @tparam KEY_TYPE 	 - The data type of the key
/// @tparam VALUE_TYPE - The data type of the value.
/// @param rhs 				 - A reference to another iterator.
/// @return A reference to BSTmap iterator.
//-------------------------------------------------------------------------------------------------------------------------
template <typename KEY_TYPE, typename VALUE_TYPE>
typename CS280::BSTmap<KEY_TYPE, VALUE_TYPE>::BSTmap_iterator_const &CS280::BSTmap<KEY_TYPE, VALUE_TYPE>::BSTmap_iterator_const::operator=(const BSTmap_iterator_const &rhs)
{
  // Copy the data from the other iterator.
	p_node = rhs.p_node;
	return *this;
}

//-------------------------------------------------------------------------------------------------------------------------
/// @brief Prefix Operator Overload for const iterators
/// @tparam KEY_TYPE - The data type of the key
/// @tparam VALUE_TYPE - The data type of the value.
/// @return A reference to a BSTmap_iterator.
//-------------------------------------------------------------------------------------------------------------------------
template <typename KEY_TYPE, typename VALUE_TYPE>
typename CS280::BSTmap<KEY_TYPE, VALUE_TYPE>::BSTmap_iterator_const &CS280::BSTmap<KEY_TYPE, VALUE_TYPE>::BSTmap_iterator_const::operator++()
{
  p_node = p_node->increment();
	return *this;
}

//-------------------------------------------------------------------------------------------------------------------------
/// @brief Postfix Operator Overload for const iterators
/// @tparam KEY_TYPE 	 - The data type of the key
/// @tparam VALUE_TYPE - The data type of the value.
/// @param - Marks this as postfix.
/// @return A BSTmap_iterator.
//-------------------------------------------------------------------------------------------------------------------------
template <typename KEY_TYPE, typename VALUE_TYPE>
typename CS280::BSTmap<KEY_TYPE, VALUE_TYPE>::BSTmap_iterator_const CS280::BSTmap<KEY_TYPE, VALUE_TYPE>::BSTmap_iterator_const::operator++(int)
{
	BSTmap_iterator_const copy(p_node);
	// Increment p_node.
	++(*this);
  return copy;
}

//-------------------------------------------------------------------------------------------------------------------------
/// @brief Dereference Operator Overload for const iterators
/// @tparam KEY_TYPE   - The data type of the key
/// @tparam VALUE_TYPE - The data type of the key
/// @return A BSTmap node (Node).
//-------------------------------------------------------------------------------------------------------------------------
template <typename KEY_TYPE, typename VALUE_TYPE>
typename CS280::BSTmap<KEY_TYPE, VALUE_TYPE>::Node const &CS280::BSTmap<KEY_TYPE, VALUE_TYPE>::BSTmap_iterator_const::operator*()
{
  return *p_node;
}

//-------------------------------------------------------------------------------------------------------------------------
/// @brief Arrow Operator Overload for const iterators
/// @tparam KEY_TYPE 	 - The data type of the key
/// @tparam VALUE_TYPE - The data type of the value.
/// @return Pointer to the node the iterator is at (Node*).
//-------------------------------------------------------------------------------------------------------------------------
template <typename KEY_TYPE, typename VALUE_TYPE>
typename CS280::BSTmap<KEY_TYPE, VALUE_TYPE>::Node const *CS280::BSTmap<KEY_TYPE, VALUE_TYPE>::BSTmap_iterator_const::operator->()
{
  return p_node;
}

//-------------------------------------------------------------------------------------------------------------------------
/// @brief Not Equal To Operator overload for const iterators
/// @tparam KEY_TYPE 	 - The data type of the key
/// @tparam VALUE_TYPE - The data type of the value.
/// @param rhs 				 - The iterator containing the node to check.
/// @return The result of the comparison (bool).
//-------------------------------------------------------------------------------------------------------------------------
template <typename KEY_TYPE, typename VALUE_TYPE>
bool CS280::BSTmap<KEY_TYPE, VALUE_TYPE>::BSTmap_iterator_const::operator!=(const BSTmap_iterator_const &rhs)
{
  return (p_node != rhs.p_node);
}

//-------------------------------------------------------------------------------------------------------------------------
/// @brief Equals To Operator overload for const iterators.
/// @tparam KEY_TYPE 	 - The data type of the key
/// @tparam VALUE_TYPE - The data type of the value.
/// @param rhs 				 - The iterator containing the node to check.
/// @return The result of the comparison (bool).
//-------------------------------------------------------------------------------------------------------------------------
template <typename KEY_TYPE, typename VALUE_TYPE>
bool CS280::BSTmap<KEY_TYPE, VALUE_TYPE>::BSTmap_iterator_const::operator==(const BSTmap_iterator_const &rhs)
{
  return (p_node == rhs.p_node);
}

//-------------------------------------------------------------------------------------------------------------------------
/// @brief Constructor for the BSTmap
/// @tparam KEY_TYPE 	 - The data type of the key
/// @tparam VALUE_TYPE - The data type of the value.
//-------------------------------------------------------------------------------------------------------------------------
template<typename KEY_TYPE, typename VALUE_TYPE>
CS280::BSTmap<KEY_TYPE, VALUE_TYPE>::BSTmap(){}

//-------------------------------------------------------------------------------------------------------------------------
/// @brief Get the height of a node.
/// @tparam KEY_TYPE - The data type of the key
/// @tparam VALUE_TYPE - The data type of the value.
/// @param node - The node whose height we want to calculate.
/// @return The height of the node (int).
//-------------------------------------------------------------------------------------------------------------------------
template <typename KEY_TYPE, typename VALUE_TYPE>
int CS280::BSTmap<KEY_TYPE, VALUE_TYPE>::getdepth(const Node *node) const
{
	int depth = 0;
	while (node->parent)
	{
		depth++;
		node = node->parent;
	}
	
  return depth;
}
//-------------------------------------------------------------------------------------------------------------------------
/// @brief Copies the tree
/// @tparam KEY_TYPE 	 - The data type of the key
/// @tparam VALUE_TYPE - The data type of the value.
/// @param node 			 - Pointer to the node to copy from.
/// @param newNode 		 - Pointer to the reference of the node to copy too.
/// @param parent 		 - Pointer to the parent node.
//-------------------------------------------------------------------------------------------------------------------------
template <typename KEY_TYPE, typename VALUE_TYPE>
void CS280::BSTmap<KEY_TYPE, VALUE_TYPE>::CopyTree(const Node *node, Node *&newNode, Node *parent)
{
	// If the node is null, set the other node to null.
	if (node == nullptr)
	{
		newNode = nullptr;
		return;
	}

	// Initialise the new node by copying the current node.
	newNode = new Node(node->Key(), node->Value(), parent);
	// Recursively copy the other nodes.
	CopyTree(node->left, newNode->left, parent);
	CopyTree(node->right, newNode->right, parent);
}

//-------------------------------------------------------------------------------------------------------------------------
/// @brief Deletes the BST
/// @tparam KEY_TYPE - The data type of the key.
/// @tparam VALUE_TYPE - The data type of the value.
/// @param tree - The BST to delete.
//-------------------------------------------------------------------------------------------------------------------------
template <typename KEY_TYPE, typename VALUE_TYPE>
void CS280::BSTmap<KEY_TYPE, VALUE_TYPE>::DeleteTree(Node *tree)
{
	// Prevent double free.
	if (tree == nullptr)
	{
		return;
	}

	// Delete the left and right subtrees.
	DeleteTree(tree->left);
	DeleteTree(tree->right);
	// Delete the node.
	delete tree;
}

//-------------------------------------------------------------------------------------------------------------------------
/// @brief Deletes a node from the tree
/// @tparam KEY_TYPE - The data type of the key.
/// @tparam VALUE_TYPE - The data type of the value.
/// @param node - The node to be deleted.
//-------------------------------------------------------------------------------------------------------------------------
template <typename KEY_TYPE, typename VALUE_TYPE>
void CS280::BSTmap<KEY_TYPE, VALUE_TYPE>::DeleteNode(Node *node)
{
	// Base Case: No left or right
	if (!node->right && !node->left)
	{
		// If the node is not pRoot
		if (node != pRoot)
		{
			// Find the address of the node.
			Node*& newNode = node->FindNode();
			newNode = nullptr;
		}
		else
		{
			pRoot = nullptr;
		}
		delete node;
		size_--;
	}
	// Case: left and right
	else if(node->right && node->left)
	{
		// Get this node's successor
		Node* successor = node->decrement();
		// Copy the key and value from the successor into the current node.
		node->key = successor->Key();
		node->value = successor->Value();
		// Delete the successor.
		DeleteNode(successor);
	}
	else
	{
		// The child is determined by the node having a left or right.
		Node* child = (node->left) ? node->left : node->right;

		// If the node is not the pRoot
		if (node != pRoot)
		{
			// Get the address of the node.
			Node*& newNode = node->FindNode();
			// Set the child to that address.
			newNode = child;
			// Update the parents
			child->parent = node->parent;
		}
		else
		{
			// Set pRoot to the child.
			pRoot = child;
		}
		// Delete the node.
		delete node;
		// Decrement the amount of nodes in the tree.
		size_--;
	}
}

//-------------------------------------------------------------------------------------------------------------------------
/// @brief Inserts a node into the tree at a specified key
/// @tparam KEY_TYPE   - The data type of the key.
/// @tparam VALUE_TYPE - The data type of the value.
/// @param insertNode  - The node to be inserted
/// @param key 				 - The position in the tree to insert at.
/// @param parent 		 - The parent of the node we are inserting.
//-------------------------------------------------------------------------------------------------------------------------
template <typename KEY_TYPE, typename VALUE_TYPE>
typename CS280::BSTmap<KEY_TYPE, VALUE_TYPE>::Node* CS280::BSTmap<KEY_TYPE, VALUE_TYPE>::InsertNode(Node *&insertNode, const KEY_TYPE &key, Node *parent)
{
	// Base case: insertNode is null.
  if (insertNode == nullptr)
	{
		insertNode = new Node(key, VALUE_TYPE(), parent);
		++size_;
		return insertNode;
	}

	// Key is smaller than the node's key 
	if (key < insertNode->Key())
	{
		return InsertNode(insertNode->left, key, insertNode);
	}
	// Key is greater than the node's key
	else if (key > insertNode->Key())
	{
		return InsertNode(insertNode->right, key, insertNode);
	}
	// If the key is the same.
	else
	{
		return insertNode;
	}
}

//-------------------------------------------------------------------------------------------------------------------------
/// @brief Copy Constructor for the BSTmap
/// @tparam KEY_TYPE 	 - The data type of the key
/// @tparam VALUE_TYPE - The data type of the value.
//-------------------------------------------------------------------------------------------------------------------------
template <typename KEY_TYPE, typename VALUE_TYPE>
CS280::BSTmap<KEY_TYPE, VALUE_TYPE>::BSTmap(const BSTmap &rhs)
	: pRoot(nullptr)
	, size_(rhs.size_)
{
	// As long as the other tree is not empty
	if (rhs.pRoot != nullptr)
	{
		CopyTree(rhs.pRoot, pRoot, nullptr);
	}
}

//-------------------------------------------------------------------------------------------------------------------------
/// @brief Copy Constructor for the BSTmap
/// @tparam KEY_TYPE 	 - The data type of the key
/// @tparam VALUE_TYPE - The data type of the value.
//-------------------------------------------------------------------------------------------------------------------------
template <typename KEY_TYPE, typename VALUE_TYPE>
CS280::BSTmap<KEY_TYPE, VALUE_TYPE> &CS280::BSTmap<KEY_TYPE, VALUE_TYPE>::operator=(const BSTmap<KEY_TYPE, VALUE_TYPE> &rhs)
{
	// Check for self assingment
  if (this == &rhs)
	{
		return *this;
	}

	// Delete the current tree.
	DeleteTree(pRoot);

	// Reinitialise the tree
	size_ = 0;
	pRoot = nullptr;

	// Copy the tree.
	if (rhs.pRoot != nullptr)
	{
		CopyTree(rhs.pRoot, pRoot, nullptr);
	}

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------------
/// @brief Move Constructor for the BSTmap
/// @tparam KEY_TYPE 	 - The data type of the key
/// @tparam VALUE_TYPE - The data type of the value.
/// @param rhs 			   - rvalue reference to another BSTmap
//-------------------------------------------------------------------------------------------------------------------------
template <typename KEY_TYPE, typename VALUE_TYPE>
CS280::BSTmap<KEY_TYPE, VALUE_TYPE>::BSTmap(BSTmap &&rhs)
{
	// Take resources from 'rhs' and make them our own 
	this->size_ = rhs.size_;
	this->pRoot = rhs.pRoot;

	// Reset 'rhs' to a valid but unspecified state.
	rhs.size_ = 0;
	rhs.pRoot = nullptr;
}

//-------------------------------------------------------------------------------------------------------------------------
/// @brief Move Assignment Operator Overload
/// @tparam KEY_TYPE 	 - The data type of the key.
/// @tparam VALUE_TYPE - The data type of the value.
/// @param rhs 				 - Rvalue reference to another BSTmap to assign to this one.
/// @return A reference to the newly assigned BSTmap (BSTmap&).
//-------------------------------------------------------------------------------------------------------------------------
template <typename KEY_TYPE, typename VALUE_TYPE>
typename CS280::BSTmap<KEY_TYPE,VALUE_TYPE>::BSTmap &CS280::BSTmap<KEY_TYPE, VALUE_TYPE>::operator=(BSTmap &&rhs)
{
	// Self assignment check
  if (this == &rhs)
	{
		return *this;
	}
	
	// Release current resources.
	DeleteTree(pRoot);

	// Take resources from 'rhs' and make them our own 
	this->size_ = rhs.size_;
	this->pRoot = rhs.pRoot;

	// Reset 'rhs' to a valid but unspecified state.
	rhs.size_ = 0;
	rhs.pRoot = nullptr;

	return *this;
}

//-------------------------------------------------------------------------------------------------------------------------
/// @brief BSTmap Destructor
/// @tparam KEY_TYPE 	 - The data type of the key
/// @tparam VALUE_TYPE - The data type of the value
//-------------------------------------------------------------------------------------------------------------------------
template <typename KEY_TYPE, typename VALUE_TYPE>
CS280::BSTmap<KEY_TYPE, VALUE_TYPE>::~BSTmap() {DeleteTree(pRoot);}

//-------------------------------------------------------------------------------------------------------------------------
/// @brief The amount of ndoes in the BSTmap
/// @tparam KEY_TYPE - The data type of the key
/// @tparam VALUE_TYPE - The data type of the value
/// @return The amount of nodes in BSTmap (unsigned int).
//-------------------------------------------------------------------------------------------------------------------------
template <typename KEY_TYPE, typename VALUE_TYPE>
unsigned int CS280::BSTmap<KEY_TYPE, VALUE_TYPE>::size() {return size_;}

//-------------------------------------------------------------------------------------------------------------------------
/// @brief Index Operator Overload
/// @tparam KEY_TYPE 	 - The data type of the key
/// @tparam VALUE_TYPE - the data type of thr value
/// @param key 				 - The key to find.
/// @return The value at the key (VALUE_TYPE).
//-------------------------------------------------------------------------------------------------------------------------
template <typename KEY_TYPE, typename VALUE_TYPE>
VALUE_TYPE &CS280::BSTmap<KEY_TYPE, VALUE_TYPE>::operator[](KEY_TYPE const &key)
{
	// Base case: tree is empty.
  if (pRoot == nullptr)
	{
		pRoot = new Node(key, VALUE_TYPE(), nullptr);
		++size_;
		return pRoot->Value();
	}

	// Search the tree.
	Node* node = InsertNode(pRoot, key, pRoot);
	return node->Value();
}

////////////////////////////////////////////////////////////
// do not change this code from here to the end of the file
/* figure out whether node is left or right child or root 
 * used in print_backwards_padded 
 */
template< typename KEY_TYPE, typename VALUE_TYPE >
char CS280::BSTmap<KEY_TYPE,VALUE_TYPE>::getedgesymbol(const Node* node) const
{
	const Node* parent = node->parent;
	if ( parent == nullptr) return '-';
	else return ( parent->left == node)?'\\':'/';
}

/* this is another "ASCII-graphical" print, but using 
 * iterative function. 
 * Left branch of the tree is at the bottom
 */
template< typename KEY_TYPE, typename VALUE_TYPE >
std::ostream& CS280::operator<<(std::ostream& os, BSTmap<KEY_TYPE,VALUE_TYPE> const& map) {
	map.print(os);
	return os;
}

template< typename KEY_TYPE, typename VALUE_TYPE >
void CS280::BSTmap<KEY_TYPE,VALUE_TYPE>::print(std::ostream& os, bool print_value ) const {
	if (pRoot) 
	{
		BSTmap<KEY_TYPE,VALUE_TYPE>::Node* b = pRoot->last();
		while ( b ) { 
			int depth = getdepth(b);
			int i;
			/* printf(b); */

			char edge = getedgesymbol(b);
			switch (edge) {
				case '-': 
					for (i=0; i<depth; ++i) std::printf("       ");
					os << b->key;
                    if ( print_value ) { os << " -> " << b->value; }
                    os << std::endl;
					break;
				case '\\': 
					for (i=0; i<depth; ++i) std::printf("       ");
					os << edge << std::endl;
					for (i=0; i<depth; ++i) std::printf("       ");
					os << b->key;
                    if ( print_value ) { os << " -> " << b->value; }
                    os << std::endl;
					break;
				case '/': 
					for (i=0; i<depth; ++i) std::printf("       ");
					os << b->key;
                    if ( print_value ) { os << " -> " << b->value; }
                    os << std::endl;
					for (i=0; i<depth; ++i) std::printf("       ");
					os << edge << std::endl;
					break;
			}
			b = b->decrement();
		}
	}
	std::printf("\n");
}
