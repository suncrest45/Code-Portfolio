/// --------------------------------------------------------------------------
/// @file avl-map.h
/// @author Aidan Straker (aidan.straker@digipen.edu)
/// @brief AVL implentation based on inheriting from BST
/// @version 0.1
/// @date 2024-03-22
/// 
/// @copyright Copyright (c) 2024
/// 
///---------------------------------------------------------------------------

#include "bst-map.h" // Inheriting from BST
#include <algorithm> // std::max
#include <vector>    // std::vector
#include <deque>     // std::deque

#ifndef AVLMAP_H
#define AVLMAP_H


namespace CS280
{
    template <typename KEY_TYPE, typename VALUE_TYPE>
    #define BSTMAP BSTmap<KEY_TYPE, VALUE_TYPE>
    #define AVLMAP AVLmap<KEY_TYPE, VALUE_TYPE>
    class AVLmap : public BSTmap <KEY_TYPE, VALUE_TYPE>
    {
        public:
            AVLmap();
            VALUE_TYPE& operator[](KEY_TYPE const& key) override;
            ~AVLmap();

            class AVLNode : public BSTMAP::Node
            {
                public:
                AVLNode(KEY_TYPE k, VALUE_TYPE val, AVLNode* p, int h = 0, int b = 0, AVLNode* l = nullptr, AVLNode* r = nullptr);
                void AVLNodeHeight();
                void AVLNodeBalance();
            };
        
        private:
            std::deque <AVLNode*> m_Nodes = {};
        protected:
            typename BSTMAP::Node* InsertNode(typename BSTMAP::Node*& insertNode, const KEY_TYPE& key, typename BSTMAP::Node* parent) override;
            void DeleteNode(typename BSTMAP::Node* node) override;
            void RotateLeft(AVLNode* node);
            void RotateRight(AVLNode* node);
            void UpdateTreeHeight(AVLNode* tree);
            typename BSTMAP::Node*& FindSuccessor(AVLNode* node);
    };
}

#include "avl-map.cpp"
#endif

