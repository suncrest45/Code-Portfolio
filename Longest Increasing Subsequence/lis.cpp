///---------------------------------------------------------------------------
/// @file lis.cpp
/// @author Aidan Straker (aidan.straker@digipen.edu)
/// @brief Find the longest, strictly, increasing subsequence of integers
/// @version 0.1
/// @date 2024-11-19
/// 
/// @copyright Copyright (c) 2024
/// 
///---------------------------------------------------------------------------
#include "lis.h"
#include <algorithm> // std::reverse


std::vector<unsigned> longest_increasing_subsequence( std::vector<int> const& sequence )
{
    // Get the size of the sequence
    int sequence_size = sequence.size();
    // The length of the longest subsequence
    int max_length = 0;
    
    // A vector keeping track of the length of the subsequences in the array
    std::vector<int>lengths(sequence_size, 1);
    // A vector keeping 
    std::vector<int>ancestors(sequence_size, -1);

    // Iterate once going ahead finding bigger values that could be added
    // To the longest increasing subsequence
    for (int i = 0; i < sequence_size; i++)
    {
        // Find the longest increasing subsequence 
        for (int j = 0; j < i; j++)
        {
            // If the element at j is less than the current element
            // Check if adding this element to the current subsequence
            // Makes it longer than the next subsequence 
            if (sequence[j] < sequence[i] && lengths[i] < lengths[j] + 1)
            {
                lengths[i] = lengths[j] + 1;
                ancestors[i] = j;
            }
        } 
    }

    max_length = lengths[0];
    int end_point = 0;
    for (int i = 1; i < sequence_size; i++)
    {
        if (lengths[i] > max_length)
        {
            max_length = lengths[i];
            end_point = i;
        }
    }
    

    // vector of indices corresponding to the LIS
    std::vector<unsigned> answer(max_length, 0); 
    int alt_length = max_length - 1;

    while (end_point != -1)
    {
        answer[alt_length--] = end_point;
        end_point = ancestors[end_point];
    }
    
    return answer;
}
