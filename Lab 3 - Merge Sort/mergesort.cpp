///---------------------------------------------------------------------------
/// @file mergesort.cpp
/// @author Aidan Straker (aidan.straker@digipen.edu)
/// @brief 
/// @version 0.1
/// @date 2024-10-11
/// 
/// @copyright Copyright (c) 2024
/// 
///---------------------------------------------------------------------------
#include "mergesort.h"
#include <vector>   // std::vector

///----------------------------------------------------------------------------------
/// @brief Sorts and merges two chunks of an array into the original array
/// @param A     - The original array
/// @param L     - The left chunk of the array
/// @param R     - The right chunk of the array
/// @param left  - The beginning index of the left chunk
/// @param mid   - The midpoint of the array
/// @param right - The size of the array
///----------------------------------------------------------------------------------
void Merge(int *A, std::vector<int>& L, std::vector<int>& R, int left, int mid, int right)
{
    // The size of the first sub-array
    int n1 = mid - left + 1;
    // The size of the second sub-array
    int n2 = right - mid;
    
    // Copy the data from the first half of the original array
    for (int i = 0; i < n1; i++)
    {
        L[i] = A[left + i];
    }
    
    // Copy the data from the second half of the original array
    for (int j = 0; j < n2; j++)
    {
        R[j] = A[mid + 1 + j];
    }

    int i  = 0, j = 0;
    int k = left;

    // Merge the two temp vectors
    while (i < n1 && j < n2)
    {
        // Compare the values at the same index in the sub-arrays
        if (L[i] < R[j])
        {
            A[k] = L[i];
            i++;
        }
        else
        {
            A[k] = R[j];
            j++;
        }

        // Move to the next element
        k++;
    }
    
    // Copy the remaining elements from the left half,
    // If there are any
    while (i < n1)
    {
        A[k] = L[i];
        i++;
        k++;
    }

    // Copy the remaining elements from the right half,
    // If there are any
    while (j < n2)
    {
        A[k] = R[j];
        j++;
        k++;
    }
}

///----------------------------------------------------------------------------------
/// @brief Breaks an array into smaller chunks, sort the chunks and merges them
/// @param A - The original array
/// @param L - The left chunk of the array
/// @param R - The right chunk of the array
/// @param p - The index for the beginning of the left chunk
/// @param r - The size of the array
///----------------------------------------------------------------------------------
void MergeReal(int* A, std::vector<int>& L, std::vector<int>& R, unsigned p, unsigned r)
{
    if (p >= r)
    {
       return;
    }

    // The midpoint of a sub-array
    int mid = p + (r - p) / 2;

    // Keep breaking the array into smaller chunks
    MergeReal(A, L, R, p, mid);
    MergeReal(A, L, R, mid + 1, r);

    // Sort and merge the chunks
    Merge(A, L, R, p, mid, r);
}


///----------------------------------------------------------------------------------
/// @brief Breaks a data structure into smaller chunks, sorts them and merges them
/// @param a - A pointer to the integer array we are sorting
/// @param p - The beginning of the range we are sorting
/// @param r - The end of the range we are sorting 
///----------------------------------------------------------------------------------
void mergesort(int *a, unsigned p, unsigned r)
{
    // Create two vectors that will serve as our sub-arrays
    std::vector<int> L(r), R(r);
    // This function is the actual mergesort
    MergeReal(a, L, R, p, r - 1);
}