///---------------------------------------------------------------------------
/// @file knapsack-dp.cpp
/// @author Aidan Straker (aidan.straker@digipen.edu)
/// @brief 
/// @version 0.1
/// @date 2024-11-15
/// 
/// @copyright Copyright (c) 2024
/// 
///---------------------------------------------------------------------------
#include "knapsack-dp.h"
#include <iostream>
#include <numeric>
#include <algorithm> //std::max

////////////////////////////////////////////////////////////
Item::Item( int const& weight, int const& value ) 
	: weight(weight)
    , value(value) 
{}

////////////////////////////////////////////////////////////
Item::Item( Item const& original ) 
	: weight(original.weight)
    , value(original.value)
{}

////////////////////////////////////////////////////////////
std::ostream& operator<< (std::ostream& os, Item const& item)
{
	os << "(" << item.weight << " , " << item.value << ") ";
	return os;
}

////////////////////////////////////////////////////////////
std::istream& operator>> (std::istream& os, Item & item)
{
	os >> item.weight >> item.value;
	return os;
}

////////////////////////////////////////////////////////////
typedef std::vector< std::vector<int> > Table; //2-dimensional table

/// @brief Determine the best combination of items we can take based on highets value
///        with limited capacity
/// @param items - A list of items we can take
/// @param W     - The maximum weight we can carry
/// @return An array of indicies indicating the best combination 
std::vector<int> knapsackDP( std::vector<Item> const& items, int const& W )
{

    // Split the weights and values of the items into separate lists
    std::vector<int> weights, values;
    for (Item i : items)
    {
        weights.push_back(i.weight);
        values.push_back(i.value);
    }

	int num_items = items.size();
    
    // Create and initialise the table to 0
    std::vector<std::vector<int>> table(num_items + 1, std::vector<int>(W + 1, 0));   
    
    // Fill the table
    for (int i = 1; i < num_items + 1; i++)
    {
        for (int w = 1; w < W + 1; w++)
        {   
            // include or exclude the current item
            if (weights[i - 1] <= w)
            { 
                table[i][w] = std::max(table[i - 1][w], values[i - 1] + table[i - 1][w-weights[i - 1]]); 
            }
            // exclude the current item
            else 
            {
                table[i][w] = table[i - 1][w]; 
            }
        }
    }
    
    


	//print final table - for debugging?
    //do not delete this code
    if ( num_items + W < 50 ) { //print only if table is not too big
        std::cout << "   ";
        for ( int n=0; n<=num_items; ++n) { std::cout << n << "     "; }
        std::cout << "  items\n        ";
        for ( int n=0; n<num_items; ++n) { std::cout << items[n].weight << "," << items[n].value<<"   "; }
        std::cout << "\n   ";
        for ( int n=0; n<=num_items; ++n) { std::cout << "------"; }
        std::cout << std::endl;

        for ( int w=0; w<=W; ++w) {
            std::cout << w << "| ";
            for ( int n=0; n<=num_items; ++n) {
                std::cout << table[n][w] << "     ";
            }
            std::cout << std::endl;
        }
    }

    // end do not delete this code 


	// figure out which items are in the bag based on the table
	std::vector<int> bag;
    // Keep track of the current capacity 
    int remaining_capacity = W;

    // Create the table of indicies which si the best combo
    for (int i = num_items; i > 0; i--)
    {
        if (table[i][remaining_capacity] != table[i - 1][remaining_capacity])
        {
            // Item i-1 was included
            bag.push_back(i - 1);
            remaining_capacity -= weights[i - 1];
        }
    }
    
	return bag;
}

////////////////////////////////////////////////////////////
int valueBag( std::vector<Item> const& items, std::vector<int> const& bag ) {
	std::vector<int>::const_iterator it   = bag.begin(),
		                          it_e = bag.end();

	int accum = 0;
	//std::cout << "Bag ";
	for ( ; it != it_e; ++it) { 
		accum += items[ *it ].value; 
		//std::cout << *it << " ";
	}
	//std::cout << std::endl;
	return accum;
}

////////////////////////////////////////////////////////////
//prototype
//notice that auxiliary function returns value of the vector of items
//the actual vector is determined later from the table (similar to DP solution)
int knapsackRecMemAux( std::vector<Item> const&, int const&, int, Table& );

/// @brief Find the best combination of items to take based on weight and value recursively
/// @param items - The list of items we can take
/// @param W     - The maximum weight we can carry
/// @return A list of indicies for the best combination
std::vector<int> knapsackRecMem( std::vector<Item> const& items, int const& W )
{
    // Number of items in the list
	int num_items = items.size();


    // Create the table and initialise all entries to -1
    std::vector<std::vector<int>> table(num_items + 1, std::vector<int>(W + 1, -1));   

    // initialize the first row and column
    for (int w = 0; w <= W; ++w) { table[0][w] = 0; }   // no items considered
    for (int i = 0; i <= num_items; ++i) { table[i][0] = 0; }   // no capacity 

    // The helper function which does the actual work
    knapsackRecMemAux(items, W, num_items, table);

	//print table - debugging?
    //do not delete this code
    if ( num_items + W < 50 ) { //print only if table is not too big
        std::cout << "   ";
        for ( int n=0; n<=num_items; ++n) { std::cout << n << "     "; }
        std::cout << "  items\n        ";
        for ( int n=0; n<num_items; ++n) { 
            std::cout << items[n].weight << "," << items[n].value<<"   "; 
        }
        std::cout << "\n   ";
        for ( int n=0; n<=num_items; ++n) { std::cout << "------"; }
        std::cout << std::endl;

        for ( int w=0; w<=W; ++w) {
            std::cout << w << "| ";
            for ( int n=0; n<=num_items; ++n) {
                std::cout << table[n][w] << "     ";
            }
            std::cout << std::endl;
        }
    }
    //end do not delete this code 

	// Figure out which items are in the bag based on the table
	std::vector<int> bag;
    int w = W;
    for (int i = num_items; i > 0 && w > 0; --i)
    {
        if (table[i][w] != table[i - 1][w])
        {
            bag.push_back(i - 1);
            w -= items[i - 1].weight;
        }
    }

	return bag;
}

/// @brief The actual function for finding the best combo recursively
/// @param items - The list of items we can take
/// @param W     - The maximum weight we can carry
/// @param index - The position of the table to check
/// @param table - A table containing already processed possible combinations
/// @return 
int knapsackRecMemAux(std::vector<Item> const& items, int const& W, int index, Table& table) {
    // base cases
    if (index == 0 || W == 0) { return 0; }

    // check if we've done this case
    if (table[index][W] != -1) { return table[index][W]; }

    // Exclude the current item
    int exclude = knapsackRecMemAux(items, W, index - 1, table);

    // include the current item 
    int include = 0;
    if (items[index - 1].weight <= W)
    { 
        // adjust for 1-based indexing in `items`
        include = items[index - 1].value + knapsackRecMemAux(items, W - items[index - 1].weight, index - 1, table);
    }

    // store the result in the table
    table[index][W] = std::max(exclude, include);
    return table[index][W];
}
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
