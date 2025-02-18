/// --------------------------------------------------------------------------
/// @file avl-map.cpp
/// @author Aidan Straker (aidan.straker@digipen.edu)
/// @brief Implementation of AVL based on inheriting from BST
/// @version 0.1
/// @date 2024-03-22
/// 
/// @copyright Copyright (c) 2024
/// 
///---------------------------------------------------------------------------

#include "avl-map.h"

//-------------------------------------------------------------------------------------------------------------------------
/// @brief AVL map constructor
/// @tparam KEY_TYPE   - The data type of the key.
/// @tparam VALUE_TYPE - The data type of the value.
//-------------------------------------------------------------------------------------------------------------------------
template<typename KEY_TYPE, typename VALUE_TYPE>
CS280::AVLMAP::AVLmap()
    : BSTMAP::BSTMAP()
{}

//-------------------------------------------------------------------------------------------------------------------------
/// @brief AVLNode constructor
/// @tparam KEY_TYPE   - The type of key
/// @tparam VALUE_TYPE - the type of data in the node.
/// @param k 	       - The key
/// @param val         - The value
/// @param p           - The node itself.
/// @param h 	       - The height of the node.
/// @param b 	       - the balance of the node.
/// @param l           - Pointer to the Node to the left of this one.
/// @param r 	       - Pointer to the Node to the right of this one.
//-------------------------------------------------------------------------------------------------------------------------
template<typename KEY_TYPE, typename VALUE_TYPE>
CS280::AVLMAP::AVLNode::AVLNode(KEY_TYPE k, VALUE_TYPE val, AVLNode* p, int h, int b, AVLNode* l, AVLNode* r)
    : BSTMAP::Node::Node(k, val, p, h, b, l, r)
{}

//-------------------------------------------------------------------------------------------------------------------------
/// @brief Updates the height of an AVLNode
/// @tparam KEY_TYPE   - The data type of the key
/// @tparam VALUE_TYPE - The data type of the value
//-------------------------------------------------------------------------------------------------------------------------
template<typename KEY_TYPE, typename VALUE_TYPE>
void CS280::AVLMAP::AVLNode::AVLNodeHeight()
{
    // Add one to the height of the left node.
    int leftHeight = this->left->height + 1;
    // Add one to the height of the left node.
    int rightHeight = this->right->height + 1;

    // The height is whichever is greater.
    this->height = std::max(leftHeight, rightHeight);
}

//-------------------------------------------------------------------------------------------------------------------------
/// @brief Updates the balance of an AVLNode
/// @tparam KEY_TYPE   - The data type of the key.
/// @tparam VALUE_TYPE - The data type of the value.
//-------------------------------------------------------------------------------------------------------------------------
template <typename KEY_TYPE, typename VALUE_TYPE>
void CS280::AVLMAP::AVLNode::AVLNodeBalance()
{
    // Add one to the height of the left node.
    int leftHeight = this->left->height + 1;
    // Add one to the height of the left node.
    int rightHeight = this->right->height + 1;

    this->balance = leftHeight - rightHeight;
}

