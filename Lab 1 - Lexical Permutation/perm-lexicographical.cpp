/// --------------------------------------------------------------------------
/// @file perm-lexicographical.cpp
/// @author Aidan Straker (aidan.straker@digipen.edu)
/// @brief 
/// @version 0.1
/// @date 2024-09-13
/// 
/// @copyright Copyright (c) 2024
/// 
///---------------------------------------------------------------------------
#include "perm-lexicographical.h"
#include <algorithm>

///------------------------------------------------------------------------
/// @brief Generate a permutation of the number stored in the vector
/// @param p - Reference to the vector holding the values
/// @return The function has finished generating all permutations (bool)
///------------------------------------------------------------------------
bool nextPermLexicographical (std::vector<int>& p) 
{

    // The vector is empty.
    if (p.size() == 0)
    {
        return false;
    }
    

    // Iterator we will use to find the next permutation
    std::vector<int>::iterator iter = p.begin();
    iter++;

    // If there is only one element return false;
    if (iter == p.end())
    {
        return false;
    }

    // Start from the back of the vector
    iter = p.end();
    --iter;

    while (true)
    {
        // Create another iterator to use for comparisons
        std::vector<int>::iterator j = iter;
        // Move iter to the second last element
        --iter;

        // Determine if the vector is in ascending order
        if (*iter < *j)
        {
            // Create an iterator pointing at the end of the vector
            std::vector<int>::iterator k = p.end();

            // Find the next largest element
            while (!(*iter < *--k)){}

            // Swap the next largest element to the front
            std::iter_swap(iter, k);
            std::reverse(j, p.end());
            return true;
            
        }

        // The vector is in descending order meaning there is no next permutation
        if (iter == p.begin())
        {
            std::reverse(p.begin(), p.end());
            return false;
        }
    }
}
