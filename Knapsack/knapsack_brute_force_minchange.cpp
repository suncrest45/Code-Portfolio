///---------------------------------------------------------------------------
/// @file knapsack_brute_force_minchange.cpp
/// @author Aidan Straker (aidan.straker@digipen.edu)
/// @brief 
/// @version 0.1
/// @date 2024-10-01
///
/// @copyright Copyright (c) 2024
///
///---------------------------------------------------------------------------
#include "knapsack_brute_force_minchange.h"

///---------------------------------------------------------------------------
/// @brief Our method for determining all the best items to steal
/// @param s - The number of items we can possibly steal
///---------------------------------------------------------------------------
GreyCode::GreyCode(int s) : numElements(s), current(1), code(0) {}

///---------------------------------------------------------------------------
/// @brief  Determine if an item is worth stealing, the position it should take in 
///         our bag of ill-gotten gains, and is this last item we should steal
/// @return A pair of pairs
///---------------------------------------------------------------------------
std::pair< bool, std::pair< bool, int > > GreyCode::Next() 
{
    // The position this item should be placed in our bag of ill-gotten gains
    int pos = 0;
    // This something is a faster method of finding a position in our bag
    int something = current;
    // Should we bother stealing this item in the first place?
    bool add = false;

    // Have we encountered the last item to steal?
   const bool last = (current >= (1 << numElements) - 1);

    // If we have found the last item, stop looking for more items, we have picked the place dry
    if (last)
    {
        // No more items to steal
        return std::make_pair(false, std::make_pair(false, pos));
    }

    // Get the position of the changed bit
    while (something % 2 == 0)
    {
        pos++;
        something /= 2;
    }
    
    // Determine if we are adding or removing an item from our bag
    // If the bit was 0 we are adding the item
    add = !(1 & (code >> pos));

    // Toggle bit
    code ^= 1 << pos;

    // Move to next item to steal
    current++;

    return std::make_pair(true, std::make_pair(add, pos));
}

///---------------------------------------------------------------------------
/// @brief  We have finished overthinking our best haul
/// @return Our best haul written down
///---------------------------------------------------------------------------
std::vector<bool> GreyCode::assign()
{
    std::vector<bool> santa_illgotten_gains(numElements, false);

    for (int i = 0; i < numElements; i++)
    {
        santa_illgotten_gains[i] = (code & 1<< i);
    }
    
    return santa_illgotten_gains;
}

///---------------------------------------------------------------------------
/// @brief A code representation of how a thief steals a list of items
/// @param items - The items to steal
/// @param W     - How much the thief can steal in terms of weight
/// @return A the best combination of items to steal based off the values and weights of the items to steal
///---------------------------------------------------------------------------
std::vector<bool> knapsack_brute_force( std::vector<Item> const& items, Weight const& W )
{
    int size = items.size();

    // Our thief's bag of ill-gotten gains
    std::vector<bool> bag(size, false ); 

    // Begin overthinking
    GreyCode gc(size);

    bool burglarise = true;

    // A running tally of your ill-gotten gains weight
    Weight combinedWeight;
    // A running tally of your ill-gotten gains value
    int totalValue = 0;
    // A previous burglary attempt's value
    int oldValue = 0;

    // Keep overthinking
    while (burglarise)
    {
        std::pair<bool, std::pair<bool, int>> result = gc.Next();
        // Have we found everything worth stealing?
        burglarise = result.first;

        // Should we bother "acquiring" this item?
        bool add = result.second.first;
        // Where in the bag you should place this ill-gotten item
        int pos = result.second.second;

        // Our thiefly intuition told us we should add this item to our bag
        if (add)
        {
            // Keep a tally of our haul's weight
            combinedWeight += items[pos].GetWeight();
            // Keep a tally of our haul's value
            totalValue += items[pos].GetValue();
        }
        // If we are not adding an item reduce our haul's weight
        else
        {
            combinedWeight -= items[pos].GetWeight();
            totalValue -= items[pos].GetValue();
        }

        
        // Have we found the best combination of items to steal?
        if ((combinedWeight <= W) && (totalValue > oldValue))
        {
            oldValue = totalValue;
            bag = gc.assign();
        }
    }
    
    // The best combination of items to steal
    return bag;
}