//-------------------------------------------------------------------------------------------------------------------------
/// @brief Inserts a node into the tree at a specified key
/// @tparam KEY_TYPE   - The data type of the key.
/// @tparam VALUE_TYPE - The data type of the value.
/// @param insertNode  - The node to be inserted
/// @param key 		   - The position in the tree to insert at.
/// @param parent 	   - The parent of the node we are inserting.
/// @return Pointer to a BSTMAP Node (BSTMAP::Node*).
//-------------------------------------------------------------------------------------------------------------------------
template<typename KEY_TYPE, typename VALUE_TYPE>
typename CS280::BSTMAP::Node* CS280::AVLMAP::InsertNode(typename BSTMAP::Node*& insertNode, const KEY_TYPE& key, typename BSTMAP::Node* parent)
{
	// Perform normal BST insertion
	if (insertNode == nullptr)
	{
		insertNode = new typename BSTMAP::Node(key, VALUE_TYPE(), parent);
		this->size_++;
		return insertNode;
	}

	this->m_Nodes.push_front(static_cast<AVLNode*>(insertNode));

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
/// @brief Performs left rotation on the passed in node
/// @tparam KEY_TYPE   - The data type of the key.
/// @tparam VALUE_TYPE - The data type of the value.
/// @param node        - The node to rotate.
//-------------------------------------------------------------------------------------------------------------------------
template<typename KEY_TYPE, typename VALUE_TYPE>
void CS280::AVLMAP::RotateLeft(AVLNode* node)
{
	AVLNode* temp = node;
	node = static_cast<AVLNode*>(node->right);
	temp->right = node->left;
	node->left = temp;
	
	AVLNode* copy = node;
	node->parent = temp->parent;

	if (temp->right)
	{
		temp->right->parent = temp;
	}
	
	temp->parent = node;
	
	temp->AVLNodeHeight();
	temp->AVLNodeBalance();

	copy->AVLNodeHeight();
	copy->AVLNodeBalance();

	UpdateTreeHeight(copy);
}

//-------------------------------------------------------------------------------------------------------------------------
/// @brief Performs right rotation on the passed in node
/// @tparam KEY_TYPE   - The data type of the key.
/// @tparam VALUE_TYPE - The data type of the value.
/// @param node        - The node to rotate.
//-------------------------------------------------------------------------------------------------------------------------
template<typename KEY_TYPE, typename VALUE_TYPE>
void CS280::AVLMAP::RotateRight(AVLNode* node)
{
	AVLNode* temp = node;
	node = static_cast<AVLNode*>(node->left);
	temp->left = node->right;
	node->right = temp;
	
	AVLNode* copy = node;
	node->parent = temp->parent;

	if (temp->left)
	{
		temp->left->parent = temp;
	}
	
	temp->parent = node;
	
	temp->AVLNodeHeight();
	temp->AVLNodeBalance();

	copy->AVLNodeHeight();
	copy->AVLNodeBalance();

	UpdateTreeHeight(copy);
}


//-------------------------------------------------------------------------------------------------------------------------
/// @brief Deletes a node from the tree
/// @tparam KEY_TYPE 	 - The data type of the key.
/// @tparam VALUE_TYPE - The data type of the value.
/// @param node 			 - The node to be deleted.
//-------------------------------------------------------------------------------------------------------------------------
template<typename KEY_TYPE, typename VALUE_TYPE>
void CS280::AVLMAP::DeleteNode(typename BSTMAP::Node* node)
{
	typename BSTMAP::Node* parentNode = node->parent;
	while (parentNode)
	{
		this->m_Nodes.push_front(static_cast<AVLNode*>(parentNode));
		parentNode = parentNode->parent;
	}

  // Base Case: No left or right
	if (!node->right && !node->left)
	{
		// If the node is not pRoot
		if (node != this->pRoot)
		{
			// Find the address of the node.
			typename BSTMAP::Node*& newNode = node->FindNode();
			newNode = nullptr;
		}
		else
		{
			this->pRoot = nullptr;
		}
		delete node;
		this->size_--;
	}
	// Case: left and right
	else if(node->right && node->left)
	{
		// Get this node's successor
		typename BSTMAP::Node* successor = node->decrement();
		// Copy the key and value from the successor into the current node.
		node->key = successor->Key();
		node->value = successor->Value();
		// Delete the successor.
		DeleteNode(successor);
	}
	else
	{
		// The child is determined by the node having a left or right.
		typename BSTMAP::Node* child = (node->left) ? node->left : node->right;

		// If the node is not the pRoot
		if (node != this->pRoot)
		{
			// Get the address of the node.
			typename BSTMAP::Node*& newNode = node->FindNode();
			// Set the child to that address.
			newNode = child;
			// Update the parents
			child->parent = node->parent;
		}
		else
		{
			// Set pRoot to the child.
			this->pRoot = child;
		}
		// Delete the node.
		delete node;
		// Decrement the amount of nodes in the tree.
		this->size_--;
	}

	// If the stack is not empty update all the nodes on it.
	while (this->m_Nodes.empty() == false)
	{
		// Get the node from the stack.
		AVLNode* stackNode = this->m_Nodes[0];
		this->m_Nodes.pop_front();

		// Update height and balance
		stackNode->AVLNodeHeight();
		stackNode->AVLNodeBalance();

		typename BSTMAP::Node*& r_node = stackNode->FindNode();

		// Right Right case
		if (r_node->balance > 1 && r_node->left->balance >= 0)
		{
			RotateRight(static_cast<AVLNode*>(r_node));
		}
		// Left Left case
		else if (r_node->balance < -1 && r_node->right->balance <= 0)
		{
			RotateLeft(static_cast<AVLNode*>(r_node));
		}
		// Right Left case
		else if (r_node->balance > 1 && r_node->left->balance < 0)
		{
			RotateLeft(static_cast<AVLNode*>(r_node->left));
			RotateRight(static_cast<AVLNode*>(r_node));
		}
		// Left Right case
		else if (r_node->balance < -1 && r_node->right->balance > 0)
		{
			RotateRight(static_cast<AVLNode*>(r_node->right));
			RotateLeft(static_cast<AVLNode*>(r_node));
		}
	}
}

//-------------------------------------------------------------------------------------------------------------------------
/// @brief Index Operator Overload
/// @tparam KEY_TYPE   - The data type of the key
/// @tparam VALUE_TYPE - The data type of value
/// @param key 		   - The keey to search the tree for
/// @return A reference to the value in the node with the matching key (VALUE_TYPE)
//-------------------------------------------------------------------------------------------------------------------------
template<typename KEY_TYPE, typename VALUE_TYPE>
VALUE_TYPE& CS280::AVLMAP::operator[](KEY_TYPE const & key)
{
	// Make a copy of the size
	unsigned int sizeCopy = this->size();

	// Find the node, insert a new node if the key is not found.
	VALUE_TYPE& value = InsertNode(this->pRoot, key, this->end())->Value();

	// If a new node was inserted
	if (sizeCopy == this->size() - 1)
	{
		// If the stack is not empty update all the nodes on it.
		while (this->m_Nodes.empty() == false)
		{
			// Get the node from the stack.
			AVLNode* stackNode = this->m_Nodes[0];
			this->m_Nodes.pop_front();

			// Update height and balance
			stackNode->AVLNodeHeight();
			stackNode->AVLNodeBalance();

			typename BSTMAP::Node*& r_node = stackNode->FindNode();

			// Right Right case
			if (r_node->balance > 1 && r_node->left->balance >= 0)
			{
				RotateRight(static_cast<AVLNode*>(r_node));
			}
			// Left Left case
			else if (r_node->balance < -1 && r_node->right->balance <= 0)
			{
				RotateLeft(static_cast<AVLNode*>(r_node));
			}
			// Right Left case
			else if (r_node->balance > 1 && r_node->left->balance < 0)
			{
				RotateLeft(static_cast<AVLNode*>(r_node->left));
				RotateRight(static_cast<AVLNode*>(r_node));
			}
			// Left Right case
			else if (r_node->balance < -1 && r_node->right->balance > 0)
			{
				RotateRight(static_cast<AVLNode*>(r_node->right));
				RotateLeft(static_cast<AVLNode*>(r_node));
			}
		}
	}

	// Empty the stack
	this->m_Nodes.clear();
	return value;
}

//-------------------------------------------------------------------------------------------------------------------------
/// @brief Update the height of a sub-tree
/// @tparam KEY_TYPE   - The data type of the key
/// @tparam VALUE_TYPE - the data type of the value
/// @param tree 	   - the node to start updating from
//-------------------------------------------------------------------------------------------------------------------------
template<typename KEY_TYPE, typename VALUE_TYPE>
void CS280::AVLMAP::UpdateTreeHeight(AVLNode* tree)
{
	while (tree->parent)
	{
		tree = static_cast<AVLNode*>(tree->parent);
		tree->AVLNodeHeight();
		tree->AVLNodeBalance();
	}
}